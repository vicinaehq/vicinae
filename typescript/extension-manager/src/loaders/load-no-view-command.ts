import { LaunchEventData } from "../proto/extension";

export default async (data: LaunchEventData) => {
	const module = await import(data.entrypoint);
	const entrypoint = module.default.default;

	if (typeof entrypoint !== "function") {
		throw new Error(
			`no-view command does not export a function as its default export`,
		);
	}

	await entrypoint({ arguments: data.argumentValues });
};
