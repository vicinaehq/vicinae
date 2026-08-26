import "./globals";
import { parentPort, workerData } from "node:worker_threads";
import {
	AI,
	BrowserExtension,
	FileSearch,
	LaunchType,
	Wallpaper,
	WindowManagement,
} from "@vicinae/api";
import { callbackManager } from "./callback";
import { globalState } from "./globals";
import loadNoView from "./loaders/load-no-view-command";
import loadView from "./loaders/load-view-command";
import { patchRequire } from "./patch-require";
import * as api from "./proto/api";
import * as extensionServer from "./proto/extension-manager";
import type { EnvironmentType } from "./types";

class Lifecycle extends extensionServer.LifecycleService {
	async launch(data: extensionServer.LaunchEventData): Promise<boolean> {
		const { environment } = workerData as { environment: EnvironmentType };

		// raycast compat captures preference values at load time: keep before patchRequire
		loadEnviron(environment, data);
		patchRequire(environment);
		(process as any).noDeprecation = environment === "production";

		if (data.mode === "View") {
			await loadView(data);
		} else {
			await loadNoView(data);
			// for no-view commands, we can request unload right away
			server.Lifecycle.emit_unload_requested();
		}

		return true;
	}

	async shutdown(): Promise<boolean> {
		if (globalState.renderer) {
			globalState.renderer.unmount();
		}

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
		parentPort?.postMessage(msg);
	},
});

const client = new api.Client(clientRpc);

client.EventCore.handlerActivated((id, data) => {
	callbackManager.activateHandler(id, data);
});

const mapLaunchType = (t: extensionServer.LaunchType) => {
	switch (t) {
		case "User":
			return LaunchType.UserInitiated;
		case "Background":
			return LaunchType.Background;
		case "CommandLine":
			return LaunchType.CommandLine;
	}
};

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
		canAccess: (api: unknown) => {
			if (api === WindowManagement) return data.capabilities.windowManagement;
			if (api === BrowserExtension) return data.capabilities.browserExtension;
			if (api === Wallpaper) return data.capabilities.wallpaper;
			if (api === FileSearch) return data.capabilities.fileSearch;
			if (api === AI) return false; // not supported yet

			return false;
		},
		isDevelopment: environment === "development",
		commandName: data.command_name,
		commandMode: data.mode === "View" ? "view" : "no-view",
		supportPath: data.support_path,
		assetsPath: data.asset_path,
		raycastVersion: "1.0.0", // provided for compatibility only, not meaningful
		launchType: mapLaunchType(data.launch_type),
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
