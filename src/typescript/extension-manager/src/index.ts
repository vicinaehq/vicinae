import { randomUUID } from "node:crypto";
import * as fs from "node:fs";
import { isatty } from "node:tty";
import { isMainThread, Worker } from "node:worker_threads";
import * as fsp from "node:fs/promises";
import { main as workerMain } from "./worker";

import * as manager from "./proto/manager";
import * as extension from "./proto/manager-extension";
import * as path from "node:path";

import type { EnvironmentType } from "./types";
import { Logger } from "./logger";
import { setTimeout } from "node:timers";

const WORKER_GRACE_PERIOD_MS = 5000;
const WORKER_MAX_HEAP_SIZE_MB = 1000; // really high limit just to make sure an extension command can't exhaust RAM by itself

type WorkerStatus = "unloading" | "running" | "awaiting_handshake";

type WorkerInfo = {
	worker: Worker;
	pendingTermination?: NodeJS.Timeout;
	client: extension.Client;
	status: WorkerStatus;
	payload: extension.LaunchEventData;
	displayId: string; // <extension_id>:<command_id>
	startedAt?: bigint; // worker info is set on start (after handshake), therefore it starts as undefined
};

export const logger = new Logger();

class ExtensionManager extends manager.ManagerService {
	constructor(transport: manager.RpcTransport) {
		super(transport);
		this.workerPool.push(this.createWorker("production"));
	}

	createExtensionClient(worker: Worker) {
		const transport = new extension.RpcTransport({
			send: (data) => worker.postMessage(data),
		});

		return new extension.Client(transport);
	}

	async ready(sessionId: string): Promise<boolean> {
		const worker = this.workerMap.get(sessionId);

		if (!worker) return false;

		worker.status = "running";
		await worker.client.Lifecycle.launch(worker.payload);
		worker.startedAt = process.hrtime.bigint();

		logger.info(
			`Started extension ${worker.payload.extension_name}:${worker.payload.command_name}`,
		);

		return true;
	}

	async load(load: manager.LoadOptions): Promise<manager.LoadResponse> {
		const sessionId = randomUUID();
		const worker = this.acquireWorker(load.env);
		const client = this.createExtensionClient(worker);
		const supportPath = path.join(
			load.vicinae_path,
			"support",
			load.extension_id,
		);
		const supportInternal = path.join(supportPath, ".vicinae"); // for log stream, cli pid file...

		const stdoutLog = path.join(supportInternal, "stdout.txt");
		const stderrLog = path.join(supportInternal, "stderr.txt");
		const assetsPath = path.join(
			load.vicinae_path,
			"extensions",
			load.extension_id,
			"assets",
		);

		await Promise.all([
			fsp.mkdir(assetsPath, { recursive: true }),
			fsp.mkdir(supportInternal, { recursive: true }),
		]);

		const workerInfo: WorkerInfo = {
			worker,
			client,
			status: "awaiting_handshake",
			displayId: `${load.extension_id}:${load.command_name}`,
			payload: {
				entrypoint: load.entrypoint,
				argumentValues: load.arguments,
				preferenceValues: load.preferences,
				mode: load.mode,
				support_path: supportPath,
				asset_path: assetsPath,
				is_raycast: load.is_raycast,
				extension_name: load.extension_name,
				owner_or_author_name: load.owner_or_author_name,
				command_name: load.command_name,
				launch_type: load.launch_type,
				capabilities: load.capabilities,
			},
		};

		this.workerMap.set(sessionId, workerInfo);

		client.Lifecycle.unload_requested(() => {
			this.unload(sessionId);
		});

		worker.on("message", (data) => {
			client.route(data); // try routing to us
			if (workerInfo.status !== "running") return;
			this.emit_extensionMessage(sessionId, data); // regular extension stuff
		});

		worker.on("messageerror", (error) => {
			logger.error(error.toString());
		});

		const sendCrash = (text: string) => {
			if (workerInfo.status !== "running") return;
			this.emit_extensionCrash(sessionId, text);
		};

		worker.on("error", (error) => {
			sendCrash(`${error.stack}`);
			logger.error(`worker error: ${error}`);
		});

		worker.on("online", () => { });

		const stdoutStream = fs.createWriteStream(stdoutLog);
		const stderrStream = fs.createWriteStream(stderrLog);

		worker.stdout.on("data", async (buf: Buffer) => {
			stdoutStream.write(buf);
		});

		worker.stderr.on("data", async (buf: Buffer) => {
			stderrStream.write(buf);
		});

		worker.on("error", (error) => {
			logger.error(`worker error: ${error.name}:${error.message}`);
		});

		worker.on("exit", (code) => {
			logger.info(`Worker ${workerInfo.displayId} exited with code ${code}`);

			stdoutStream.close();
			stderrStream.close();

			// an exit while we are not deliberately unloading is a crash
			if (workerInfo.status !== "unloading") {
				sendCrash(`Extension exited prematurely with exit code ${code}`);
			} else if (workerInfo.pendingTermination) {
				// worker exited on its own before the grace period elapsed
				clearTimeout(workerInfo.pendingTermination);
			}

			this.workerMap.delete(sessionId);
		});

		logger.info(`Loaded extension ${workerInfo.displayId}`);

		// if handshake is not completed 1 second (!) later we auto unload the worker
		// we don't expect that to happen but we provide this as a safeguard
		setTimeout(() => {
			const worker = this.workerMap.get(sessionId);
			if (worker?.status === "awaiting_handshake") {
				logger.error(
					`worker for command ${worker.displayId} did not complete handshake under 1s, unloading...`,
				);
				this.unload(sessionId);
			}
		}, 1000);

		return { session_id: sessionId };
	}

	async unload(session_id: string): Promise<boolean> {
		const workerInfo = this.workerMap.get(session_id);

		if (!workerInfo) {
			// FIXME: no-view interval scheduler will try to unload non existent extension when
			// launching a new instance, resulting in a misleading error message. Eventually, we just
			// need to watch for unload on the C++ side.
			/*	
			logger.error(
				`Failed to unload extension with session id ${session_id}: no such worker`,
			);
			*/
			return false;
		}

		workerInfo.status = "unloading";
		await workerInfo.client.Lifecycle.shutdown();

		if (workerInfo.payload.mode === "NoView") {
			workerInfo.worker.terminate();
		} else {
			// FIXME: we let view commands some time to execute their useEffect cleanup, if they need to.
			// Currently this has one significant caveat: vicinae won't process incoming requests during the
			// grace period to avoid unexpected behavior.
			workerInfo.pendingTermination = setTimeout(() => {
				workerInfo.worker.terminate();
			}, WORKER_GRACE_PERIOD_MS);
		}

		const end = process.hrtime.bigint();
		const elapsed = end - (workerInfo.startedAt ?? end);
		const elapsedMs = parseInt((elapsed / BigInt(1e6)).toString(), 10);
		const elapsedSeconds = elapsedMs / 1e3;

		logger.info(
			`Unloaded extension ${workerInfo.displayId} (ran for ${elapsedSeconds}s)`,
		);

		return true;
	}

	async messageExtension(
		session_id: string,
		payload: string,
	): Promise<boolean> {
		const worker = this.workerMap.get(session_id);
		worker?.client.Lifecycle.send_message(payload);
		return true;
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

	private acquireWorker(env: manager.CommandEnv): Worker {
		if (env === "Development") {
			// we create a new development worker on the fly all the time
			// for development extensions. This is because the worker preloads
			// React and we need to know whether we want the dev or prod version
			// from the get go. We don't preload a dev worker to keep resource
			// usage low.
			return this.createWorker("development");
		}

		if (!this.workerPool.length) {
			logger.error("no worker in pool!");
		}

		const acquired = this.workerPool.pop() ?? this.createWorker("production");

		// immediately prepare a new worker for next time we launch an extension
		// we don't reuse the same worker twice to guarantee isolation
		this.workerPool.push(this.createWorker("production"));

		return acquired;
	}

	private readonly workerPool: Worker[] = [];
	private readonly workerMap = new Map<string, WorkerInfo>();
}

class Vicinae {
	private currentMessage: { data: Buffer } = {
		data: Buffer.from(""),
	};

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);

		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);
		process.stdout.write(packet);
	}

	handleRead(data: Buffer) {
		this.currentMessage.data = Buffer.concat([this.currentMessage.data, data]);

		while (this.currentMessage.data.length >= 4) {
			const length = this.currentMessage.data.readUInt32BE();
			const isComplete = this.currentMessage.data.length - 4 >= length;

			if (!isComplete) return;

			const packet = this.currentMessage.data.subarray(4, length + 4);

			this.server.route(packet.toString("utf8"))?.catch((error) => {
				logger.error(`Uncaught exception from handler: ${error}`);
			});

			this.currentMessage.data = this.currentMessage.data.subarray(length + 4);
		}
	}

	constructor() {
		const rpc = new manager.RpcTransport({
			send: (data) => {
				this.writePacket(Buffer.from(data));
			},
		});

		this.server = new manager.Server(rpc, new ExtensionManager(rpc));

		process.stdin.on("error", (error) => {
			throw new Error(`${error}`);
		});
		process.stdin.on("data", (buf) => this.handleRead(buf));
	}

	private server: manager.Server;
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
