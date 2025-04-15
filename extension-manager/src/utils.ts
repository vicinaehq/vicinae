import { join } from 'path';
import { homedir } from 'os';
import { access } from 'fs/promises';
import { kill } from 'process';

const platformDataDir = () => {
	const platform = process.platform;

	if (platform === 'linux') return  process.env.XDG_DATA_HOME || join(homedir(), '.local', 'share');
	if (platform === 'darwin') return join(homedir(), 'Library', 'Application Support');
	if (platform === 'win32') return process.env.APPDATA || join(homedir(), 'AppData', 'Roaming');

	return join(homedir(), '.data');
}

export const dataDir = () => join(platformDataDir(), 'omnicast');

export const runtimeDir = () => join(dataDir(), 'runtime');

export const extensionDataDir = () => join(dataDir(), "extensions");

export const testMode = async (path: string): Promise<boolean> => {
	return new Promise<boolean>((resolve, _) => access(path).then(() => resolve(true)).catch(() => resolve(false)));
}

export const safeKill = (pid: number, signal: string): boolean => {
	try { kill(pid, signal); return true; }
	catch { return false; }
}
