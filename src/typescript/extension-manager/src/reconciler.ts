import Reconciler, { type OpaqueRoot } from "react-reconciler";
import { setTimeout, clearTimeout } from "node:timers";
import { DefaultEventPriority } from "react-reconciler/constants";
import React, { type ReactElement } from "react";
import { callbackManager } from "./callback";

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Instance = {
	id: string;
	type: InstanceType;
	props: InstanceProps;
	parent?: Instance;
	children: Instance[];
	_handlers: string[];
	_createOpIdx?: number;
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

export type Op =
	| { op: "create"; id: string; type: string; props: InstanceProps; parentId?: string; index?: number }
	| { op: "update"; id: string; props: InstanceProps }
	| { op: "move"; id: string; parentId: string; index: number }
	| { op: "remove"; id: string };

const ctx: HostContext = {};

let nextNodeId = 0;
const genId = () => `n${nextNodeId++}`;

const detachInstance = (instance: Instance) => {
	for (const handler of instance._handlers)
		callbackManager.deferRemoval(handler);
	for (const child of instance.children) {
		detachInstance(child);
	}
};

type HostConfigDeps = {
	ops: Op[];
};

const pushOp = (deps: HostConfigDeps, op: Op) => {
	deps.ops.push(op);
};

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

type FormInstance = any;

const createHostConfig = (hostCtx: HostContext, callback: () => void, deps: HostConfigDeps) => {
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

			const nodeId = genId();
			const processedProps = processProps(initialProps);

			const createOpIdx = deps.ops.length;
			pushOp(deps, { op: "create", id: nodeId, type, props: processedProps });

			return {
				id: nodeId,
				type,
				props: processedProps,
				children: [],
				_handlers: handlers,
				_createOpIdx: createOpIdx,
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
		scheduleMicrotask: queueMicrotask,

		isPrimaryRenderer: true,

		getInstanceFromNode() {
			return null;
		},

		beforeActiveInstanceBlur() { },
		afterActiveInstanceBlur() { },

		prepareScopeUpdate(scope, instance) { },
		getInstanceFromScope(scope) {
			return null;
		},

		detachDeletedInstance(instance) { },

		appendChild(parent: Instance, child: Instance) {
			const wasUnparented = !child.parent;
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx !== -1) {
				parent.children.splice(selfIdx, 1);
			}

			child.parent = parent;
			parent.children.push(child);

			const index = parent.children.indexOf(child);

			if (wasUnparented && child._createOpIdx !== undefined) {
				const createOp = deps.ops[child._createOpIdx] as any;
				createOp.parentId = parent.id;
				createOp.index = index;
				delete child._createOpIdx;
			} else {
				pushOp(deps, { op: "move", id: child.id, parentId: parent.id, index });
			}
		},

		appendChildToContainer(container: Instance, child: Instance) {
			hostConfig.appendChild?.(container, child);
		},

		insertBefore(parent, child, beforeChild) {
			const beforeIndex = parent.children.indexOf(beforeChild);
			const wasUnparented = !child.parent;
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx != -1) {
				parent.children.splice(selfIdx, 1);
			}

			if (beforeIndex != -1) {
				parent.children.splice(beforeIndex, 0, child);
				child.parent = parent;

				const index = parent.children.indexOf(child);

				if (wasUnparented && child._createOpIdx !== undefined) {
					const createOp = deps.ops[child._createOpIdx] as any;
					createOp.parentId = parent.id;
					createOp.index = index;
					delete child._createOpIdx;
				} else {
					pushOp(deps, { op: "move", id: child.id, parentId: parent.id, index });
				}
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

			parent.children.splice(idx, 1);
			delete child.parent;
			detachInstance(child);

			pushOp(deps, { op: "remove", id: child.id });
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

			instance.props = props;

			pushOp(deps, { op: "update", id: instance.id, props });
		},

		replaceContainerChildren() { },
		hideInstance() { },
		hideTextInstance() { },
		unhideInstance() { },
		unhideTextInstance() { },

		clearContainer(container) {
			for (const child of container.children) {
				pushOp(deps, { op: "remove", id: child.id });
			}
			container.children = [];
		},

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

export type RendererConfig = {
	maxRendersPerSecond?: number;
	onUpdate: (ops: Op[]) => void;
};

const createContainer = (): Container => {
	return {
		id: "root",
		type: "root",
		props: {},
		children: [],
		_handlers: [],
	};
};

const MAX_RENDER_PER_SECOND = 60;

export const createRenderer = (config: RendererConfig) => {
	const container = createContainer();
	const ops: Op[] = [];

	let debounce: NodeJS.Timer | null = null;
	const debounceInterval = 1000 / MAX_RENDER_PER_SECOND;

	const renderImpl = () => {
		if (!debounce) {
			debounce = setTimeout(() => {
				debounce = null;
				if (ops.length === 0) return;

				config.onUpdate(ops.slice());
				callbackManager.flushDeferredRemovals();
				ops.length = 0;
			}, debounceInterval);
		}
	};

	const deps: HostConfigDeps = { ops };
	const hostConfig = createHostConfig({}, renderImpl, deps);
	const reconciler = Reconciler(
		process.env.RECONCILER_TRACE === "1"
			? traceWrap(hostConfig)
			: hostConfig,
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
