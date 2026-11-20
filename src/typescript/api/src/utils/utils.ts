import * as os from "node:os";
import * as path from "node:path";
import type { BuildOptions } from "esbuild";

export const dataDir = () => {
	if (process.platform === "win32") {
		const local =
			process.env.LOCALAPPDATA ?? path.join(os.homedir(), "AppData", "Local");
		return path.join(local, "vicinae", "data");
	}
	if (process.platform === "darwin")
		return path.join(os.homedir(), ".local", "share", "vicinae");

	return path.join(
		process.env.XDG_DATA_HOME ?? path.join(os.homedir(), ".local", "share"),
		"vicinae",
	);
};

export const extensionDataDir = () => path.join(dataDir(), "extensions");

export const supportDir = path.join(dataDir(), "support");

export const extensionInternalSupportDir = (id: string) =>
	path.join(supportDir, id, ".vicinae");

export const BUILD_OPTIONS = {
	bundle: true,
	external: ["react", "react/jsx-runtime", "@vicinae/api", "@raycast/api"],
	format: "cjs",
	jsx: "automatic",
	platform: "node",
	loader: {
		".node": "file",
	},
} satisfies BuildOptions;
