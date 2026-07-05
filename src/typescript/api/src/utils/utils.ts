import * as os from "node:os";
import * as path from "node:path";

const platformDataDir = () => {
	if (process.platform === "win32")
		return process.env.APPDATA || path.join(os.homedir(), "AppData", "Roaming");
	if (process.platform === "darwin")
		return path.join(os.homedir(), ".local", "share");

	return (
		process.env.XDG_DATA_HOME || path.join(os.homedir(), ".local", "share")
	);
};

export const dataDir = () => path.join(platformDataDir(), "vicinae");

export const extensionDataDir = () => path.join(dataDir(), "extensions");

export const supportDir = path.join(dataDir(), "support");

export const extensionInternalSupportDir = (id: string) =>
	path.join(supportDir, id, ".vicinae");
