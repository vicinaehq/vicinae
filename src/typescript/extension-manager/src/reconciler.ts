import Reconciler, { type OpaqueRoot } from "react-reconciler";
import { setTimeout, clearTimeout } from "node:timers";
import { DefaultEventPriority } from "react-reconciler/constants";
import React, { type ReactElement } from "react";
import { callbackManager } from "./callback";

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Instance = { $t: string; children?: Instance[]; [key: string]: any };
type Container = Instance;

type TextInstance = Instance;
type SuspenseInstance = any;
type HydratableInstance = any;
type PublicInstance = Instance;
type HostContext = {};
type UpdatePayload = any[];
type ChildSet = {};
type MyTimeoutHandle = number;
type NoTimeout = number;

const ctx: HostContext = {};
const TEXT_TAG = "#text";
let frameGen = 0;
let hasTextNodes = false;

const isText = (node: unknown): boolean =>
	(node as Instance | null | undefined)?.$t === TEXT_TAG;

const initMeta = (
	instance: Instance,
	dirty: boolean,
	handlers: Map<string, string>,
) => {
	Object.defineProperties(instance, {
		_dirtyGen: { value: dirty ? frameGen : -1, writable: true },
		_parent: { value: undefined, writable: true },
		_handlers: { value: handlers, writable: true },
	});
};

const releaseHandler = (instance: Instance, prop: string) => {
	callbackManager.deferRemoval(instance._handlers.get(prop));
	instance._handlers.delete(prop);
};

const isPlain = (proto: unknown) =>
	proto === Object.prototype || proto === null;

/**
 * Whether two prop values serialize to the same thing. Components rebuild objects such as
 * icons and accessories on every render, so identity alone would flag every node as changed.
 * Anything that is not a plain object or array is only ever equal to itself.
 */
const isWireEqual = (a: unknown, b: unknown): boolean => {
	if (a === b) return true;
	if (typeof a !== "object" || typeof b !== "object" || !a || !b) return false;

	if (Array.isArray(a)) {
		if (!Array.isArray(b) || a.length !== b.length) return false;
		return a.every((value, idx) => isWireEqual(value, b[idx]));
	}

	if (!isPlain(Object.getPrototypeOf(a)) || !isPlain(Object.getPrototypeOf(b)))
		return false;

	const keys = Object.keys(a);
	if (keys.length !== Object.keys(b).length) return false;

	return keys.every(
		(key) =>
			key in b &&
			isWireEqual(
				(a as Record<string, unknown>)[key],
				(b as Record<string, unknown>)[key],
			),
	);
};

const emitDirty = (instance?: Instance) => {
	let current = instance;
	while (current) {
		if (current._dirtyGen === frameGen) break;
		current._dirtyGen = frameGen;
		current = current._parent;
	}
};

function traceWrap(hostConfig: any) {
	let traceWrappedHostConfig = {} as any;
	Object.keys(hostConfig).map((key) => {
		const func = hostConfig[key];
		traceWrappedHostConfig[key] = (...args: any[]) => {
			console.log(key);
			return func(...args);
		};
	});
	return traceWrappedHostConfig;
}

const detachInstance = (instance: Instance) => {
	for (const handler of instance._handlers.values())
		callbackManager.deferRemoval(handler);
	if (instance.children) {
		for (const child of instance.children) detachInstance(child);
	}
};

type FormInstance = any;

const createHostConfig = (hostCtx: HostContext, callback: () => void) => {
	const hostConfig: Reconciler.HostConfig<
		InstanceType,
		InstanceProps,
		Container,
		Instance,
		TextInstance,
		SuspenseInstance,
		HydratableInstance,
		FormInstance,
		PublicInstance,
		HostContext,
		UpdatePayload,
		ChildSet,
		MyTimeoutHandle,
		NoTimeout
	> = {
		supportsMutation: true,
		supportsPersistence: false,
		supportsHydration: false,

		createInstance(type, props, root, ctx, handle): Instance {
			const { children, key, ...rest } = props;
			const handlers = new Map<string, string>();
			const instance: Instance = { $t: type };

			for (const [k, v] of Object.entries(rest)) {
				if (React.isValidElement(v)) {
					console.error(`React element in props is ignored for key ${k}`);
					continue;
				}
				if (typeof v === "function") {
					const { id } = callbackManager.subscribe(v);
					instance[k] = id;
					handlers.set(k, id);
				} else {
					instance[k] = v;
				}
			}

			initMeta(instance, true, handlers);
			return instance;
		},

		createTextInstance(text: string) {
			const instance: TextInstance = { $t: TEXT_TAG, text };
			initMeta(instance, true, new Map());
			hasTextNodes = true;
			return instance;
		},

		appendInitialChild(parent, child) {
			hostConfig.appendChild?.(parent, child);
		},

		finalizeInitialChildren(instance, type, props, root, ctx) {
			return false;
		},

		shouldSetTextContent() {
			return false;
		},

		getRootHostContext(root) {
			return ctx;
		},

		getChildHostContext(parentHostContext, type, root) {
			return ctx;
		},

		getPublicInstance(instance) {
			return instance;
		},

		prepareForCommit(container) {
			return null;
		},

		resetAfterCommit() {
			callback();
			return null;
		},

		preparePortalMount(container) {},

		scheduleTimeout: setTimeout,
		cancelTimeout: (id: MyTimeoutHandle) => clearTimeout(id),
		noTimeout: -1,
		scheduleMicrotask: queueMicrotask,

		isPrimaryRenderer: true,

		getInstanceFromNode() {
			return null;
		},

		beforeActiveInstanceBlur() {},
		afterActiveInstanceBlur() {},

		prepareScopeUpdate(scope, instance) {},
		getInstanceFromScope(scope) {
			return null;
		},

		detachDeletedInstance(instance) {},

		appendChild(parent: Instance, child: Instance) {
			if (parent.children) {
				const selfIdx = parent.children.indexOf(child);
				if (selfIdx !== -1) parent.children.splice(selfIdx, 1);
				parent.children.push(child);
			} else {
				parent.children = [child];
			}

			child._parent = parent;
			emitDirty(parent);
		},

		appendChildToContainer(container: Instance, child: Instance) {
			hostConfig.appendChild?.(container, child);
		},

		insertBefore(parent, child, beforeChild) {
			if (!parent.children) parent.children = [];

			const selfIdx = parent.children.indexOf(child);
			if (selfIdx !== -1) parent.children.splice(selfIdx, 1);

			const beforeIndex = parent.children.indexOf(beforeChild);
			if (beforeIndex === -1) {
				throw new Error("Unreachable");
			}

			parent.children.splice(beforeIndex, 0, child);
			child._parent = parent;
			emitDirty(parent);
		},

		insertInContainerBefore(container, child, beforeChild) {
			hostConfig.insertBefore?.(container, child, beforeChild);
		},

		removeChild(parent: Instance, child: Instance) {
			if (!parent.children) return;
			const idx = parent.children.indexOf(child);
			if (idx === -1) return;

			emitDirty(parent);
			parent.children.splice(idx, 1);
			if (parent.children.length === 0) delete parent.children;

			child._parent = undefined;
			detachInstance(child);
		},

		removeChildFromContainer(container: Instance, child: Instance) {
			hostConfig.removeChild?.(container, child);
		},

		resetTextContent() {},
		commitTextUpdate(
			textInstance: TextInstance,
			_oldText: string,
			newText: string,
		) {
			textInstance.text = newText;
		},
		commitMount() {},

		commitUpdate(instance: Instance, type, prevProps, nextProps, handle) {
			let changed = false;

			for (const key of Object.keys(instance)) {
				if (key === "$t" || key === "children") continue;
				if (!(key in nextProps)) {
					if (instance._handlers.has(key)) releaseHandler(instance, key);
					delete instance[key];
					changed = true;
				}
			}

			for (const [k, v] of Object.entries(nextProps)) {
				if (k === "children" || k === "key") continue;
				if (React.isValidElement(v)) continue;

				const handlerId = instance._handlers.get(k);

				if (typeof v === "function") {
					if (handlerId) {
						callbackManager.setHandler(handlerId, v);
						continue;
					}
					const { id } = callbackManager.subscribe(v);
					instance._handlers.set(k, id);
					instance[k] = id;
					changed = true;
					continue;
				}

				if (handlerId) releaseHandler(instance, k);
				else if (isWireEqual(instance[k], v)) continue;

				instance[k] = v;
				changed = true;
			}

			if (changed) emitDirty(instance);
		},

		replaceContainerChildren() {},
		hideInstance() {},
		hideTextInstance() {},
		unhideInstance() {},
		unhideTextInstance() {},

		clearContainer(container) {
			delete container.children;
		},

		NotPendingTransition: null,
		HostTransitionContext: {} as any,

		setCurrentUpdatePriority(priority) {},

		getCurrentUpdatePriority() {
			return DefaultEventPriority;
		},

		resolveUpdatePriority() {
			return DefaultEventPriority;
		},

		resetFormInstance(form) {},
		requestPostPaintCallback() {},
		shouldAttemptEagerTransition() {
			return false;
		},

		trackSchedulerEvent() {},
		resolveEventType() {
			return null;
		},
		resolveEventTimeStamp() {
			return Date.now();
		},
		maySuspendCommit() {
			return false;
		},

		preloadInstance(type, props) {
			return false;
		},
		startSuspendingCommit() {},
		suspendInstance(type, props) {},
		waitForCommitToBeReady() {
			return null;
		},
	};

	return hostConfig;
};

export type ViewData = {
	dirty: boolean;
	root?: Record<string, any>;
};

export type RendererConfig = {
	maxRendersPerSecond?: number;
	onInitialRender: (views: ViewData[]) => void;
	onUpdate?: (views: ViewData[]) => void;
};

const MAX_RENDER_PER_SECOND = 60;

const stripTextNodes = (key: string, value: unknown) =>
	key === "children" && Array.isArray(value) && value.some(isText)
		? value.filter((node) => !isText(node))
		: value;

// a replacer makes serialization about three times slower, most trees have no text node
export const serializeViews = (views: ViewData[]) =>
	JSON.stringify({ views }, hasTextNodes ? stripTextNodes : undefined);

export const createRenderer = (config: RendererConfig) => {
	const container: Container = { $t: "root", children: [] };
	initMeta(container, true, new Map());

	let root: OpaqueRoot | undefined;
	let debounce: NodeJS.Timer | null = null;
	const debounceInterval = 1000 / MAX_RENDER_PER_SECOND;
	//let lastRender = performance.now();

	const renderImpl = () => {
		if (!debounce) {
			debounce = setTimeout(() => {
				debounce = null;

				if (container._dirtyGen !== frameGen) return;

				//const start = performance.now();

				const views = (container.children ?? []).map<ViewData>((viewSlot) => {
					const viewRoot = viewSlot.children?.at(-1);
					if (!viewRoot || isText(viewRoot)) return { dirty: true };

					const dirty = viewRoot._dirtyGen === frameGen;
					return { dirty, root: dirty ? viewRoot : undefined };
				});

				config.onUpdate?.(views);
				callbackManager.flushDeferredRemovals();
				frameGen++;

				//const end = performance.now();

				/*
				console.error(
					`[PERF] reconciler frame: ${(end - start).toFixed(2)}ms (since last: ${(end - lastRender).toFixed(1)}ms)`,
				);
				*/
				//lastRender = end;
			}, debounceInterval);
		}
	};

	const hostConfig = createHostConfig({}, renderImpl);
	const reconciler = Reconciler(
		process.env.RECONCILER_TRACE === "1" ? traceWrap(hostConfig) : hostConfig,
	);

	return {
		flushSync: (reconciler as any).flushSyncFromReconciler.bind(reconciler),
		render(element: ReactElement) {
			if (!root) {
				root = reconciler.createContainer(
					container,
					0,
					null,
					false,
					null,
					"",
					(error) => {
						throw error;
					},
					(error) => {
						throw error;
					},
					(error) => {
						throw error;
					},
					() => {},
					null,
				);
			}

			reconciler.updateContainer(element, root, null, renderImpl);
		},
		unmount() {
			if (!root) return;

			const currentRoot = root;

			(reconciler as any).flushSyncFromReconciler(() => {
				reconciler.updateContainer(null, currentRoot, null, () => {});
			});

			root = undefined;
		},
	};
};
