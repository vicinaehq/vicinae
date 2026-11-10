import { pathToFileURL } from "node:url";
import type { LaunchEventData, LaunchResult } from "../proto/extension-manager";

const serialize = (value: unknown): string => {
	if (value === undefined) return "";
	if (typeof value === "string") return value;
	return JSON.stringify(value);
};

export default async (data: LaunchEventData): Promise<LaunchResult> => {
	try {
		const module = await import(pathToFileURL(data.entrypoint).href);
		const tool = module.default.default;

		if (typeof tool !== "function") {
			throw new Error("tool does not export a function as its default export");
		}

		return { output: serialize(await tool(data.argumentValues)) };
	} catch (error) {
		return { error: error instanceof Error ? error.message : String(error) };
	}
};
