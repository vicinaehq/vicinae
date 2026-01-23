import { randomUUID } from "node:crypto";
import * as fs from "node:fs";
import { isatty } from "node:tty";
import { isMainThread, Worker } from "node:worker_threads";
import { main as workerMain } from "./worker";
import * as ipc from "./proto/ipc";
import type * as common from "./proto/common";
import * as manager from "./proto/manager";
import * as path from "node:path";
import * as fsp from "node:fs/promises";
import type { EnvironmentType } from "./types";

const WORKER_GRACE_PERIOD_MS = 10_000;
const WORKER_MAX_HEAP_SIZE_MB = 1000; // really high limit just to make sure an extension command can't exhaust RAM by itself

type WorkerInfo = {
	worker: Worker;
	pendingTermination?: NodeJS.Timeout;
};

class Vicinae {
	private readonly workerPool: Worker[] = [];
	private readonly workerMap = new Map<string, WorkerInfo>();
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

	private acquireWorker(env: manager.CommandEnv): Worker {
		if (env === manager.CommandEnv.Development) {
			// we create a new development worker on the fly all the time
			// for development extensions. This is because the worker preloads
			// React and we need to know whether we want the dev or prod version
			// from the get go. We don't preload a dev worker to keep resource
			// usage low.
			return this.createWorker("development");
		}

		if (!this.workerPool.length) {
			console.error("no worker in pool!");
		}

		const acquired = this.workerPool.pop() ?? this.createWorker("production");

		// immediately prepare a new worker for next time we launch an extension
		// we don't reuse the same worker twice to guarantee isolation
		this.workerPool.push(this.createWorker("production"));

		return acquired;
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

			const stdoutLog = path.join(supportInternal, "stdout.txt");
			const stderrLog = path.join(supportInternal, "stderr.txt");

			const worker = this.acquireWorker(load.env);
			const workerInfo: WorkerInfo = { worker };

			this.workerMap.set(sessionId, workerInfo);

			worker.on("messageerror", (error) => {
				console.error(error);
			});

			const sendCrash = (text: string) => {
				this.writeMessage({
					extensionEvent: {
						sessionId,
						event: { id: randomUUID(), crash: { text } },
					},
				});
			};

			worker.on("error", (error) => {
				sendCrash(this.formatError(error));
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
					sendCrash(
						`The extension manager process received a malformed request.\nThis most likely indicates a problem with Vicinae, not the extension.\nPlease file a bug report: https://github.com/vicinaehq/vicinae/issues/new`,
					);
					worker.terminate();
				}
			});

			const stdoutStream = fs.createWriteStream(stdoutLog);
			const stderrStream = fs.createWriteStream(stderrLog);

			worker.stdout.on("data", async (buf: Buffer) => {
				stdoutStream.write(buf);
			});

			worker.stderr.on("data", async (buf: Buffer) => {
				stderrStream.write(buf);
			});

			worker.on("error", (error) => {
				console.error(`worker error: ${error.name}:${error.message}`);
			});

			worker.on("exit", (code) => {
				console.error(`Worker exited with code ${code}`);

				// any exit is considered as a crash if not flagged for termination
				if (!workerInfo.pendingTermination) {
					sendCrash(`Extension exited prematurely with exit code ${code}`);
				} else {
					clearTimeout(workerInfo.pendingTermination);
				}

				stdoutStream.close();
				stderrStream.close();
				this.workerMap.delete(sessionId);
			});

			worker.postMessage(
				ipc.ExtensionMessage.encode({
					event: {
						id: randomUUID(),
						launch: {
							entrypoint: load.entrypoint,
							preferenceValues: load.preferenceValues,
							argumentValues: load.argumentValues,
							mode: load.mode,
							supportPath,
							assetPath: assetsPath,
							isRaycast: load.isRaycast,
							extensionName: load.extensionName,
							ownerOrAuthorName: load.ownerOrAuthorName,
							commandName: load.commandName,
						},
					},
				}).finish(),
			);

			return this.respond(request.requestId, { load: { sessionId } });
		}

		if (request.payload?.unload) {
			const { sessionId } = request.payload.unload;
			const workerInfo = this.workerMap.get(sessionId);

			if (!workerInfo) {
				return this.respondError(request.requestId, {
					errorText: `No running command with session ${sessionId}`,
				});
			}

			workerInfo.worker.postMessage(
				ipc.ExtensionMessage.encode({
					event: { id: "shutdown", generic: { json: "[]" } },
				}).finish(),
			);

			// force kill after 10s
			workerInfo.pendingTermination = setTimeout(() => {
				workerInfo.worker.terminate();
			}, WORKER_GRACE_PERIOD_MS);

			return this.respond(request.requestId, { ack: {} });
		}

		return this.respondError(request.requestId, {
			errorText: "No handler configured for this command",
		});
	}

	private createWorker(environment: EnvironmentType): Worker {
		return new Worker(__filename, {
			stdout: true,
			stderr: true,
			resourceLimits: {
				maxOldGenerationSizeMb: WORKER_MAX_HEAP_SIZE_MB,
			},
			workerData: {
				environment,
			},
			env: {
				...process.env,
			},
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
				worker.worker.postMessage(
					ipc.ExtensionMessage.encode({ event: extensionEvent.event }).finish(),
				);
			}
		}

		if (extensionResponse) {
			const worker = this.workerMap.get(extensionResponse.sessionId);

			if (worker) {
				worker.worker.postMessage(
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
		this.workerPool.push(this.createWorker("production"));
		process.stdin.on("error", (error) => {
			throw new Error(`${error}`);
		});
		process.stdin.on("data", (buf) => this.handleRead(buf));
	}
}

const main = async () => {
	if (!isMainThread) {
		workerMain();
		return;
	}

	if (isatty(process.stdout.fd)) {
		console.error("Running the extension manager from a TTY is not supported.");
		process.exit(1);
	}

	new Vicinae();
};

main();
