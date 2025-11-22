import { randomUUID } from "node:crypto";
import { existsSync } from "node:fs";
import { isatty } from "node:tty";
import { isMainThread, Worker } from "node:worker_threads";
import { main as workerMain } from "./worker";
import * as ipc from "./proto/ipc";
import type * as common from "./proto/common";
import * as manager from "./proto/manager";
import * as extension from "./proto/extension";
import * as path from "node:path";
import * as fsp from "node:fs/promises";

class Vicinae {
	private readonly workerMap = new Map<string, Worker>();
	private readonly requestMap = new Map<string, Worker>();
	private currentMessage: { data: Buffer } = {
		data: Buffer.from(""),
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
		this.writeMessage({ managerResponse: { requestId, value } });
	}

	private writeMessage(message: ipc.IpcMessage) {
		const buf = Buffer.from(ipc.IpcMessage.encode(message).finish());
		this.writePacket(buf);
	}

	private respondError(requestId: string, error: common.ErrorResponse) {
		this.writeMessage({ managerResponse: { requestId, error } });
	}

	private parseMessage(packet: Buffer): ipc.IpcMessage {
		return ipc.IpcMessage.decode(packet);
	}

	private async handleManagerRequest(request: ipc.ManagerRequest) {
		if (request.payload?.load) {
			const load = request.payload.load;
			const sessionId = randomUUID();
			const supportPath = path.join(
				load.vicinaePath,
				"support",
				load.extensionId,
			);
			const supportInternal = path.join(supportPath, ".vicinae"); // for log stream, cli pid file...
			const assetsPath = path.join(
				load.vicinaePath,
				"extensions",
				load.extensionId,
				"assets",
			);

			await Promise.all([
				fsp.mkdir(assetsPath, { recursive: true }),
				fsp.mkdir(supportInternal, { recursive: true }),
			]);

			const devLogPath = path.join(supportInternal, "dev.log");
			const shouldLog = existsSync(devLogPath);
			const worker = new Worker(__filename, {
				workerData: {
					// the transpiled JS file to execute
					entrypoint: load.entrypoint,
					preferenceValues: load.preferenceValues,
					launchProps: { arguments: load.argumentValues },
					commandMode:
						load.mode === manager.CommandMode.View ? "view" : "no-view",
					supportPath,
					assetsPath,
					isRaycast: load.isRaycast,
					extensionName: load.extensionName,
					ownerOrAuthorName: load.ownerOrAuthorName,
					commandName: load.commandName,
					vicinaeVersion: {
						tag: process.env.VICINAE_VERSION ?? "unknown",
						commit: process.env.VICINAE_COMMIT ?? "unknown",
					},
				},

				stdout: true,
				env: {
					...process.env,
					NODE_ENV:
						load.env === manager.CommandEnv.Development
							? "development"
							: "production",
				},
			});

			this.workerMap.set(sessionId, worker);

			worker.on("messageerror", (error) => {
				console.error(error);
			});

			worker.on("error", (error) => {
				const crash = extension.CrashEventData.create({
					text: this.formatError(error),
				});
				const event = ipc.QualifiedExtensionEvent.create({
					sessionId,
					event: { id: randomUUID(), crash },
				});

				this.writeMessage({ extensionEvent: event });
				console.error(`worker error`, error);
			});

			worker.on("online", () => { });

			worker.on("message", (buf: Buffer) => {
				try {
					const { event, request } = ipc.ExtensionMessage.decode(buf);

					/**
					 * Here we qualify the request or event by appending to it the runtime session id
					 * which is only known to us. Extensions cannot forge one themselves.
					 */

					if (request) {
						this.requestMap.set(request.requestId, worker);
						this.writeMessage({ extensionRequest: { sessionId, request } });
						return;
					}

					if (event) {
						if (event.crash) {
							this.workerMap.delete(sessionId);
							worker.terminate();
						}

						this.writeMessage({ extensionEvent: { sessionId, event } });
					}
				} catch (_) {
					const crash = extension.CrashEventData.create({
						text: `The extension manager process received a malformed request.\nThis most likely indicates a problem with Vicinae, not the extension.\nPlease file a bug report: https://github.com/vicinaehq/vicinae/issues/new`,
					});
					const event = ipc.QualifiedExtensionEvent.create({
						sessionId,
						event: { id: randomUUID(), crash },
					});

					this.writeMessage({ extensionEvent: event });
					this.workerMap.delete(sessionId);
					worker.terminate();
				}
			});

			worker.stdout.on("data", async (buf: Buffer) => {
				//console.error(buf.toString());
				if (shouldLog) await fsp.appendFile(devLogPath, buf);
			});

			worker.stderr.on("data", async (buf: Buffer) => {
				if (shouldLog) await fsp.appendFile(devLogPath, buf);
				else console.error(buf.toString());
			});

			worker.on("error", (error) => {
				console.error(`worker error: ${error.name}:${error.message}`);
			});

			worker.on("exit", (_) => {
				this.workerMap.delete(sessionId);
			});

			return this.respond(request.requestId, { load: { sessionId } });
		}

		if (request.payload?.unload) {
			const { sessionId } = request.payload.unload;
			const worker = this.workerMap.get(sessionId);

			if (!worker) {
				return this.respondError(request.requestId, {
					errorText: `No running command with session ${sessionId}`,
				});
			}

			if (worker) {
				this.workerMap.delete(sessionId);
				await worker.terminate();
			}

			return this.respond(request.requestId, { ack: {} });
		}

		return this.respondError(request.requestId, {
			errorText: "No handler configured for this command",
		});
	}

	private async routeMessage(message: ipc.IpcMessage) {
		const { managerRequest, extensionEvent, extensionResponse } = message;

		if (managerRequest) {
			this.handleManagerRequest(managerRequest);
		}

		if (extensionEvent) {
			const worker = this.workerMap.get(extensionEvent.sessionId);

			if (worker) {
				worker.postMessage(
					ipc.ExtensionMessage.encode({ event: extensionEvent.event }).finish(),
				);
			}
		}

		if (extensionResponse) {
			const worker = this.workerMap.get(extensionResponse.sessionId);

			if (worker) {
				worker.postMessage(
					ipc.ExtensionMessage.encode({
						response: extensionResponse.response,
					}).finish(),
				);
			}
		}
	}

	handleRead(data: Buffer) {
		this.currentMessage.data = Buffer.concat([this.currentMessage.data, data]);

		while (this.currentMessage.data.length >= 4) {
			const length = this.currentMessage.data.readUInt32BE();
			const isComplete = this.currentMessage.data.length - 4 >= length;

			//console.error('read message: length', length);

			if (!isComplete) return;

			const packet = this.currentMessage.data.subarray(4, length + 4);
			const message = this.parseMessage(packet);

			//console.error('routing message');

			this.routeMessage(message);
			this.currentMessage.data = this.currentMessage.data.subarray(length + 4);
		}
	}

	constructor() {
		process.stdin.on("error", (error) => {
			throw new Error(`${error}`);
		});
		process.stdin.on("data", (buf) => this.handleRead(buf));
	}
}

const main = async () => {
	if (!isMainThread) workerMain();

	if (isatty(process.stdout.fd)) {
		console.error("Running the extension manager from a TTY is not supported.");
		process.exit(1);
	}

	const vicinae = new Vicinae();
};

main();
