import "./globals";
import { globalState } from "./globals";
import type { EnvironmentType } from "./types";
import { parentPort, workerData } from "node:worker_threads";
import { LaunchType } from "@vicinae/api";
import { patchRequire } from "./patch-require";
import loadView from "./loaders/load-view-command";
import loadNoView from "./loaders/load-no-view-command";
import * as extensionServer from "./proto/extension-manager";
import * as api from "./proto/api";
import { callbackManager } from "./callback";

const loaders: Record<
	extensionServer.CommandMode,
	(data: extensionServer.LaunchEventData) => Promise<void>
> = {
	View: loadView,
	NoView: loadNoView,
};

class Lifecycle extends extensionServer.LifecycleService {
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
		client.route(msg);
		return true;
	}
}

const serverRpc = new extensionServer.RpcTransport({
	send: (msg) => {
		parentPort?.postMessage(msg);
	},
});

const server = new extensionServer.Server(serverRpc, new Lifecycle(serverRpc));

const clientRpc = new api.RpcTransport({
	send: (msg: string) => {
		//server.Lifecycle.emit_extension_message(msg);
		parentPort?.postMessage(msg);
	},
});

const client = new api.Client(clientRpc);

client.EventCore.handlerActivated((id, data) => {
	callbackManager.activateHandler(id, data);
});

const loadEnviron = (
	environment: EnvironmentType,
	data: extensionServer.LaunchEventData,
) => {
	globalState.client = client;
	globalState.preferences = data.preferenceValues;
	Object.assign(globalState.environ, {
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
};

export const main = async () => {
	if (!parentPort) {
		console.error(
			`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`,
		);
		return;
	}

	parentPort.on("message", (msg) => {
		server.route(msg);
	});
};
