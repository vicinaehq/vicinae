import { getGlobal } from "./globals";

/**
 * Client used to access vicinae capabilities from the extension runtime.
 * Always use this function instead of caching the client at module load time,
 * as the client is only initialized after the extension command is activated.
 */
export const getClient = () => getGlobal().client;
