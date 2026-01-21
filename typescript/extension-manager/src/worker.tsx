import "./globals";
import type { Global } from "./globals";
import type { EnvironmentType } from "./types";
import { parentPort, workerData } from "node:worker_threads";
import { LaunchType, bus } from "@vicinae/api";
import { patchRequire } from "./patch-require";
import type { LaunchEventData } from "./proto/extension";
import { CommandMode } from "./proto/manager";
import loadView from "./loaders/load-view-command";
import loadNoView from "./loaders/load-no-view-command";

const loadEnviron = (environment: EnvironmentType, data: LaunchEventData) => {
	const g = globalThis as Global;
	Object.assign(g.vicinae.environ, {
		theme: "dark",
		textSize: "medium",
		appearance: "dark",
		canAccess: (_: unknown) => false,
		isDevelopment: environment === "development",
		commandName: data.commandName,
		commandMode: data.mode === CommandMode.View ? "view" : "no-view",
		supportPath: data.supportPath,
		assetsPath: data.assetPath,
		raycastVersion: "1.0.0", // provided for compatibility only, not meaningful
		launchType: LaunchType.UserInitiated,
		extensionName: data.extensionName,
		ownerOrAuthorName: data.ownerOrAuthorName,
		vicinaeVersion: {
			tag: process.env.VICINAE_VERSION ?? "unknown",
			commit: process.env.VICINAE_COMMIT ?? "unknown",
		},
		isRaycast: data.isRaycast,
	});
	g.vicinae.preferences = data.preferenceValues;
};

const loaders: Record<CommandMode, (data: LaunchEventData) => Promise<void>> = {
	[CommandMode.View]: loadView,
	[CommandMode.NoView]: loadNoView,
	[CommandMode.UNRECOGNIZED]: () => {
		throw new Error("Unsupported command type");
	},
};

export const main = async () => {
	if (!parentPort) {
		console.error(
			`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`,
		);
		return;
	}

	const { environment } = workerData as { environment: EnvironmentType };

	patchRequire(environment);

	// workers are provisioned before extension commands are launched, in order to avoid cold starts.
	// we need to wait for the manager to send the launch event to initialize stuff.
	// a worker is only used for a command once
	bus.onLaunch(async (data) => {
		loadEnviron(environment, data);
		(process as any).noDeprecation = environment === "production";
		await loaders[data.mode](data);
		bus.emit("exit", {});
	});
};
