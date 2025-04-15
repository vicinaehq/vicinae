import { ReadStream, WriteStream } from "fs";

type RouterSettings = {
	readStream?: ReadStream;
	writeStream?: WriteStream;
};

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

type HandlerApi = {
	forward: (message: FullMessage) => void;
	emit: (name: string, data: Record<string, any>) => void;
};

type Promisifiable<T> = T | Promise<T>;

type RequestHandler = (payload: Record<string, any>, api: HandlerApi) => Promise<Record<string, any>> | Record<string, any>;

class Router {
	private readonly handlers: { handler: RequestHandler, path: string }[] = [];

	constructor(public readonly base: string) {
	}

	public routeRequest(path: string, payload: Record<string, any>, api: HandlerApi): Promisifiable<Record<string, any>> {
		for (const handler of this.handlers) {
			if (handler.path === path) {
				return handler.handler(payload, api);
			}
		}

		return {};
	}

	onRequest(path: string, handler: RequestHandler) {
		this.handlers.push({ path, handler });
	}
};

class OmniServer {
	private readonly requestMap = new Map<string, Worker>;
	private readonly input = process.stdin;
	private readonly output = process.stdout;
	private readonly routers: Router[] = [];
	private readonly selfMessenger: Messenger = {
		type: 'manager'
	};

	private async writePacket(message: Buffer) {
		const packet = Buffer.allocUnsafe(message.length + 4);
		
		packet.writeUint32BE(message.length, 0);
		message.copy(packet, 4, 0);
		this.output.write(packet);
	}

	private async writeMessage(message: FullMessage) {
		const json = JSON.stringify(message);

		await this.writePacket(Buffer.from(json));
	}

	private respond(envelope: MessageEnvelope, data: Record<string, any>) {
		envelope.target = envelope.sender;
		envelope.sender = this.selfMessenger;
		envelope.type = 'response';

		this.writeMessage({ envelope, data });
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

	private findSuitableRouter(path: string): Router | null {
		let candidate: Router | null = null;
		let maxLen = 0;
		
		for (const router of this.routers) {
			if (path.startsWith(router.base) && router.base.length >= maxLen) {
				candidate = router;
				maxLen = router.base.length;
			}
		}

		return candidate;
	}

	private async routeMessage(message: FullMessage) {
		const { envelope } = message;

		if (envelope.type === 'request') {
			const router = this.findSuitableRouter(envelope.action);

			if (!router) {
				console.error(`No handler for ${envelope.action}`);

				return this.respond(envelope, {
					error: {
						message: `No request handler for requested action ${envelope.action}`
					}
				});
			}

			console.error(`Passed request to router with base ${router.base}`);

			const response = await router.routeRequest(envelope.action, message.data, {
				forward: (message: FullMessage) => this.writeMessage(message),
				emit: () => {}
			});

			this.respond(envelope, response);
		}

	}

	constructor() {
		this.input.on('error', (error) => {
			throw new Error(`${error}`);
		});
		this.input.on('data', (buf) => {
			const packet = Buffer.from(buf);
			const message = this.parseMessage(packet);

			this.routeMessage(message);
		});
	}
};
