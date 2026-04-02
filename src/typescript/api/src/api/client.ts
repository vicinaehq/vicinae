import type { Client } from "./proto/api";

/**
 * Client used to access vicinae capabitilies from the extension runtime.
 * The client is initialized when vicinae loads the extension command is activated.
 */
export const client = (globalThis as any).vicinae.client as Client;

export const getClient = () => (globalThis as any).vicinae.client as Client;
