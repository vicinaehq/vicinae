import { bus } from "./bus";

export namespace AI {
	const creativityToTemperature: Record<Creativity, number> = {
		none: 0,
		low: 0.3,
		medium: 0.7,
		high: 1.0,
		maximum: 2.0,
	};

	/**
	 * High level function to leverage vicinae AI capabilities to create a prompt completion.
	 *
	 * @param prompt - The prompt to ask the AI
	 * @param options - Options to further tweak model behavior or explicitly select what model to use
	 *
	 * @example
	 * ```typescript
	 * const completion = AI.ask('Give me a fun fact about bananas', { creativity: 'high' })
	 *
	 * // automatic streaming support
	 * completion.on('data', (chunk) => {
	 * 	console.log({ chunk });
	 * });
	 *
	 * const fullAnswer = await completion;
	 * ```
	 */
	export function ask(
		prompt: string,
		options?: AskOptions,
	): Promise<string> & {
		on(event: "data", listener: (chunk: string) => void): void;
	} {
		const subscribers = [] as Array<(chunk: string) => void>;
		let temperature = creativityToTemperature[options?.creativity ?? "medium"];

		if (typeof options?.creativity === "number") {
			if (options.creativity < 0 || options.creativity > 2) {
				throw new Error(
					"Creativity should be a floating point number in the 0-2 range.",
				);
			}
			temperature = options.creativity;
		}

		const promise = new Promise<string>((resolve, reject) => {
			let data = "";
			const { id } = bus.addEventHandler((chunk, done) => {
				data += chunk;
				for (const cb of subscribers) cb(chunk as string);
				if (done) resolve(data);
			});

			options?.signal?.addEventListener("abort", () => {
				bus.request("ai.abortAsk", { handler: id });
			});

			bus
				.request("ai.ask", {
					prompt,
					temperature,
					model: options?.model,
					handler: id,
				})
				.then((res) => {
					if (!res.ok) reject(res.error);
				});
		});

		Object.assign(promise, {
			on: (event: "data", listener: (chunk: string) => void) => {
				if (event === "data") {
					subscribers.push(listener);
				}
			},
		});

		return promise as any;
	}

	/**
	 * List all AI models currently available in vicinae.
	 * Only models that are suitable for completion tasks are returned.
	 */
	export async function getModels(): Promise<AI.Model[]> {
		return bus
			.request("ai.getModels", {})
			.then((res) =>
				res.unwrap().models.map(({ capabilities, ...rest }) => rest),
			);
	}

	export type AskOptions = {
		/**
		 * How creative you want the AI model to be. This internally maps to the `temperature` parameter.
		 * Some models may not be affected by this option.
		 * You can set
		 */
		creativity?: Creativity;
		/**
		 * The AI model to use to answer to the prompt. If it is not specified, vicinae will pick the model best suited for the job.
		 */
		model?: string;
		/**
		 * Abort signal to cancel the request.
		 */
		signal?: AbortSignal;
	};
	/**
	 * Concrete tasks, such as fixing grammar, require less creativity while open-ended questions, such as generating ideas, require more.
	 * If a number is passed, it needs to be in the range 0-2. For larger values, 2 will be used. For lower values, 0 will be used.
	 */
	export type Creativity =
		| "none"
		| "low"
		| "medium"
		| "high"
		| "maximum"
		| number;

	//type Capability = "completion" | "tools" | "vision";

	export type Model = {
		id: string;
		name: string;
		//capabilities: Capability[]; // we may add this later.
	};
}
