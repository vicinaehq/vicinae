import Reconciler, { type OpaqueRoot } from "react-reconciler";
import { setTimeout, clearTimeout } from "node:timers";
import { DefaultEventPriority } from "react-reconciler/constants";
import React, { type ReactElement } from "react";
import { callbackManager } from "./callback";

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Instance = { $t: string; children?: Instance[]; [key: string]: any };
type Container = Instance;

type TextInstance = any;
type SuspenseInstance = any;
type HydratableInstance = any;
type PublicInstance = Instance;
type HostContext = {};
type UpdatePayload = any[];
type ChildSet = {};
type MyTimeoutHandle = number;
type NoTimeout = number;

const ctx: HostContext = {};
let frameGen = 0;

const initMeta = (
	instance: Instance,
	dirty: boolean,
	handlers: Set<string>,
) => {
	Object.defineProperties(instance, {
		_dirtyGen: { value: dirty ? frameGen : -1, writable: true },
		_parent: { value: undefined, writable: true },
		_handlers: { value: handlers, writable: true },
	});
};

const releaseHandler = (instance: Instance, id: string) => {
	callbackManager.deferRemoval(id);
	instance._handlers.delete(id);
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
	for (const handler of instance._handlers)
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
			const handlers = new Set<string>();
			const instance: Instance = { $t: type };

			for (const [k, v] of Object.entries(rest)) {
				if (React.isValidElement(v)) {
					console.error(`React element in props is ignored for key ${k}`);
					continue;
				}
				if (typeof v === "function") {
					const { id } = callbackManager.subscribe(v);
					instance[k] = id;
					handlers.add(id);
				} else {
					instance[k] = v;
				}
			}

			initMeta(instance, true, handlers);
			return instance;
		},

		createTextInstance() {
			throw new Error(`createTextInstance is not supported`);
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
			const beforeIndex = parent.children.indexOf(beforeChild);

			const selfIdx = parent.children.indexOf(child);
			if (selfIdx !== -1) parent.children.splice(selfIdx, 1);

			if (beforeIndex !== -1) {
				parent.children.splice(beforeIndex, 0, child);
				child._parent = parent;
				emitDirty(parent);
			} else {
				throw new Error("Unreachable");
			}
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
		commitTextUpdate() {},
		commitMount() {},

		commitUpdate(instance: Instance, type, prevProps, nextProps, handle) {
			for (const key of Object.keys(instance)) {
				if (key === "$t" || key === "children") continue;
				if (!(key in nextProps)) {
					const old = instance[key];
					if (typeof old === "string" && instance._handlers.has(old))
						releaseHandler(instance, old);
					delete instance[key];
				}
			}

			for (const [k, v] of Object.entries(nextProps)) {
				if (k === "children" || k === "key") continue;
				if (React.isValidElement(v)) continue;

				const old = instance[k];
				const oldIsHandler =
					typeof old === "string" && instance._handlers.has(old);

				if (typeof v === "function") {
					if (oldIsHandler) {
						callbackManager.setHandler(old, v);
						continue;
					}
					const { id } = callbackManager.subscribe(v);
					instance._handlers.add(id);
					instance[k] = id;
					continue;
				}

				if (oldIsHandler) releaseHandler(instance, old);
				instance[k] = v;
			}

			emitDirty(instance);
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

export const createRenderer = (config: RendererConfig) => {
	const container: Container = { $t: "root", children: [] };
	initMeta(container, true, new Set());

	let root: OpaqueRoot | undefined;
	let debounce: NodeJS.Timer | null = null;
	const debounceInterval = 1000 / MAX_RENDER_PER_SECOND;
	//let lastRender = performance.now();

	const renderImpl = () => {
		if (!debounce) {
			debounce = setTimeout(() => {
				debounce = null;

				//const start = performance.now();

				const views = (container.children ?? []).map<ViewData>((viewSlot) => {
					const viewRoot = viewSlot.children?.at(-1);
					if (!viewRoot) return { dirty: true };

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
	};
};
