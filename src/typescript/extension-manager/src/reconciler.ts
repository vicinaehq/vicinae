import Reconciler, { type OpaqueRoot } from "react-reconciler";
import { setTimeout, clearTimeout } from "node:timers";
import { DefaultEventPriority } from "react-reconciler/constants";
import React, { type ReactElement } from "react";
import { callbackManager } from "./callback";

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Instance = {
	id: Symbol;
	type: InstanceType;
	props: InstanceProps;
	dirty: boolean;
	propsDirty: boolean;
	parent?: Instance;
	children: Instance[];
	_handlers: string[];
};
type Container = Instance & { _root?: OpaqueRoot };
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

const emitDirty = (instance?: Instance) => {
	let current: Instance | undefined = instance;

	//while (current && !current.dirty) {
	while (current) {
		current.dirty = true;
		current = current.parent;
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

const processProps = (props: Record<string, any>): Record<string, any> => {
	const sanitized: Record<string, any> = {};

	for (const [k, v] of Object.entries(props)) {
		if (React.isValidElement(v)) {
			console.error(`React element in props is ignored for key ${k}`);
		} else if (k !== "children") {
			sanitized[k] = v;
		}
	}

	return sanitized;
};

/**
 * Cleanup all event handlers and other things that are related to the instance
 */
const detachInstance = (instance: Instance) => {
	for (const handler of instance._handlers)
		callbackManager.deferRemoval(handler);
	for (const child of instance.children) {
		detachInstance(child);
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
			let { children, key, ...rest } = props;

			const initialProps = rest;
			const handlers: string[] = [];

			for (const [k, v] of Object.entries(rest)) {
				if (typeof v === "function") {
					const { id } = callbackManager.subscribe(v);
					initialProps[k] = id;
					handlers.push(id);
				} else {
					initialProps[k] = v;
				}
			}

			return {
				id: Symbol(type),
				type,
				props: processProps(initialProps),
				children: [],
				dirty: true,
				propsDirty: true,
				_handlers: handlers,
			};
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

		/*
		prepareUpdate(instance, type, oldProps, newProps, root, ctx) {
			const changes = [];

			for (const key in newProps) {
				if (key === 'children') { continue ; }

				const oldValue = oldProps[key];
				const newValue = newProps[key];

				if (typeof oldValue !== typeof newValue) {
					changes.push(key, newValue);
					continue ;
				}

				if (typeof newValue === 'object') {
					if (!isDeepEqual(newValue, oldValue)) {
						changes.push(key, newValue);
					}
					continue ;
				}

				if (oldValue !== newValue) {
					changes.push(key, newValue);
				}
			}

			return changes.length > 0 ? changes : null;
		},
		*/

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

		preparePortalMount(container) { },

		scheduleTimeout: setTimeout,
		cancelTimeout: (id: MyTimeoutHandle) => clearTimeout(id),
		noTimeout: -1,
		//supportsMicrotasks: false,
		scheduleMicrotask: queueMicrotask,

		isPrimaryRenderer: true,

		/*
		getCurrentEventPriority() {
			return DefaultEventPriority;
		},
		*/

		getInstanceFromNode() {
			return null;
		},

		beforeActiveInstanceBlur() { },
		afterActiveInstanceBlur() { },

		prepareScopeUpdate(scope, instance) { },
		getInstanceFromScope(scope) {
			return null;
		},

		// not sure what this one is really about, as it's undocumented
		detachDeletedInstance(instance) { },

		appendChild(parent: Instance, child: Instance) {
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx !== -1) {
				parent.children.splice(selfIdx, 1);
			}

			child.parent = parent;
			emitDirty(parent);
			parent.children.push(child);
		},

		appendChildToContainer(container: Instance, child: Instance) {
			hostConfig.appendChild?.(container, child);
		},

		insertBefore(parent, child, beforeChild) {
			const beforeIndex = parent.children.indexOf(beforeChild);

			// insertBefore is used for reordering
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx != -1) {
				parent.children.splice(selfIdx, 1);
			}

			if (beforeIndex != -1) {
				parent.children.splice(beforeIndex, 0, child);
				child.parent = parent;
				emitDirty(parent);
			} else {
				throw new Error("Unreachable");
			}
		},

		insertInContainerBefore(container, child, beforeChild) {
			hostConfig.insertBefore?.(container, child, beforeChild);
		},

		removeChild(parent: Instance, child: Instance) {
			const idx = parent.children.indexOf(child);

			if (idx == -1) return;

			emitDirty(parent);
			parent.children.splice(idx, 1);
			delete child.parent;
			detachInstance(child);
		},

		removeChildFromContainer(container: Instance, child: Instance) {
			hostConfig.removeChild?.(container, child);
		},

		resetTextContent() { },

		commitTextUpdate() { },

		commitMount() { },

		commitUpdate(instance: Instance, type, prevProps, nextProps, handle) {
			const props: Record<string, any> = {};

			for (const [k, v] of Object.entries(nextProps)) {
				if (k == "children") continue;

				if (React.isValidElement(v)) {
					console.warn(
						`Received react element as prop (key '${k}'), which is unsupported.`,
					);
					continue;
				}

				if (typeof v === "function") {
					const oldHandlerId = instance.props[k];

					if (typeof oldHandlerId === "string") {
						callbackManager.setHandler(oldHandlerId, v);
						props[k] = oldHandlerId;
						continue;
					}

					const { id } = callbackManager.subscribe(v);

					instance._handlers.push(id);
					props[k] = id;
					continue;
				}

				props[k] = v;
			}

			emitDirty(instance.parent);
			instance.propsDirty = true;
			instance.props = props;
		},

		replaceContainerChildren() { },
		hideInstance() { },
		hideTextInstance() { },
		unhideInstance() { },
		unhideTextInstance() { },

		clearContainer(container) {
			container.children = [];
		},

		/** added for react 19 - we don't have to implement most of this */
		NotPendingTransition: null,
		HostTransitionContext: {} as any,

		setCurrentUpdatePriority(priority) { },

		getCurrentUpdatePriority() {
			return DefaultEventPriority;
		},

		resolveUpdatePriority() {
			return DefaultEventPriority;
		},

		resetFormInstance(form) { },
		requestPostPaintCallback() { },
		shouldAttemptEagerTransition() {
			return false;
		},

		trackSchedulerEvent() { },
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
		startSuspendingCommit() { },
		suspendInstance(type, props) { },
		waitForCommitToBeReady() {
			return null;
		},
	};

	return hostConfig;
};

export type ViewData = {
	root?: SerializedInstance;
};

export type RendererConfig = {
	maxRendersPerSecond?: number;
	onInitialRender: (views: ViewData[]) => void;
	onUpdate?: (views: ViewData[]) => void;
};

type SerializedInstance = {
	props: InstanceProps;
	type: string;
	dirty: boolean;
	propsDirty: boolean;
	children: SerializedInstance[];
};

const serializeInstance = (instance: Instance): SerializedInstance => {
	const obj: SerializedInstance = {
		props: instance.props,
		type: instance.type,
		dirty: instance.dirty,
		propsDirty: instance.propsDirty,
		children: instance.children.map(serializeInstance),
	};

	instance.dirty = false;
	instance.propsDirty = false;

	return obj;
};

const createContainer = (): Container => {
	return {
		id: Symbol("root"),
		type: "root",
		dirty: true,
		propsDirty: false,
		props: {},
		children: [],
		_handlers: [],
	};
};

const MAX_RENDER_PER_SECOND = 60;

export const createRenderer = (config: RendererConfig) => {
	const container = createContainer();
	let debounce: NodeJS.Timer | null = null;
	const debounceInterval = 1000 / MAX_RENDER_PER_SECOND;
	let lastRender = performance.now();

	const renderImpl = () => {
		if (!debounce) {
			debounce = setTimeout(() => {
				debounce = null;

				const start = performance.now();
				const root = serializeInstance(container);

				//writeFileSync("/tmp/render.txt", `${inspect(root, { depth: null, colors: true })}`);
				//appendFileSync('/tmp/render.txt', JSON.stringify(root, null, 2));

				const views = root.children.map<ViewData>((child) => ({
					root: child.children.at(-1),
				}));

				config.onUpdate?.(views);
				callbackManager.flushDeferredRemovals();

				const end = performance.now();

				//console.error(`[PERF] processed render frame in ${end - start}ms`);
				///console.error(`[PERF] last render ${end - lastRender}ms`);
				lastRender = end;
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
			if (!container._root) {
				container._root = reconciler.createContainer(
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
					() => { },
					null,
				);
			}

			reconciler.updateContainer(element, container._root, null, renderImpl);
		},
	};
};
