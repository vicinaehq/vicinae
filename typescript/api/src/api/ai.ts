import { EventEmitter } from "stream";
import { randomUUID } from "crypto";
import { ImageLike } from "./image";
import { bus } from "./bus";

export namespace AI {
	/**
	 * Returns a prompt completion.
	 *
	 * @param prompt - The prompt to ask the AI.
	 * @param options - Options to control which and how the AI model should behave.
	 *
	 * @example
	 * ```typescript
	 * import { Detail, AI, LaunchProps } from "@raycast/api";
	 * import { usePromise } from "@raycast/utils";
	 * import { useState } from "react";
	 *
	 * export default function Command(props: LaunchProps<{ arguments: { prompt: string } }>) {
	 *   const [data, setData] = useState("");
	 *   const { isLoading } = usePromise(
	 *     async (prompt) => {
	 *       const stream = AI.ask(prompt);
	 *       stream.on("data", (data) => {
	 *         setData((x) => x + data);
	 *       });
	 *       await stream;
	 *     },
	 *     [props.arguments.prompt]
	 *   );
	 *
	 *   return <Detail isLoading={isLoading} markdown={data} />;
	 * }
	 * ```
	 */
	export function ask(
		prompt: string,
		options?: AskOptions,
	): Promise<string> & {
		on(event: "data", listener: (chunk: string) => void): void;
	} {
		throw new Error("not implemented");
	}

	export type AskOptions = {
		/**
		 * Concrete tasks, such as fixing grammar, require less creativity while open-ended questions, such as generating ideas, require more.
		 * If a number is passed, it needs to be in the range 0-2. For larger values, 2 will be used. For lower values, 0 will be used.
		 */
		creativity?: Creativity;
		/**
		 * The AI model to use to answer to the prompt.
		 */
		model?: Model | string | __DeprecatedModelUnion;
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
	/**
	 * The AI model to use to answer to the prompt.
	 * @defaultValue `AI.Model["OpenAI_GPT4o-mini"]`
	 */
	export enum Model {
		OpenAI_GPT4 = "openai-gpt-4",
		"OpenAI_GPT4-turbo" = "openai-gpt-4-turbo",
		OpenAI_GPT4o = "openai-gpt-4o",
		"OpenAI_GPT4o-mini" = "openai-gpt-4o-mini",
		Anthropic_Claude_Haiku = "anthropic-claude-haiku",
		Anthropic_Claude_Opus = "anthropic-claude-opus",
		Anthropic_Claude_Sonnet = "anthropic-claude-sonnet",
		MixtraL_8x7B = "mixtral-8x7b",
		Mistral_Nemo = "mistral-nemo",
		Mistral_Large2 = "mistral-large-2",
		Llama3_70B = "llama3-70b",
		"Llama3.1_70B" = "llama3.1-70b",
		"Llama3.1_8B" = "llama3.1-8b",
		"Llama3.1_405B" = "llama3.1-405b",
		"Perplexity_Llama3.1_Sonar_Huge" = "perplexity-llama-3.1-sonar-huge-128k-online",
		"Perplexity_Llama3.1_Sonar_Large" = "perplexity-llama-3.1-sonar-large-128k-online",
		"Perplexity_Llama3.1_Sonar_Small" = "perplexity-llama-3.1-sonar-small-128k-online",
		/** @deprecated Use `AI.Model["OpenAI_GPT4o-mini"]` instead */
		"OpenAI_GPT3.5-turbo-instruct" = "openai-gpt-3.5-turbo-instruct",
		/** @deprecated Use `AI.Model.Llama3_70B` instead */
		Llama2_70B = "llama2-70b",
		/** @deprecated Use `AI.Model.Perplexity_Llama3_Sonar_Large` instead */
		Perplexity_Sonar_Medium_Online = "perplexity-sonar-medium-online",
		/** @deprecated Use `AI.Model.Perplexity_Llama3_Sonar_Small` instead */
		Perplexity_Sonar_Small_Online = "perplexity-sonar-small-online",
		/** @deprecated Use `AI.Model.Llama3_70B` instead */
		Codellama_70B_instruct = "codellama-70b-instruct",
		/** @deprecated Use `AI.Model["Perplexity_Llama3.1_Sonar_Large"]` instead */
		Perplexity_Llama3_Sonar_Large = "perplexity-llama-3-sonar-large-online",
		/** @deprecated Use `AI.Model["Perplexity_Llama3.1_Sonar_Small"]` instead */
		Perplexity_Llama3_Sonar_Small = "perplexity-llama-3-sonar-small-online",
		/** @deprecated Use `AI.Model["OpenAI_GPT4o-mini"]` instead */
		"OpenAI_GPT3.5-turbo" = "openai-gpt-3.5-turbo",
	}
	/** @deprecated Use the `AI.Model` enum instead */
	export type __DeprecatedModelUnion =
		| "openai-gpt-3.5-turbo-instruct"
		| "openai-gpt-3.5-turbo"
		| "openai-gpt-4"
		| "openai-gpt-4-turbo"
		| "anthropic-claude-haiku"
		| "anthropic-claude-opus"
		| "anthropic-claude-sonnet"
		| "perplexity-sonar-medium-online"
		| "perplexity-sonar-small-online"
		| "llama2-70b"
		| "mixtral-8x7b"
		| "codellama-70b-instruct"
		/** @deprecated */
		| "gpt-3.5-turbo"
		/** @deprecated */
		| "gpt-3.5-turbo-instruct"
		/** @deprecated */
		| "gpt-4"
		/** @deprecated */
		| "text-davinci-003";

	export type ModelInfo = {
		id: string;
		name: string;
		icon?: ImageLike;
	};

	export const getModels = async (): Promise<AI.ModelInfo[]> => {
		throw new Error("not implemented");
	};
}

type TokenData = {
	token: string;
	done: boolean;
};
