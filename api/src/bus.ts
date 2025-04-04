import { randomUUID } from "crypto"
import { parentPort, MessagePort } from "worker_threads"

export type Message<T = Record<string, any>> = {
	envelope: {
		id: string,
		type: 'request' | 'response' | 'event',
		action: string,
	}
	error: { message?: string } | null;
	data: T; 
};


class Bus {
  private requestMap = new Map<string, { resolve: (message: Message) => void }>();
  private listeners: { id: string, type: string, handler: (...args: any[]) => void }[] = [];

  private handleMessage(message: Message) {
	const { envelope, data } = message;
	console.log('request map size is ', this.requestMap.size);
	console.log('got message', { envelope });

	if (envelope.type == 'response') {
		const request = this.requestMap.get(envelope.id);

		if (!request) {
			console.error(`Received response for unknown request ${envelope.action} ${envelope.id}`);
			return ;
		}

		this.requestMap.delete(envelope.id);
		request.resolve(message);

		console.log({ resolve: message });

		return ;
	}

	if (envelope.type == 'event') {
		console.log('got event in extension', envelope);
		for (const listener of this.listeners) {
			if (listener.type != envelope.action) continue ;

			listener.handler(...(data.args ?? []))
		}

		return ;
	}

	if (envelope.type == 'request') {
		console.error(`Direct requests to extensions are not yet supported`);
		return ;
	}

	console.log('resolved request', message);
  }

  constructor(private readonly port: MessagePort) {
	  if (!port) return ;
	  console.error('INSTANCIATE BUS');
	  port.on('message', this.handleMessage.bind(this));
	  port.on('messageerror', (error) => {
		  console.error(`Message error from manager`, error);
	  });
	  port.on('close', () => {
		  console.error(`Parent port closed prematurely`);
	  });
  }

  unsubscribe(id: string) {
	  const idx = this.listeners.findIndex(lstn => lstn.id == id);

	  if (idx == -1) return ;

	  this.listeners.splice(idx, 1);
  }

  subscribe(type: string, cb: (...args: any[]) => void) {
	  const item = { id: randomUUID(), type, handler: cb };

	  this.listeners.push(item);

	  return {
		  unsubscribe: () => this.unsubscribe(item.id)
	  }
  }

  emit(action: string, data: Record<string, any>) {
	const id = randomUUID();
	const message: Message = {
		envelope: {
			type: 'event',
			action, 
			id
		},
		data,
		error: null,
	};
	
	this.port.postMessage(message);
  }

  request<T = Record<string, any>>(action: string, data: Record<string, any> = {}, options: { timeout?: number, rejectOnError?: boolean } = {}): Promise<Message<T>> {
	const id = randomUUID();
	const { rejectOnError = true } = options;

	return new Promise<Message<T>>((resolve, reject) => {
		let timeout: NodeJS.Timeout | undefined;

		if (options.timeout) {
			timeout = setTimeout(() => reject(new Error(`request timed out`)), options.timeout);
		}

		const resolver = (message: Message) => { 
			clearTimeout(timeout);

			if (message.error && rejectOnError) {
				return reject(message.error.message ?? "Unknown error");
			}

			resolve(message as Message<T>);
		};

		try {
			this.requestMap.set(id, { resolve: resolver });
			console.log(`add id for message type ${action}` + id);

			const message: Message = {
				envelope: {
					type: 'request',
					action, 
					id
				},
				data,
				error: null
			};

			this.port.postMessage(message);
		} catch (error) {
			reject(error);
		}
	});
  }
};

export const createHandler = (handler: (...args: any[]) => void): string => {
	const id = randomUUID();

	bus.subscribe(id, handler);

	return id;
};

export const bus = new Bus(parentPort!);
