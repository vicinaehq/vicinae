import { randomUUID } from "crypto"
import { parentPort, MessagePort } from "worker_threads"

export type Message<T = Record<string, any>> = {
	envelope: {
		id: string,
		type: 'request' | 'response' | 'event',
		action: string,
	}
	data: T; 
};


class Bus {
  private requestMap = new Map<string, { resolve: (message: Message) => void }>();
  private listeners: { id: string, type: string, handler: (...args: any[]) => void }[] = [];

  private handleMessage(message: Message) {
	const { envelope, data } = message;
	console.log('request map size is ', this.requestMap.size);
	console.log({ envelope });

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
	  console.error('INSTANCIATE BUS');
	  port.on('message', this.handleMessage.bind(this));
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
		data
	};
	
	this.port.postMessage(message);
  }

  request(action: string, data: Record<string, any>): Promise<Message> {
	const id = randomUUID();

	return new Promise<Message>((resolve) => {
		this.requestMap.set(id, { resolve });
		console.log(`add id for message type ${action}` + id);

		const message: Message = {
			envelope: {
				type: 'request',
				action, 
				id
			},
			data
		};

		this.port.postMessage(message);
	});
  }
};

export const bus = parentPort ? new Bus(parentPort) : null;
