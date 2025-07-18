import { randomUUID } from "crypto"
import { parentPort, MessagePort } from "worker_threads"
import { ExtensionRequest, ExtensionMessage, ListApplicationRequest, ListApplicationResponse, ExtensionRequestData, ExtensionResponseData, ExtensionResponse } from './omnicast/protocols/extension/extension';

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

interface RequestResponseMap extends Record<keyof ExtensionRequestData, keyof ExtensionResponseData> {
	'listApps': 'listApplications'
};

type RequestType = keyof ExtensionRequestData;
type RequestFor<T extends RequestType> = NonNullable<ExtensionRequestData[T]>;
type ResponseFor<T extends RequestType> = NonNullable<ExtensionResponseData[RequestResponseMap[T]]>;


class Bus {
  private requestMap = new Map<string, { resolve: (message: Message) => void }>();
  private safeRequestMap = new Map<string, { resolve: (message: ExtensionResponse) => void }>();
  private eventListeners = new Map<string, EventListenerInfo[]>();

  private handleSafeMessage(message: ExtensionMessage) {
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
		const { id, args } = message.event;
		const listeners = this.listEventListeners(id)

		for (const listener of listeners) {
			listener.callback(...(args ?? []))
		}
	  }
  }

  private handleMessage(message: Message) {
	const { envelope, data } = message;

	if (envelope.type == 'response') {
		const request = this.requestMap.get(envelope.id);

		if (!request) {
			console.error(`Received response for unknown request ${envelope.action} ${envelope.id}`);
			return ;
		}

		this.requestMap.delete(envelope.id);
		request.resolve(message);

		return ;
	}

	if (envelope.type == 'event') {
		let start = performance.now();
		const listeners = this.listEventListeners(envelope.action)
		let end = performance.now();

		start = performance.now();

		for (const listener of listeners) {
			listener.callback(...(data.args ?? []))
		}

		end = performance.now();

		return ;
	}

	if (envelope.type == 'request') {
		console.error(`Direct requests to extensions are not yet supported`);
		return ;
	}
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

  safeRequest<T extends keyof ExtensionRequestData>(type: T, data: RequestFor<T>, options: { timeout?: number, rejectOnError?: boolean } = {}): Promise<ResponseFor<T>> {
	  const dat: ExtensionRequestData = {
		  [type]: data
	  };

	  const req: ExtensionRequest = {
		  requestId: randomUUID(),
		  data: dat
	  };


	return new Promise<ResponseFor<T>>((resolve, reject) => {
		let timeout: NodeJS.Timeout | undefined;

		if (options.timeout) {
			timeout = setTimeout(() => reject(new Error(`request timed out`)), options.timeout);
		}

		const resolver = (response: ExtensionResponse) => { 
			clearTimeout(timeout);

			if (response.error && options.rejectOnError) {
				return reject(response.error.errorText);
			}

			if (!response.data) {
				return reject("No error and not data. This should normally not happen.");
			}

			const resData = response.data[type as any] as ResponseFor<T>;

			if (!resData) {
				return reject("Malformed response");
			}

			resolve(resData);
		};

		try {
			this.safeRequestMap.set(req.requestId, { resolve: resolver });
			this.port.postMessage(ExtensionRequest.encode(req).finish());
		} catch (error) {
			reject(error);
		}
	});
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
