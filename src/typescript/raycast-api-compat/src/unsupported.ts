// React stuff, must not resolve to a stub
const INTROSPECTION_KEYS = new Set([
	"$$typeof",
	"displayName",
	"defaultProps",
	"propTypes",
	"contextTypes",
	"then",
]);

export const unsupported = (symbol: string): any => {
	const fail = () => {
		throw new Error(
			`'${symbol}' from @raycast/api is not supported by Vicinae`,
		);
	};

	return new Proxy(fail, {
		apply: fail,
		construct: fail,
		get: (target, prop, receiver) => {
			if (typeof prop === "symbol") return Reflect.get(target, prop, receiver);
			if (prop === "name") return symbol;
			if (INTROSPECTION_KEYS.has(prop)) return undefined;
			return unsupported(`${symbol}.${String(prop)}`);
		},
	});
};

export const launchCommand = unsupported("launchCommand");
export const BrowserExtension = unsupported("BrowserExtension");
export const MenuBarExtra = unsupported("MenuBarExtra"); // we don't support menu bar commands for now, we may in the future :)
