import { createConnection, Socket } from 'net';
import { Action, Component, isHandlerType, StatefulComponent } from './ui';
import { randomUUID } from 'crypto';

export const MessageTypes = [
	'render',
	'echo',
	'register',
	'event',
] as const;

export type Message<T extends Record<string, any> = Record<string, any>> = {
	type: MessageType,
	data: T
};

export type MessageType = typeof MessageTypes[number];

export type RegisteredHandler = {
	dispatcher: Function;
};

export type UpdateOptions = {
	dispatchAsync: (fn: () => Promise<Action>) => void;
};

export type SerializedComponent = {
	type: string,
	props: Record<string, any>;
	children: SerializedComponent[],
};

export class OmnicastClient {
	private socket: Socket;
	private handlerMap = new Map<string, RegisteredHandler>;
	private currentTree: SerializedComponent | null = null;

	private sendRaw(data: Record<string, any>)  {
		const msg = Buffer.from(JSON.stringify(data), 'utf-8')
		const buffer = Buffer.allocUnsafe(msg.length + 4);

		buffer.writeUint32LE(msg.length);
		msg.copy(buffer, 4, 0, msg.length);

		this.socket.write(buffer);
	}
	
	private parseMessage(data: Buffer): Message {
		const mlen = data.readUInt32LE();
		const payload = data.subarray(4, mlen + 4);

		if (mlen != payload.length) console.warn(`mlen (${mlen}) != payload.length (${payload.length})`);

		const json = JSON.parse(payload.toString('utf-8'));

		return {
			type: json.type,
			data: json.data
		};
	}

	private sendMessage(type: MessageType, payload: Record<string, any>) {
		this.sendRaw({
			type,
			data: payload
		});
	}

	async dispatchAsync(fn: () => Promise<Action>) {
		const action = await fn();
		this.app.update(action, { dispatchAsync: this.dispatchAsync.bind(this) });
		this.app.render();
	}

	public renderNode(ctx: StatefulComponent, current: Record<string, any> | null, node: Component): SerializedComponent {
		const data: SerializedComponent = {
			type: node.type,
			props: node.props,
			children: []
		};

		const dispatchAsync = (fn: () => Promise<Action>) => {
		}

		for (const [k, v] of Object.entries(node.props)) {
			if (isHandlerType(k)) {
				let id = '';
				if (current && current.type == node.type && current.props[k]) id = current.props[k];
				else id = randomUUID();

				const HandlerAction: new(...params: any[]) => Action = data.props[k];

				data.props[k] = id;
				this.handlerMap.set(id, {
					dispatcher: (payload: any[]) => ctx.update(new HandlerAction(payload), { dispatchAsync: this.dispatchAsync.bind(this) })
				});
			} else {
				data.props[k] = v;
			}
		}

		for (let i = 0; i != node.children.length; i += 1) {
			const child = node.children[i];
			let childCtx = child instanceof Component ? ctx : child;
			let childNode = child instanceof Component ? child : child.render();

			data.children[i] = this.renderNode(childCtx, current?.children[i] ?? null, childNode);
		}

		return data;
	}

	public render() {
		const root = this.app.render();
		const tree = this.renderNode(this.app, this.currentTree, root);

		//console.log(JSON.stringify(tree, null, 2));

		this.currentTree = tree;
		this.sendMessage('render', { root: tree });
	}
 
	constructor(private readonly app: StatefulComponent) {
		this.socket = createConnection({ path: process.env.ENDPOINT! });
		this.sendMessage('register', { token: process.env.TOKEN! });

		this.socket.on('data', (data) => {
			try {
				const message = this.parseMessage(data);

				if (message.type == 'event') {
					const handler = this.handlerMap.get(message.data.handlerId);

					if (!handler) return ;

					handler.dispatcher(message.data);
					this.render();
				}
			}  catch (error) {
				console.error(error);	
			}
		});

		this.socket.on('error', (error) => { throw error });
	}
};
