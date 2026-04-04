import { randomBytes } from "node:crypto";

export type CallbackHandler = (...args: any[]) => void;

class CallbackManager {
	activateHandler(id: string, args: any[]) {
		const handler = this.handlers.get(id);

		if (!handler) {
			throw new Error(
				`activateHandler called on non existent handler with id ${id}`,
			);
		}

		handler(...args);
	}

	/**
	 * Removes callback associated with this ID if there is one.
	 */
	removeHandler(id: string): void {
		this.handlers.delete(id);
	}

	setHandler(id: string, handler: CallbackHandler) {
		this.handlers.set(id, handler);
	}

	subscribe(cb: CallbackHandler) {
		const id = this.generateHandlerId();
		this.handlers.set(id, cb);

		return {
			id,
			unsubscribe: () => {
				this.removeHandler(id);
			},
		};
	}

	private generateHandlerId() {
		return `handler-${randomBytes(16).toString("hex")}`;
	}

	private readonly handlers = new Map<string, CallbackHandler>();
}

export const callbackManager = new CallbackManager();
