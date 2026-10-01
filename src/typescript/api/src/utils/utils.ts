import * as os from "node:os";
import * as path from "node:path";

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
