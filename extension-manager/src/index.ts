import { randomUUID } from 'crypto';
import { isMainThread, Worker } from "worker_threads";
import { main as workerMain } from './worker';
import { isatty } from "tty";

import * as ipc from './proto/ipc';
import * as common from './proto/common';
import * as manager from './proto/manager';
import * as extension from './proto/extension';

class Omnicast {
	private readonly workerMap = new Map<string, Worker>;
	private readonly requestMap = new Map<string, Worker>;
	private currentMessage: { data: Buffer }= {
		data: Buffer.from(''),
	};

	private formatError(error: Error) {
		return `${error.stack}`;
	}

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);
		
		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);
		process.stdout.write(packet);
	}

	private respond(requestId: string, value: manager.ResponseData) {
		this.writeMessage({ managerResponse: { requestId, value} });
	}

	private writeMessage(message: ipc.IpcMessage) {
		const buf = Buffer.from(ipc.IpcMessage.encode(message).finish());
		this.writePacket(buf);
	}

	private respondError(requestId: string, error: common.ErrorResponse) {
		this.writeMessage({managerResponse: {requestId, error}});
	}

	private parseMessage(packet: Buffer): ipc.IpcMessage {
		return ipc.IpcMessage.decode(packet);
	}

	private async handleManagerRequest(request: ipc.ManagerRequest) {
		if (request.payload?.load) {
			console.error('load command from extension manager');
			const load = request.payload.load;

			const sessionId = randomUUID();

			const worker = new Worker(__filename, {
				workerData: {
					// the transpiled JS file to execute
					entrypoint: load.entrypoint,
					preferenceValues: load.preferenceValues,
					launchProps: { arguments: load.argumentValues },
					commandMode: load.mode == manager.CommandMode.View ? "view" : "no-view"
				},
				stdout: true,
				env: {
					'NODE_ENV': load.env == manager.CommandEnv.Development ? 'development' : 'production',
					'RECONCILER_TRACE': process.env.RECONCILER_TRACE,
				}
			});

			this.workerMap.set(sessionId, worker);
			
			worker.on('messageerror', (error) => {
				console.error(error);
			});

			worker.on('error', (error) => {
				const crash = extension.CrashEventData.create({ text: this.formatError(error) });
				const event = ipc.QualifiedExtensionEvent.create({ sessionId, event: { id: randomUUID(), crash } });

				this.writeMessage({ extensionEvent: event });
				console.error(`worker error`, error);
			});

			worker.on('online', () => {
				console.error(`worker is online`);
			});

			worker.on('message', (buf: Buffer) => {
				console.error(buf);
				try {
					const { event, request } = ipc.ExtensionMessage.decode(buf);

					/**
					 * Here we qualify the request or event by appending to it the runtime session id
					 * which is only known to us. Extensions cannot forge one themselves.
					 */

					if (request) {
						this.requestMap.set(request.requestId, worker);
						this.writeMessage({ extensionRequest: { sessionId, request } });
						return ;
					}

					if (event) {
						if (event.crash) {
							this.workerMap.delete(sessionId);
							worker.terminate();
						}

						this.writeMessage({ extensionEvent: { sessionId, event }});
					}
				} catch (error) {
					const crash = extension.CrashEventData.create({ text: `The extension manager process received a malformed request.\nThis most likely indicate a problem with the software itself, not the extension.\nPlease file a bug report.` });
					const event = ipc.QualifiedExtensionEvent.create({ sessionId, event: { id: randomUUID(), crash } });

					this.writeMessage({ extensionEvent: event });
					this.workerMap.delete(sessionId);
					worker.terminate();
				}

				//console.error(`from worker`, { qualifiedPayload });
				//console.error(`[DEBUG] forward event type ${qualifiedPayload.envelope.action}`);
			});

			worker.stdout.on('data', async (buf: Buffer) => {
				console.error(buf.toString());
				//await appendFile(join(extension.path, "dev.log"), buf)
			});

			worker.stderr.on('data', async (buf: Buffer) => {
				console.error(buf.toString());
				//await appendFile(join(extension.path, "dev.log"), buf)
			});

			worker.on('error', (error) => { 
				console.error(`error is of type ${typeof error}: instance of error ${error instanceof Error}`);
				console.error(`worker error: ${error.name}:${error.message}`); 
			});

			worker.on('exit', (code) => {
				console.error(`worker exited with code ${code}`)
				this.workerMap.delete(sessionId);
			});

			return this.respond(request.requestId, { load: { sessionId } });
		}

		if (request.payload?.unload) {
			const { sessionId } = request.payload.unload;
			const worker = this.workerMap.get(sessionId);

			if (!worker) {
				return this.respondError(request.requestId, { errorText: `No running command with session ${sessionId}` });
			}

			if (worker) {
				this.workerMap.delete(sessionId);
				await worker.terminate();
			}

			return this.respond(request.requestId, { ack: {} });

		}

		return this.respondError(request.requestId, { errorText: "No handler configured for this command" });
	}

	private async routeMessage(message: ipc.IpcMessage) {
		const { managerRequest, extensionEvent, extensionResponse } = message;

		console.error(JSON.stringify({ message }, null, 2));

		if (managerRequest) {
			this.handleManagerRequest(managerRequest);
		}
		
		if (extensionEvent) {
			const worker = this.workerMap.get(extensionEvent.sessionId);

			console.error('extension event', extensionEvent.event?.id, 'has worker', !!worker);
			
			if (worker) {
				worker.postMessage(ipc.ExtensionMessage.encode({ event: extensionEvent.event }).finish());
			}
		}

		if (extensionResponse) {
			const worker = this.workerMap.get(extensionResponse.sessionId);
			
			if (worker) {
				worker.postMessage(ipc.ExtensionMessage.encode({ response: extensionResponse.response }).finish());
			}
		}
	}

	handleRead(data: Buffer) {
		this.currentMessage.data = Buffer.concat([this.currentMessage.data, data]);


		while (this.currentMessage.data.length >= 4) {
			const length = this.currentMessage.data.readUInt32BE();
			const isComplete = this.currentMessage.data.length - 4 >= length;

			console.error('read message: length', length);

			if (!isComplete) return ;

			const packet = this.currentMessage.data.subarray(4, length + 4);
			const message = this.parseMessage(packet);

			console.error('routing message');

			this.routeMessage(message);
			this.currentMessage.data = this.currentMessage.data.subarray(length + 4);
		}
	}

	constructor() {
		console.error('init extman');
		process.stdin.on('error', (error) => {
			throw new Error(`${error}`);
		});
		process.stdin.on('data', (buf) => this.handleRead(buf));
	}
};

const main = async () => {
	if (!isMainThread) workerMain();

	if (isatty(process.stdout.fd)) {
		console.error('Running the extension manager from a TTY is not supported.');
		process.exit(1);
	}

	const omnicast = new Omnicast()
}

main();
