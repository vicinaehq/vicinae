import Module from "node:module";

const requireOverrides: Record<string, any> = {
	react: require("react"),
	"react/jsx-runtime": require("react/jsx-runtime"),
	"@vicinae/api": require("@vicinae/api"),
	"@raycast/api": require("@vicinae/raycast-api-compat"),
};

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

export const patchRequire = () => {
	injectJsxGlobals();

	const originalRequire = Module.prototype.require;

	// @ts-ignore
	Module.prototype.require = function(id: string) {
		return requireOverrides[id] ?? originalRequire.call(this, id);
	};
};
