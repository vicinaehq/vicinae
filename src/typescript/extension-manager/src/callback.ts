import { randomBytes } from "node:crypto";

export type CallbackHandler = (...args: any[]) => void;

class CallbackManager {
	activateHandler(id: string, args: any[]) {
		if (this.pendingCleanup.has(id) || this.previousCleanup.has(id)) {
			return;
		}

		const handler = this.handlers.get(id);

		if (!handler) {
			return;
		}

		handler(...args);
	}

	removeHandler(id: string): void {
		this.handlers.delete(id);
	}

	deferRemoval(id: string): void {
		this.pendingCleanup.add(id);
	}

	flushDeferredRemovals(): void {
		for (const id of this.previousCleanup) {
			this.handlers.delete(id);
		}
		this.previousCleanup = this.pendingCleanup;
		this.pendingCleanup = new Set();
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
	private pendingCleanup = new Set<string>();
	private previousCleanup = new Set<string>();
}

export const callbackManager = new CallbackManager();
