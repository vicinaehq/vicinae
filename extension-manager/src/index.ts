import { join } from "path"
import { readFileSync, readdirSync } from 'fs';
import { inspect } from 'util';
import { Socket, createConnection } from 'net';
import { randomUUID } from 'crypto';
import { Worker, parentPort } from "worker_threads";
import { renderView } from "./view";

const EXTENSION_DIR = "/home/aurelle/.local/share/omnicast/extensions/installed";

type ExtensionCommand = {
	name: string;
	title: string;
	subtitle: string;
	description: string;
	mode: string;
	componentPath: string;
};

type Extension = {
	sessionId: string;
	name: string;
	commands: ExtensionCommand[];
};

const extensions: Extension[] = [];

type Message = {
	id: string;
	type: string;
	data: Record<string, any>;
}

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
	private readonly client: Socket
	private readonly requestMap = new Map<string, Worker>;
	private readonly selfMessenger: Messenger = {
		type: 'manager'
	};

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);
		
		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);

		const write = this.client.write(packet);

		console.log({ write });
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
		const n = packet.readUint32BE();

		if (n < packet.length - 4) {
			console.error(`size mismatch: ${n} < ${packet.length - 4}`);
		}
			
		const dat = packet.subarray(4, n + 4);
		const { envelope, data } = JSON.parse(dat.toString());

		return { envelope, data };
	}

	private async handleManagerRequest({ envelope, data }: FullMessage) {
		const { action } = envelope;

		if (action == 'ping') {
			return this.respond(envelope, { message: 'pong' });
		}

		if (action == 'echo') {
			return this.respond(envelope, data);
		}

		if (action == "load-command") {
			const { extensionId, commandName } = data;
			const extension = extensions.find(ext => ext.sessionId == extensionId);

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

			console.log('loading command', { extensionId, commandName });

			const sessionId = randomUUID();
			const worker = new Worker(__filename, {
				workerData: {
					component: command.componentPath
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
				console.log(`worker is online`);
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

				console.log(`from worker`, qualifiedPayload);

				this.writePacket(Buffer.from(JSON.stringify(qualifiedPayload)));
			});

			worker.on('error', (error) => { 
				console.error(`error is of type ${typeof error}: instance of error ${error instanceof Error}`);
				console.error(`worker error: ${error.name}:${error.message}`); 
			});

			worker.on('exit', (code) => {
				console.error(`worker exited with code ${code}`)
				//this.workerMap.delete(sessionId);
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
			return this.respond(envelope, {
				extensions: extensions.map((extension) => ({
					...extension,
					commands: extension.commands.map(cmd => cmd)
				}))
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


	constructor(serverUrl: string) {
		this.client = createConnection({ path: serverUrl });
		this.client.on('error', (error) => {
			throw new Error(`${error}`);
		});
		this.client.on('data', (buf) => {
			const packet = Buffer.from(buf);
			const message = this.parseMessage(packet);

			this.routeMessage(message);
		});
	}
};

const executeView = async () => {
 await renderView();
}

const main = () => {
	// we are in a worker
	if (parentPort) {
		return executeView();
	}

	const url = process.env.OMNICAST_SERVER_URL;

	if (!url) {
	 	throw new Error(`OMNICAST_SERVER_URL is not set`);
	}

	const omnicast = new Omnicast(url)
	
	for (const path of readdirSync(EXTENSION_DIR)) {
		const extPath = join(EXTENSION_DIR, path);
		const metadata = JSON.parse(readFileSync(join(extPath, 'package.json'), 'utf-8'));

		console.log(`loading ${metadata.name} v${metadata.version}...`);

		const extension: Extension = { sessionId: randomUUID(), name: metadata.name, commands: [] };

		for (const cmd of metadata.commands) {
			console.log(`${cmd.name}`);
			const bundle = join(__dirname, 'installed', metadata.name, `${cmd.name}.js`);
			
			extension.commands.push({ 
				name: cmd.name, 
				title: cmd.title,
				subtitle: cmd.subtitle,
				description: cmd.description,
				mode: cmd.mode,
				componentPath: bundle 
			});
		}

		extensions.push(extension);
	}

	for (const extension of extensions) {
		if (extension.commands.length == 0) return ;
		const cmd = extension.commands.at(0);
	}

	console.log(inspect(extensions, { depth: null }));
}

main();
