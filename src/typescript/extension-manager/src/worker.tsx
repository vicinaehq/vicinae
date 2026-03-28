import "./globals";
import type { Global } from "./globals";
import type { EnvironmentType } from "./types";
import { parentPort, workerData } from "node:worker_threads";
import { LaunchType } from "@vicinae/api";
import { patchRequire } from "./patch-require";
import loadView from "./loaders/load-view-command";
import loadNoView from "./loaders/load-no-view-command";
import * as extensionServer from "./proto/extension-manager";

const loadEnviron = (
	environment: EnvironmentType,
	data: extensionServer.LaunchEventData,
) => {
	const g = globalThis as Global;
	Object.assign(g.vicinae.environ, {
		theme: "dark",
		textSize: "medium",
		appearance: "dark",
		canAccess: (_: unknown) => false,
		isDevelopment: environment === "development",
		commandName: data.command_name,
		commandMode: data.mode === "View" ? "view" : "no-view",
		supportPath: data.support_path,
		assetsPath: data.asset_path,
		raycastVersion: "1.0.0", // provided for compatibility only, not meaningful
		launchType: LaunchType.UserInitiated,
		extensionName: data.extension_name,
		ownerOrAuthorName: data.owner_or_author_name,
		vicinaeVersion: {
			tag: process.env.VICINAE_VERSION ?? "unknown",
			commit: process.env.VICINAE_COMMIT ?? "unknown",
		},
		isRaycast: data.is_raycast,
	});
	//g.vicinae.preferences = data.preferenceValues;
};

const loaders: Record<
	extensionServer.CommandMode,
	(data: extensionServer.LaunchEventData) => Promise<void>
> = {
	View: loadView,
	NoView: loadNoView,
};

class Lifecycle extends extensionServer.Lifecycle {
	async launch(data: extensionServer.LaunchEventData): Promise<boolean> {
		const { environment } = workerData as { environment: EnvironmentType };

		patchRequire(environment);
		loadEnviron(environment, data);
		(process as any).noDeprecation = environment === "production";

		loaders[data.mode](data);
		return true;
	}

	async shutdown(): Promise<boolean> {
		return true;
	}

	async send_message(msg: string): Promise<boolean> {
		return false;
	}
}

const transport = new extensionServer.RpcTransport({
	send: (msg) => parentPort?.postMessage(msg),
});

const server = new extensionServer.Server(transport, new Lifecycle(transport));

export const main = async () => {
	if (!parentPort) {
		console.error(
			`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`,
		);
		return;
	}

	parentPort.on("message", (msg) => server.route(msg));

	// workers are provisioned before extension commands are launched, in order to avoid cold starts.
	// we need to wait for the manager to send the launch event to initialize stuff.
	// a worker is only used for a command once
};
