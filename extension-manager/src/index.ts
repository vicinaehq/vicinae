import { basename, join } from "path"
import { readFileSync } from 'fs';
import { randomUUID } from 'crypto';
import { isMainThread, Worker } from "worker_threads";
import { main as workerMain } from './worker';
import { isatty } from "tty";
import { extensionDataDir, safeKill, testMode } from "./utils";
import { appendFile, readdir, readFile, stat } from "fs/promises";

type ExtensionCommand = {
	name: string;
	title: string;
	subtitle: string;
	description: string;
	arguments: any[];
	preferences: any[];
	mode: string;
	componentPath: string;
};

type Extension = {
	id: string;
	name: string;
	description: string;
	title: string;
	path: string;
	icon: string;
	commands: ExtensionCommand[];
	environment: ExtensionEnvironment;
	preferences: any[];
};

export type ExtensionEnvironment = 'development' | 'production';

type Messenger = {
	id?: string;
	type: 'main' | 'manager' | 'extension';
};

type MessageEnvelope = {
	id: string;
	type: 'request' | 'response' | 'event';
	sender: Messenger;
	target: Messenger;
	action: string;
};

type FullMessage = {	
	envelope: MessageEnvelope;
	data: Record<string, any>;
};

export type LoadedCommand = {
	worker: Worker;
	sessionId: string;
};

class Omnicast {
	private readonly workerMap = new Map<string, Worker>;
	private readonly requestMap = new Map<string, Worker>;
	private readonly selfMessenger: Messenger = {
		type: 'manager'
	};
	private currentMessage: { data: Buffer }= {
		data: Buffer.from(''),
	};

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);
		
		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);
		process.stdout.write(packet);
	}

	private respond(envelope: MessageEnvelope, data: Record<string, any>) {
		envelope.target = envelope.sender;
		envelope.sender = this.selfMessenger;
		envelope.type = 'response';

		this.writeMessage({ envelope, data });
	}

	private async writeMessage(message: FullMessage) {
		const json = JSON.stringify(message);

		await this.writePacket(Buffer.from(json));
	}

	private parseMessage(packet: Buffer): FullMessage {
		const { envelope, data } = JSON.parse(packet.toString());

		return { envelope, data };
	}

	private async parseExtensionBundle(path: string): Promise<Extension> {
		const metadata = JSON.parse(readFileSync(join(path, 'package.json'), 'utf-8'));
		const { name, title, icon, version, description, preferences = [] } = metadata;
		const extension: Extension = { environment: 'production', id: basename(path), description, icon, title, name, preferences, path, commands: [] };

		for (const cmd of metadata.commands) {
			const bundle = join(path, `${cmd.name}.js`);
			
			extension.commands.push({ 
				name: cmd.name, 
				title: cmd.title,
				subtitle: cmd.subtitle,
				description: cmd.description,
				arguments: cmd.arguments ?? [],
				preferences: cmd.preferences ?? [],
				mode: cmd.mode,
				componentPath: bundle 
			});
		}

		return extension;
	}

	private async handleManagerRequest({ envelope, data }: FullMessage) {
		const { action } = envelope;

		if (action == 'ping') {
			return this.respond(envelope, { message: 'pong' });
		}

		if (action == 'echo') {
			return this.respond(envelope, data);
		}

		if (action.startsWith("develop.")) {
			const { id } = data as { id: string };
			const path = join(extensionDataDir(), id);

			if (!await testMode(path)) {
				return this.respond(envelope, {
					error: {
						message: `No extension bundle at ${path}`
					}
				});
			}

			if (!id.endsWith('.dev')) {
				console.debug(`develop.start: extension bundle has no .dev suffix: ${id}`);
			}

			if (action == "develop.start") {
				const extension = this.parseExtensionBundle(path);

				return this.respond(envelope, {
					extension
				});
			}

			if (action == "develop.refresh") {
				// XXX do something smart here, probably
				return this.respond(envelope, {
					message: 'refresh OK'
				});
			}

			if (action == "develop.stop") {
				const cliPidFile = join(path, 'cli.pid');

				if (await testMode(path)) {
					const pid = parseInt(await readFile(cliPidFile, 'utf-8'));

					if (!safeKill(pid, 'SIGINT')) {
						console.debug(`Could not kill CLI pid ${pid}. A CLI process may not be running under this pid.`);
					}
				} else {
					console.debug(`Could not find a cli.pid file`);
				}

				return this.respond(envelope, {
					message: 'OK'
				});
			}
		}

		if (action == "load-command") {
			const extensions = await this.scanInstallDirectory();
			const { extensionId, commandName, preferenceValues = {}, launchProps = {} } = data;
			const extension = extensions.find(ext => ext.id == extensionId);

			if (!extension) {
				return this.respond(envelope, {
					error: {
						message: `No extension with id ${extensionId}`
					}
				});
			}

			const command = extension.commands.find(cmd => cmd.name == commandName);

			if (!command) {
				return this.respond(envelope, {
					error: {
						message: `No command exposed by extension ${extensionId} (extension exists)`
					}
				});
			}

			console.error('loading command', { extensionId, commandName });

			const sessionId = randomUUID();

			const worker = new Worker(__filename, {
				workerData: {
					entrypoint: command.componentPath,
					preferenceValues,
					launchProps,
					commandMode: command.mode,
				},
				stdout: true,
				env: {
					'NODE_ENV': extension.environment,
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

			worker.on('message', ({ envelope, data }) => {
				this.requestMap.set(envelope.id, worker);

				const qualifiedPayload: FullMessage = {
					envelope: {
						sender: {
							type: 'extension',
							id: sessionId,
						},
						target: { type: 'main' },
						action: envelope.action,
						id: envelope.id,
						type: envelope.type
					},
					data
				};

				//console.error(`from worker`, { qualifiedPayload });
				//console.error(`[DEBUG] forward event type ${qualifiedPayload.envelope.action}`);

				this.writePacket(Buffer.from(JSON.stringify(qualifiedPayload)));
			});

			worker.stdout.on('data', async (buf) => {
				await appendFile(join(extension.path, "dev.log"), buf)
			});

			worker.on('error', (error) => { 
				console.error(`error is of type ${typeof error}: instance of error ${error instanceof Error}`);
				console.error(`worker error: ${error.name}:${error.message}`); 
			});

			worker.on('exit', (code) => {
				console.error(`worker exited with code ${code}`)
				this.workerMap.delete(sessionId);
			});

			return this.respond(envelope, {
				sessionId,
				command: {
					name: command.name,
				}
			});
		}

		if (action == "unload-command") {
			const { sessionId } = data;
			const worker = this.workerMap.get(sessionId);

			if (!worker) {
				return this.respond(envelope, {
					error: {
						message: `No running command with session ${sessionId}`
					}
				});
			}

			if (worker) {
				this.workerMap.delete(sessionId);
				await worker.terminate();
			}

			return this.respond(envelope, {
				message: 'command unloaded'
			});
		}

		if (action == "list-extensions") {
			const extensions = await this.scanInstallDirectory();

			return this.respond(envelope, {
				extensions
			});
		}

		return this.respond(envelope, {
			message: `unknown action ${action}`
		});
	}

	private async routeMessage(message: FullMessage) {
		const { envelope } = message;
		const { target } = envelope;

		if (target.type === 'manager') {
			if (envelope.type == 'response') {
				throw new Error('main response to manager not implemented');
			}

			if (envelope.type == 'request') {
				return this.handleManagerRequest(message);
			}
		}

		if (target.type == 'extension' && target.id) {
			const command = this.workerMap.get(target.id);

			if (!command) {
				console.error(`Cannot forward message to non existant extension session id ${target.id}`);
				return ;
			}

			command.postMessage(message);
		}
	}

	async scanInstallDirectory(): Promise<Extension[]> {
		const extensionDir = extensionDataDir();
		const entries = await readdir(extensionDir);
		const extensions: Extension[] = [];

		for (const entry of entries) {
			const path = join(extensionDir, entry);
			const stats = await stat(path);

			if (!stats.isDirectory()) continue ;

			const extension = await this.parseExtensionBundle(path);

			extensions.push(extension);
		}

		return extensions;
	}

	handleRead(data: Buffer) {
		this.currentMessage.data = Buffer.concat([this.currentMessage.data, data]);

		while (this.currentMessage.data.length >= 4) {
			const length = this.currentMessage.data.readUInt32BE();
			const isComplete = this.currentMessage.data.length - 4 >= length;

			if (!isComplete) return ;

			const packet = this.currentMessage.data.subarray(4, length + 4);
			const message = this.parseMessage(packet);

			this.routeMessage(message);
			this.currentMessage.data = this.currentMessage.data.subarray(length + 4);
		}
	}

	constructor() {
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

	await omnicast.scanInstallDirectory();
}

main();
