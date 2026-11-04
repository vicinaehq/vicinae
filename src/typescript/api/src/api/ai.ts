import { getClient } from "./client";

export namespace AI {
	type AIAskRes = Promise<string> & {
		on(event: "data", listener: (chunk: string) => void): void;
	};

	/**
	 * Create a prompt completion using one of the models set up in Vicinae.
	 *
	 * @param prompt - The prompt to ask the AI
	 * @param options - Optionally select which model to use or cancel the request
	 *
	 * @example
	 * ```typescript
	 * const completion = AI.ask('Give me a fun fact about bananas');
	 *
	 * // automatic streaming support
	 * completion.on('data', (chunk) => {
	 * 	console.log({ chunk });
	 * });
	 *
	 * const fullAnswer = await completion;
	 * ```
	 */
	export function ask(prompt: string, options?: AskOptions): AIAskRes {
		const subscribers = [] as Array<(chunk: string) => void>;
		const signal = options?.signal;
		const api = getClient();

		const promise = new Promise<string>((resolve, reject) => {
			if (signal?.aborted) {
				reject(signal.reason);
				return;
			}

			api.AI.ask(prompt, { model: options?.model }).then((completionId) => {
				if (signal?.aborted) {
					api.AI.abort(completionId);
					reject(signal.reason);
					return;
				}

				let data = "";

				const stop = () => {
					stopData.unsubscribe();
					stopFailure.unsubscribe();
					signal?.removeEventListener("abort", onAbort);
				};
				const onAbort = () => {
					stop();
					api.AI.abort(completionId);
					reject(signal?.reason);
				};
				const stopData = api.AI.dataReceived((id, chunk, done) => {
					if (id !== completionId) return;
					data += chunk;
					for (const cb of subscribers) cb(chunk as string);
					if (done) {
						stop();
						resolve(data);
					}
				});
				const stopFailure = api.AI.completionFailed((id, error) => {
					if (id !== completionId) return;
					stop();
					reject(new Error(error));
				});

				signal?.addEventListener("abort", onAbort, { once: true });
			}, reject);
		});

		Object.assign(promise, {
			on: (event: "data", listener: (chunk: string) => void) => {
				if (event === "data") {
					subscribers.push(listener);
				}
			},
		});

		return promise as AIAskRes;
	}

	/**
	 * List all AI models currently available in vicinae.
	 * Only models that are suitable for completion tasks are returned.
	 */
	export async function getModels(): Promise<AI.Model[]> {
		return getClient().AI.listModels();
	}

	export type AskOptions = {
		/**
		 * The AI model to use to answer to the prompt, as returned by `AI.getModels`.
		 * If it is not specified, the default model from the AI settings is used, and the call throws if there is none.
		 */
		model?: string;
		/**
		 * Abort signal to cancel the request.
		 */
		signal?: AbortSignal;
	};

	//type Capability = "completion" | "tools" | "vision";

	export type Model = {
		id: string;
		name: string;
		//capabilities: Capability[]; // we may add this later.
	};
}
