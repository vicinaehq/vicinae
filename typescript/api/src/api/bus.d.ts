/// <reference types="node" />
import { MessagePort } from "worker_threads";
export type Message<T = Record<string, any>> = {
	envelope: {
		id: string;
		type: "request" | "response" | "event";
		action: string;
	};
	data: T;
};
declare class Bus {
	private readonly port;
	private requestMap;
	private listeners;
	private handleMessage;
	constructor(port: MessagePort);
	unsubscribe(id: string): void;
	subscribe(
		type: string,
		cb: (...args: any[]) => void,
	): {
		unsubscribe: () => void;
	};
	request(action: string, data: Record<string, any>): Promise<Message>;
}
export declare const bus: Bus | null;
export {};
//# sourceMappingURL=bus.d.ts.map
