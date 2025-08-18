import { join } from 'path';
import { homedir } from 'os';
import { spawnSync } from 'child_process';

const platformDataDir = () => {
	const platform = process.platform;

	if (platform === 'linux') return  process.env.XDG_DATA_HOME || join(homedir(), '.local', 'share');
	if (platform === 'darwin') return join(homedir(), 'Library', 'Application Support');
	if (platform === 'win32') return process.env.APPDATA || join(homedir(), 'AppData', 'Roaming');

	return join(homedir(), '.data');
}

export const dataDir = () => join(platformDataDir(), 'vicinae');

export const extensionDataDir = () => join(dataDir(), "extensions");

export const invokeVicinae = (endpoint: string): Error | null => {
	if (endpoint.startsWith('/')) {
		endpoint = endpoint.slice(1);
	}

	const url = `vicinae://${endpoint}`; 
	const result = spawnSync("vicinae", [url]);

	if (result.error) return result.error;

	return result.status === 0 ? null : new Error(result.stderr.toString());
}
