import * as net from "node:net";
import * as path from "node:path";

export type VicinaeClientOptions = {
	socketPath?: string;
	timeoutMs?: number;
};

export type PingResponse = {
	version: string;
	pid: number;
};

type DeeplinkResponse = {
	error?: string;
};

type JsonRpcResponse = {
	id?: number;
	result?: unknown;
	error?: string;
};

const runtimeDir = (): string => {
	if (process.platform === "darwin") {
		return path.join(process.env.TMPDIR ?? "/tmp", "vicinae");
	}
	return path.join(process.env.XDG_RUNTIME_DIR ?? "/tmp", "vicinae");
};

export const serverSocketPath = (): string =>
	path.join(runtimeDir(), "vicinae.sock");

export class VicinaeClient {
	private requestId = 0;

	constructor(private readonly options: VicinaeClientOptions = {}) {}

	ping(): Promise<PingResponse> {
		return this.request<PingResponse>("Ipc/ping", {});
	}

	refreshDevSession(extensionId: string): Promise<void> {
		return this.deeplink(
			`vicinae://api/extensions/develop/refresh?id=${extensionId}`,
		);
	}

	startDevSession(extensionId: string): Promise<void> {
		return this.deeplink(
			`vicinae://api/extensions/develop/start?id=${extensionId}`,
		);
	}

	stopDevSession(extensionId: string): Promise<void> {
		return this.deeplink(
			`vicinae://api/extensions/develop/stop?id=${extensionId}`,
		);
	}

	private async deeplink(url: string): Promise<void> {
		const response = await this.request<DeeplinkResponse>("Ipc/deeplink", {
			req: { url },
		});

		if (response.error) throw new Error(response.error);
	}

	private request<T>(method: string, params: unknown): Promise<T> {
		const socketPath = this.options.socketPath ?? serverSocketPath();
		const timeoutMs = this.options.timeoutMs ?? 5000;
		const id = ++this.requestId;
		const payload = Buffer.from(
			JSON.stringify({ jsonrpc: "2.0", id, method, params }),
		);
		const frame = Buffer.alloc(4 + payload.length);

		frame.writeUInt32LE(payload.length, 0);
		payload.copy(frame, 4);

		return new Promise<T>((resolve, reject) => {
			const socket = net.createConnection({ path: socketPath });
			let data = Buffer.alloc(0);
			let settled = false;

			const fail = (error: Error) => {
				if (settled) return;
				settled = true;
				socket.destroy();
				reject(error);
			};

			const succeed = (value: T) => {
				if (settled) return;
				settled = true;
				socket.end();
				resolve(value);
			};

			socket.setTimeout(timeoutMs, () => {
				fail(new Error(`Timed out waiting for a response to ${method}`));
			});

			socket.on("error", (error: NodeJS.ErrnoException) => {
				if (error.code === "ENOENT" || error.code === "ECONNREFUSED") {
					fail(
						new Error(
							`Could not connect to Vicinae at ${socketPath}. Is Vicinae running?`,
						),
					);
					return;
				}
				fail(error);
			});

			socket.on("connect", () => socket.write(frame));

			socket.on("data", (chunk) => {
				data = Buffer.concat([data, chunk]);
				if (data.length < 4) return;

				const size = data.readUInt32LE(0);
				if (data.length < 4 + size) return;

				try {
					const message = JSON.parse(
						data.subarray(4, 4 + size).toString(),
					) as JsonRpcResponse;

					if (message.error) {
						fail(new Error(message.error));
					} else {
						succeed(message.result as T);
					}
				} catch (error) {
					fail(new Error(`Received a malformed response: ${error}`));
				}
			});

			socket.on("close", () => {
				fail(new Error("Connection closed before a response was received"));
			});
		});
	}
}
