import { pathToFileURL } from "node:url";
import { Action, Alert, confirmAlert, type Tool } from "@vicinae/api";
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

		const confirmation: Tool.Confirmation<unknown> | undefined =
			module.default.confirmation;
		if (confirmation !== undefined && typeof confirmation !== "function") {
			throw new Error("tool confirmation export is not a function");
		}
		const options = await confirmation?.(data.argumentValues);
		if (options !== undefined || data.tool_requires_approval) {
			const details =
				options?.info
					?.filter((item) => item.value !== undefined)
					.map((item) => `${item.name}: ${item.value}`) ?? [];
			const message = [options?.message, ...details].filter(Boolean).join("\n");
			const input = JSON.stringify(data.argumentValues, null, 2);
			const confirmed = await confirmAlert({
				title: "",
				message: message || (input === "{}" ? "" : input),
				icon: options?.image,
				primaryAction: {
					title: "",
					style:
						options?.style === Action.Style.Destructive
							? Alert.ActionStyle.Destructive
							: Alert.ActionStyle.Default,
				},
				dismissAction: { title: "" },
			});
			if (!confirmed) return { error: "The tool was cancelled." };
		}

		return { output: serialize(await tool(data.argumentValues)) };
	} catch (error) {
		return { error: error instanceof Error ? error.message : String(error) };
	}
};
