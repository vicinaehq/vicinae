import type { LaunchEventData } from "../proto/extension-manager";
import { pathToFileURL } from "node:url";

export default async (data: LaunchEventData) => {
	const module = await import(pathToFileURL(data.entrypoint).href);
	const entrypoint = module.default.default;

	if (typeof entrypoint !== "function") {
		throw new Error(
			`no-view command does not export a function as its default export`,
		);
	}

	await entrypoint({
		arguments: data.argumentValues,
		launchContext: data.launch_context,
	});
};
