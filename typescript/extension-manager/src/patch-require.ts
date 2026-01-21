import Module from "node:module";
import type { EnvironmentType } from "./types";

const injectJsxGlobals = () => {
	const { jsx, jsxs, Fragment } = require("react/jsx-runtime");

	// react/jsx-runtime always expect non-null props
	const safeJsx =
		(original: typeof jsx) =>
			(type: React.ElementType, props: unknown, key: React.Key) =>
				original(type, props ?? {}, key);

	(globalThis as any)._jsx = safeJsx(jsx);
	(globalThis as any)._jsxs = safeJsx(jsxs);
	(globalThis as any)._jsxFragment = Fragment;
};

export const patchRequire = (env: EnvironmentType) => {
	// we do not want NODE_ENV to be set in the extension worker as it will get inherited by
	// child processes which may yield to a lot of unexpected behaviour.
	// We tried many things: the method below has been the most reliable so far.
	process.env.NODE_ENV = env;
	const reconciler = require("react-reconciler");
	const react = require("react");
	const jsxRuntime = require("react/jsx-runtime");
	const api = require("@vicinae/api");
	const apiCompat = require("@vicinae/raycast-api-compat");
	delete process.env.NODE_ENV;

	const requireOverrides: Record<string, any> = {
		react: () => react,
		"react/jsx-runtime": () => jsxRuntime,
		"react-reconciler": () => reconciler,
		"@vicinae/api": () => api,
		"@raycast/api": () => apiCompat,
	};
	const originalRequire = Module.prototype.require;

	injectJsxGlobals();

	// @ts-ignore
	Module.prototype.require = function(id: string) {
		return requireOverrides[id]?.() ?? originalRequire.call(this, id);
	};
};
