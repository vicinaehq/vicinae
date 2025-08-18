import { randomUUID } from "crypto"
import { parentPort, MessagePort } from "worker_threads"

import * as ipc from './proto/ipc';
import * as extension from './proto/extension';
import { PushViewRequest } from "./proto/ui";
import { AckResponse } from "./proto/common";
import { Err, Ok, Result } from './lib/result';


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

type Requests = {
	[K in keyof Required<extension.RequestData>]: {
		[R in keyof Required<Required<extension.RequestData>[K]>]: {
			data: NonNullable<Required<extension.RequestData>[K][R]>
		}
	}
}

// Requests and Responses are unrelated types, but we want to define a type safe mapping between them

type Responses = {
	[K in keyof Required<extension.ResponseData>]: {
		[R in keyof Required<Required<extension.ResponseData>[K]>]: {
			data: NonNullable<Required<extension.ResponseData>[K][R]>
		}
	}
}

// requests and responses are technically unrelated as far as the type system is concerned,
// so we need this mapping here. It's good practice to structure the request path and the response path the same
// so this looks mostly repetitive.
// Using this approach, an invalid mapping does not compile.

type EndpointMapping = {
	"app.list": "app.list";
	"app.open": "app.open";

	'ui.render': 'ui.render'
	'ui.showToast': 'ui.showToast',
	'ui.hideToast': 'ui.hideToast',
	'ui.updateToast': 'ui.updateToast',
	'ui.pushView': 'ui.pushView',
	'ui.popView': 'ui.popView',
	'ui.closeMainWindow': 'ui.closeMainWindow',
	'ui.showHud': 'ui.showHud',
	'ui.setSearchText': 'ui.setSearchText'
	'ui.confirmAlert': 'ui.confirmAlert'
	'ui.getSelectedText': 'ui.getSelectedText',

	'storage.get': 'storage.get',
	'storage.set': 'storage.set',
	'storage.remove': 'storage.remove',
	'storage.clear': 'storage.clear',
	'storage.list': 'storage.list',

	'oauth.authorize': 'oauth.authorize',

	'clipboard.copy': 'clipboard.copy'
	'clipboard.paste': 'clipboard.paste'
}

type RequestEndpoint = keyof EndpointMapping
type ResponseEndpoint = EndpointMapping[RequestEndpoint]

// Helper types to extract from dot notation
type ExtractRequestType<T extends RequestEndpoint> = 
	T extends `${infer Category}.${infer Action}`
		? Category extends keyof Requests
			? Action extends keyof Requests[Category]
				? Requests[Category][Action]
				: never
			: never
		: never

type ExtractResponseType<T extends ResponseEndpoint> = 
	T extends `${infer Category}.${infer Action}`
		? Category extends keyof Responses
			? Action extends keyof Responses[Category]
				? Responses[Category][Action]
				: never
			: never
		: never

type Map = {
	[K in RequestEndpoint]: {
		request: ExtractRequestType<K>['data'];
		response: ExtractResponseType<EndpointMapping[K]>['data'];
	}
}

class Bus {
  private requestMap = new Map<string, { resolve: (message: Message) => void }>();
  private safeRequestMap = new Map<string, { resolve: (message: extension.Response) => void }>();
  private eventListeners = new Map<string, EventListenerInfo[]>();

  async turboRequest<T extends RequestEndpoint>(endpoint: T, data: Map[T]['request']): Promise<Result<Map[T]['response'], Error>> {
	  const [category, requestId] = endpoint.split('.') as [any, any];
	  const request = extension.RequestData.create({ [category]: { [requestId]: data } } as any);
	  const res = await this.request2(request);

	  if (!res.ok) {
		  return Err(res.error);
	  }

	  const resData = res.value[category]?.[requestId];

	  if (!resData) return Err(Error(`Invalid response for request of type ${endpoint}: ${JSON.stringify(res, null, 2)}`));

	  //console.error(`Got valid response for ${endpoint}`);

	  return Ok(resData);
  }


  private handleSafeMessage(message: ipc.ExtensionMessage) {
	  if (message.response) {
		  //console.log('got response response', message.response.requestId);
		const request = this.safeRequestMap.get(message.response.requestId);

		if (!request) {
			//console.error(`Received response for unknown request ${message.response.requestId}`);
			return ;
		}

		this.requestMap.delete(message.response.requestId);
		request.resolve(message.response);
		return ;
	  }

	  if (message.event) {
		const { id, generic } = message.event;

		//console.error('got event with id', id);

		if (generic) {
			const listeners = this.listEventListeners(id)
			const args = JSON.parse(generic.json);

			for (const listener of listeners) {
				listener.callback(...(args ?? []))
			}
		}
	  }
  }

  emitCrash(errorText: string) {
	  this.sendMessage({
		  event: {
			  id: randomUUID(),
			  crash: { text: errorText }
		  }
	  });
  }

  constructor(private readonly port: MessagePort) {
	  if (!port) return ;

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
	
	this.sendMessage(message);
  }

  sendMessage(message: ipc.ExtensionMessage) {
	  this.port.postMessage(ipc.ExtensionMessage.encode(message).finish());
  }

  request2(data: extension.RequestData, options: { timeout?: number } = {}): Promise<Result<extension.ResponseData, Error>> {
	  const req = extension.Request.create({ requestId: randomUUID(), data });

	  return new Promise<Result<extension.ResponseData, Error>>((resolve, reject) => {
		let timeout: NodeJS.Timeout | undefined;

		if (options.timeout) {
			timeout = setTimeout(() => resolve(Err(Error(`request timed out`))), options.timeout);
		}

		const resolver = (response: extension.Response) => { 
			clearTimeout(timeout);

			if (response.error) {
				return resolve(Err(new Error(response.error.errorText)));
			}

			if (!response.data) {
				return resolve(Err(new Error("No error and no data")));
			}

			resolve(Ok(response.data));
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
