import { randomUUID } from "node:crypto";
import * as fs from "node:fs";
import { isatty } from "node:tty";
import { isMainThread, Worker } from "node:worker_threads";
import { main as workerMain } from "./worker";

import * as manager from "./proto/manager";
import * as extension from "./proto/manager-extension";
import * as path from "node:path";
import * as fsp from "node:fs/promises";

import type { EnvironmentType } from "./types";

const WORKER_GRACE_PERIOD_MS = 10_000;
const WORKER_MAX_HEAP_SIZE_MB = 1000; // really high limit just to make sure an extension command can't exhaust RAM by itself

type WorkerInfo = {
	worker: Worker;
	pendingTermination?: NodeJS.Timeout;
	client: extension.Client;
};

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

	async load(load: manager.LoadOptions): Promise<manager.LoadResponse> {
		console.error("loading", { load });
		const sessionId = randomUUID();
		const supportPath = path.join(
			load.vicinae_path,
			"support",
			load.extension_id,
		);
		const supportInternal = path.join(supportPath, ".vicinae"); // for log stream, cli pid file...
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

		const stdoutLog = path.join(supportInternal, "stdout.txt");
		const stderrLog = path.join(supportInternal, "stderr.txt");

		const worker = this.acquireWorker(load.env);
		const client = this.createExtensionClient(worker);
		const workerInfo: WorkerInfo = { worker, client };

		client.Lifecycle.extension_message((message) => {
			this.emit_extensionMessage(sessionId, message);
		});

		this.workerMap.set(sessionId, workerInfo);

		worker.on("message", (data) => {
			const { result } = JSON.parse(data);
			if (result !== undefined) client.route(data); // response
			this.emit_extensionMessage(sessionId, data); // regular extension stuff
		});

		worker.on("messageerror", (error) => {
			console.error(error);
		});

		const sendCrash = (text: string) => {
			this.emit_extensionCrash(sessionId, text);
		};

		worker.on("error", (error) => {
			sendCrash(`${error.stack}`);
			console.error(`worker error`, error);
		});

		worker.on("online", () => { });

		const stdoutStream = fs.createWriteStream(stdoutLog);
		const stderrStream = fs.createWriteStream(stderrLog);

		worker.stdout.on("data", async (buf: Buffer) => {
			console.error(buf.toString());
			stdoutStream.write(buf);
		});

		worker.stderr.on("data", async (buf: Buffer) => {
			console.error(buf.toString());
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

		console.error("launching...");
		await client.Lifecycle.launch({
			entrypoint: load.entrypoint,
			//preferenceValues: load.preferenceValues,
			//preferenceValues: {},
			//argumentValues: load.argumentValues,
			//argumentValues: {},
			mode: load.mode,
			support_path: supportPath,
			asset_path: assetsPath,
			is_raycast: load.is_raycast,
			extension_name: load.extension_name,
			owner_or_author_name: load.owner_or_author_name,
			command_name: load.command_name,
		});

		console.error("loaded...");

		return { session_id: sessionId };
	}

	async unload(session_id: string): Promise<boolean> {
		const workerInfo = this.workerMap.get(session_id);

		if (!workerInfo) {
			throw new Error(`No running command with session ${session_id}`);
		}

		await workerInfo.client.Lifecycle.shutdown();

		// force kill after 10s
		workerInfo.pendingTermination = setTimeout(() => {
			workerInfo.worker.terminate();
		}, WORKER_GRACE_PERIOD_MS);

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
			console.error("no worker in pool!");
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

			//console.error('read message: length', length);

			if (!isComplete) return;

			const packet = this.currentMessage.data.subarray(4, length + 4);

			console.error("process", packet.toString("utf8"));

			this.server.route(packet.toString("utf8"));
			//console.error('routing message');
			this.currentMessage.data = this.currentMessage.data.subarray(length + 4);
		}
	}

	constructor() {
		const rpc = new manager.RpcTransport({
			//e
			send: (data) => {
				console.error("send data back", data);
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
