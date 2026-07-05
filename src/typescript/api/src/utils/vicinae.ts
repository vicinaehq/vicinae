import * as net from "node:net";
import * as path from "node:path";
import { Client, type PingResponse, RpcTransport } from "../proto/ipc.js";

export type VicinaeClientOptions = {
	socketPath?: string;
	timeoutMs?: number;
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
	constructor(private readonly options: VicinaeClientOptions = {}) {}

	ping(): Promise<PingResponse> {
		return this.withConnection((client) => client.Ipc.ping());
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
		const response = await this.withConnection((client) =>
			client.Ipc.deeplink({ url }),
		);

		if (response.error) throw new Error(response.error);
	}

	private withConnection<T>(run: (client: Client) => Promise<T>): Promise<T> {
		const socketPath = this.options.socketPath ?? serverSocketPath();
		const timeoutMs = this.options.timeoutMs ?? 5000;

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

			const client = new Client(
				new RpcTransport({
					send: (payload: string) => {
						const body = Buffer.from(payload);
						const frame = Buffer.alloc(4 + body.length);

						frame.writeUInt32LE(body.length, 0);
						body.copy(frame, 4);
						socket.write(frame);
					},
				}),
			);

			socket.setTimeout(timeoutMs, () => {
				fail(new Error("Timed out waiting for a response from Vicinae"));
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

			socket.on("close", () => {
				fail(new Error("Connection closed before a response was received"));
			});

			socket.on("connect", () => {
				run(client).then(succeed, (error: unknown) => {
					fail(error instanceof Error ? error : new Error(String(error)));
				});
			});

			socket.on("data", (chunk) => {
				data = Buffer.concat([data, chunk]);

				while (data.length >= 4) {
					const size = data.readUInt32LE(0);
					if (data.length < 4 + size) break;

					try {
						client.route(data.subarray(4, 4 + size).toString());
					} catch (error) {
						fail(new Error(`Received a malformed response: ${error}`));
						return;
					}

					data = data.subarray(4 + size);
				}
			});
		});
	}
}
