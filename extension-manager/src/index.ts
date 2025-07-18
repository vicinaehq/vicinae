import { randomUUID } from 'crypto';
import { isMainThread, Worker } from "worker_threads";
import { main as workerMain } from './worker';
import { isatty } from "tty";
import { CommandEnv, CommandMode, IpcMessage, ManagerRequest, ManagerLoadResponseData, ManagerResponse, ErrorResponseData, ExtensionEvent, ExtensionMessage, QualifiedExtensionRequest, QualifiedExtensionEvent, ManagerResponseData, AckResponse } from './protocols/extension';

class Omnicast {
	private readonly workerMap = new Map<string, Worker>;
	private readonly requestMap = new Map<string, Worker>;
	private currentMessage: { data: Buffer }= {
		data: Buffer.from(''),
	};

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);
		
		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);
		process.stdout.write(packet);
	}

	private respond(requestId: string, value: ManagerResponseData) {
		const response = ManagerResponse.create({ requestId, value });
		const message = IpcMessage.create({ managerResponse: response })
		const data = IpcMessage.encode(message).finish();

		this.writePacket(Buffer.from(data));
	}

	private respondError(requestId: string, error: ErrorResponseData) {
		const response = ManagerResponse.create({ requestId, error });
		const message = IpcMessage.create({ managerResponse: response })
		const data = IpcMessage.encode(message).finish();

		this.writePacket(Buffer.from(data));
	}

	private parseMessage(packet: Buffer): IpcMessage {
		return IpcMessage.decode(packet);
	}

	private async handleManagerRequest(request: ManagerRequest) {
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
					commandMode: load.mode == CommandMode.View ? "view" : "no-view"
				},
				stdout: true,
				env: {
					'NODE_ENV': load.env == CommandEnv.Development ? 'development' : 'production',
				}
			});

			this.workerMap.set(sessionId, worker);
			
			worker.on('messageerror', (error) => {
				console.error(error);
			});

			worker.on('error', (error) => {
				console.error(`worker error`, error);
			});

			worker.on('online', () => {
				console.error(`worker is online`);
			});

			worker.on('message', (buf: Buffer) => {
				const { event, request } = ExtensionMessage.decode(buf);

				/**
				 * Here we qualify the request or event by appending to it the runtime session id
				 * which is only known to us. Extensions cannot forge one themselves.
				 */

				if (request) {
					const message = QualifiedExtensionRequest.encode({ sessionId, request }).finish();

					this.requestMap.set(request.requestId, worker);
					this.writePacket(Buffer.from(message));
					return ;
				}

				if (event) {
					const message = QualifiedExtensionEvent.encode({ sessionId, event }).finish();

					this.writePacket(Buffer.from(message));
				}

				//console.error(`from worker`, { qualifiedPayload });
				//console.error(`[DEBUG] forward event type ${qualifiedPayload.envelope.action}`);
			});

			worker.stdout.on('data', async (buf) => {
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

			let res = ManagerLoadResponseData.create({ sessionId });

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

	private async routeMessage(message: IpcMessage) {
		const { managerRequest, extensionEvent } = message;
		
		if (managerRequest) {
			this.handleManagerRequest(managerRequest);
		}
		
		if (extensionEvent) {
			const worker = this.workerMap.get(extensionEvent.sessionId);
			
			if (worker) {
				worker.postMessage(ExtensionMessage.encode({ event: extensionEvent.event }).finish());
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
