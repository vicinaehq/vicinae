import { createConnection, Socket } from 'net';
import { Action, NativeElement, isHandlerType, Node, Component, HandlerType } from './native-ui';
import { randomUUID } from 'crypto';
import { inspect } from 'util';

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

export type Nullable<T> = T | null;

export type ComponentTree = {
	component: Node,
	children: ComponentTree[];
};

export class OmnicastClient {
	private socket: Socket;
	private handlerMap = new Map<string, RegisteredHandler>;
	private handlerStore = new Map<Symbol, Map<HandlerType, string>>;
	private root: Nullable<ComponentTree> = null;

	private findHandler(node: Node, type: HandlerType): Nullable<string> {
		return this.handlerStore.get(node.id)?.get(type) ?? null;
	}

	private createHandler(node: Node, type: HandlerType): string {
		const handlers = this.handlerStore.get(node.id) ?? new Map<HandlerType, string>();
		const id = randomUUID();

		handlers.set(type, id);
		this.handlerStore.set(node.id, handlers);

		return id;
	}

	private getOrCreateHandler(node: Node, type: HandlerType) {
		const id = this.findHandler(node, type);

		if (id) return id;
		 
		return this.createHandler(node, type);
	}

	private sendRaw(data: Record<string, any>) {
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

	serializeNativeElement(node: NativeElement): ComponentTree {
		const tree: ComponentTree = { component: node, children: [] };

		for (const children of node.children) {
			tree.children.push(this.serializeNode(children));
		}

		return tree;
	}

    serializeNode(node: Node): ComponentTree {
		if (node instanceof Component) {
			return { component: node, children: [this.serializeNativeElement(node.render())] };
		}
		return this.serializeNativeElement(node); 
	}

	public printAsHtml(tree: ComponentTree) {
		let s = '';

		const recurse = (tree: ComponentTree, depth = 0) => {
			const indent = `\t`.repeat(depth);

			const props = Object.entries(tree.component.props).filter(([k]) => !isHandlerType(k)).map(([k, v]) => `${k}=${v}`).join(' ');

			if (tree.children.length == 0) {
				s += indent + `<${tree.component.type} ${props} />\n`
				return ;
			}

			s += indent + `<${tree.component.type} ${props}>\n`
			for (const child of tree.children) {
				recurse(child, depth + 1);
			}
			s += indent + `</${tree.component.type}>\n`;
		}

		recurse(tree);

		console.log(s);
	}

	public printTree(node: Node) {
		const serialized = this.serializeNode(node);

		console.log(inspect(serialized, { colors: true, depth: null }));
	}

	public updateComponentTree(root: Nullable<ComponentTree>, next: ComponentTree): ComponentTree {
		if (!root) {
			return this.updateComponentTree(next, next);
		}

		if (root.component.type != next.component.type) {
			if (next.component instanceof Component) next.component.onMount();
			root = next;
		}


		root.component.props = { ...next.component.props };

		const min = Math.min(root.children.length, next.children.length);

		for (let i = 0; i != min; ++i) {
			root.children[i] = this.updateComponentTree(root.children[i], next.children[i]);
		}

		for (let i = min; i < next.children.length; ++i) {
			root.children.push(this.updateComponentTree(next.children[i], next.children[i]));
		}

		while (root.children.length > next.children.length) root.children.pop();

		return root;
	}


	serializeTree(tree: ComponentTree): SerializedComponent {
		const serialize = (ctx: Component, tree: ComponentTree): SerializedComponent => {
			if (tree.component instanceof Component) {
				// custom component always have only one child
				return serialize(tree.component, tree.children[0]);
			}
			
			const { type, props, } = tree.component;
			const data: SerializedComponent = {
				type,
				props,
				children: []
			};

			for (const [k, v] of Object.entries(data.props)) {
				if (isHandlerType(k)) {
					const HandlerAction: new (...params: any[]) => Action = data.props[k];
					const id = this.getOrCreateHandler(tree.component, k);

					data.props[k] = id;
					this.handlerMap.set(id, {
						dispatcher: (payload: any[]) => ctx.update(new HandlerAction(payload), { dispatchAsync: () => {} })
					});
				} else {
					data.props[k] = v;
				}
			}

			for (const child of tree.children) {
				data.children.push(serialize(ctx, child))
			}

			return data;
		}

		return serialize(this.app, tree);
	}

	public render() {
		if (this.root == null) this.app.onMount();

		const root = this.app.render();
		const tree = this.updateComponentTree(this.root, this.serializeNode(root));
		this.printAsHtml(tree);
		const data = this.serializeTree(tree);

		console.log(JSON.stringify(data, null, 2));

		this.root = tree;
		this.sendMessage('render', { root: data });
	}

	constructor(private readonly app: Component) {
		this.socket = createConnection({ path: process.env.ENDPOINT! });
		this.sendMessage('register', { token: process.env.TOKEN! });

		this.socket.on('data', (data) => {
			try {
				const message = this.parseMessage(data);

				if (message.type == 'event') {
					const handler = this.handlerMap.get(message.data.handlerId);

					if (!handler) {
						console.log('no handler ' + message.data.handlerId);
						return;
					}

					handler.dispatcher(message.data);
					this.render();
				}
			} catch (error) {
				console.error(error);
			}
		});

	   this.socket.on('error', (error) => { throw error });
	}
}

