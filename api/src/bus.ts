import { randomUUID } from "crypto"
import { parentPort, MessagePort } from "worker_threads"

import * as ipc from './proto/ipc';
import * as extension from './proto/extension';


export type Message<T = Record<string, any>> = {
	envelope: {
		id: string,
		type: 'request' | 'response' | 'event',
		action: string,
	}
	error: { message?: string } | null;
	data: T; 
};

namespace EventListener {
	export type ArgValue = string | number | Record<any, any> | boolean | null | undefined;
	export type Callback = (...args: EventListener.ArgValue[]) => void;
};

type EventListenerInfo = {
	callback: EventListener.Callback;
};


type RequestType = keyof extension.RequestData;
type RequestFor<T extends RequestType> = NonNullable<extension.RequestData[T]>;


class Bus {
  private requestMap = new Map<string, { resolve: (message: Message) => void }>();
  private safeRequestMap = new Map<string, { resolve: (message: extension.Response) => void }>();
  private eventListeners = new Map<string, EventListenerInfo[]>();

  private handleSafeMessage(message: ipc.ExtensionMessage) {
	  if (message.response) {
		const request = this.safeRequestMap.get(message.response.requestId);

		if (!request) {
			console.error(`Received response for unknown request ${message.response.requestId}`);
			return ;
		}

		this.requestMap.delete(message.response.requestId);
		request.resolve(message.response);
		return ;
	  }

	  if (message.event) {
		const { id, generic } = message.event;

		console.error('got event with id', id);

		if (generic) {
			const listeners = this.listEventListeners(id)
			const args = JSON.parse(generic.json);

			console.error(`Got ${listeners.length} listeners for event ${id}`);

			for (const listener of listeners) {
				listener.callback(...(args ?? []))
			}
		}
	  }
  }

  constructor(private readonly port: MessagePort) {
	  if (!port) return ;
	  console.error('INSTANCIATE BUS');

	  port.on('message', (buf) => {
		  this.handleSafeMessage(ipc.ExtensionMessage.decode(buf));
	  });

	  port.on('messageerror', (error) => {
		  console.error(`Message error from manager`, error);
	  });
	  port.on('close', () => {
		  console.error(`Parent port closed prematurely`);
	  });
  }
  
  listEventListeners(type: string): EventListenerInfo[] {
	  return this.eventListeners.get(type) ?? [];
  }

  subscribe(type: string, cb: EventListenerInfo['callback']) {
	  const item: EventListenerInfo = { callback: cb };
	  let listeners = this.eventListeners.get(type)

	  if (!listeners) { 
		  this.eventListeners.set(type, [item]);
	  } else {
		  listeners.push(item);
	  }

	  return {
		  unsubscribe: () => {
			  const listeners = this.eventListeners.get(type) ?? [];
			  const index = listeners.indexOf(item);

			  if (index != -1) {
			  	listeners.splice(index, 1);
				if (listeners.length === 0) {
					this.eventListeners.delete(type);
				}
			  }
		  }
	  }
  }

  emit(action: string, data: Record<string, any>) {
	const message = ipc.ExtensionMessage.create({
		event: { 
			id: action, 
			generic: { json: JSON.stringify([data]) } 
		}
	});
	
	console.error('event', message);

	this.sendMessage(message);
  }

  sendMessage(message: ipc.ExtensionMessage) {
	  this.port.postMessage(ipc.ExtensionMessage.encode(message).finish());
  }

  request2(data: extension.RequestData, options: { timeout?: number, rejectOnError?: boolean } = {}) {
	  const req = extension.Request.create({ requestId: randomUUID(), data });

	  return new Promise<extension.ResponseData>((resolve, reject) => {
		let timeout: NodeJS.Timeout | undefined;

		if (options.timeout) {
			timeout = setTimeout(() => reject(new Error(`request timed out`)), options.timeout);
		}

		const resolver = (response: extension.Response) => { 
			clearTimeout(timeout);

			if (response.error && options.rejectOnError) {
				return reject(response.error.errorText);
			}

			if (!response.data) {
				return reject("No error and not data. This should normally not happen.");
			}

			resolve(response.data);
		};

		try {
			this.safeRequestMap.set(req.requestId, { resolve: resolver });
			this.sendMessage({ request: req });
		} catch (error) {
			reject(error);
		}
	});

  }

  request<T = Record<string, any>>(action: string, data: Record<string, any> = {}, options: { timeout?: number, rejectOnError?: boolean } = {}): Promise<Message<T>> {
	const id = randomUUID();
	const { rejectOnError = true } = options;

	console.error('request', action);

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
