import { join } from "path";
import { homedir } from "os";
import { spawnSync } from "child_process";

const platformDataDir = () => {
	const platform = process.platform;

	if (platform === "linux")
		return process.env.XDG_DATA_HOME || join(homedir(), ".local", "share");
	if (platform === "darwin")
		return join(homedir(), "Library", "Application Support");
	if (platform === "win32")
		return process.env.APPDATA || join(homedir(), "AppData", "Roaming");

	return join(homedir(), ".data");
};

export const dataDir = () => join(platformDataDir(), "vicinae");

export const extensionDataDir = () => join(dataDir(), "extensions");
