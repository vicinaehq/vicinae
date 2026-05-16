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

export type Op =
	| { op: "create"; id: string; type: string; props: InstanceProps }
	| { op: "update"; id: string; props: InstanceProps }
	| { op: "move"; id: string; parentId: string; index: number }
	| { op: "remove"; id: string };

type ShadowNode = {
	type: string;
	props: InstanceProps;
	dirty: boolean;
	propsDirty: boolean;
	parentId: string | null;
	childIds: string[];
};

const ctx: HostContext = {};

let nextNodeId = 0;
const genId = () => `n${nextNodeId++}`;

const emitDirty = (instance?: Instance) => {
	let current: Instance | undefined = instance;

	while (current) {
		current.dirty = true;
		current = current.parent;
	}
};

const emitShadowDirty = (nodes: Map<string, ShadowNode>, nodeId: string | null) => {
	let id = nodeId;
	while (id) {
		const node = nodes.get(id);
		if (!node) break;
		node.dirty = true;
		id = node.parentId;
	}
};

const applyShadowOp = (nodes: Map<string, ShadowNode>, op: Op) => {
	switch (op.op) {
		case "create":
			nodes.set(op.id, {
				type: op.type,
				props: op.props,
				dirty: true,
				propsDirty: true,
				parentId: null,
				childIds: [],
			});
			break;

		case "update": {
			const node = nodes.get(op.id);
			if (!node) break;
			node.props = op.props;
			node.propsDirty = true;
			emitShadowDirty(nodes, node.parentId);
			break;
		}

		case "move": {
			const node = nodes.get(op.id);
			const newParent = nodes.get(op.parentId);
			if (!node || !newParent) break;

			if (node.parentId) {
				const oldParent = nodes.get(node.parentId);
				if (oldParent) {
					const idx = oldParent.childIds.indexOf(op.id);
					if (idx !== -1) oldParent.childIds.splice(idx, 1);
				}
			}

			node.parentId = op.parentId;
			newParent.childIds.splice(op.index, 0, op.id);
			emitShadowDirty(nodes, op.parentId);
			break;
		}

		case "remove": {
			const node = nodes.get(op.id);
			if (!node) break;

			if (node.parentId) {
				const parent = nodes.get(node.parentId);
				if (parent) {
					const idx = parent.childIds.indexOf(op.id);
					if (idx !== -1) parent.childIds.splice(idx, 1);
					emitShadowDirty(nodes, node.parentId);
				}
			}

			const removeRecursive = (removeId: string) => {
				const n = nodes.get(removeId);
				if (!n) return;
				for (const childId of n.childIds) removeRecursive(childId);
				nodes.delete(removeId);
			};
			removeRecursive(op.id);
			break;
		}
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

const detachInstance = (instance: Instance) => {
	for (const handler of instance._handlers)
		callbackManager.deferRemoval(handler);
	for (const child of instance.children) {
		detachInstance(child);
	}
};

type FormInstance = any;

type HostConfigDeps = {
	ops: Op[];
	shadowNodes: Map<string, ShadowNode>;
};

const pushOp = (deps: HostConfigDeps, op: Op) => {
	deps.ops.push(op);
	applyShadowOp(deps.shadowNodes, op);
};

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

			pushOp(deps, { op: "create", id: nodeId, type, props: processedProps });

			return {
				id: nodeId,
				type,
				props: processedProps,
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
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx !== -1) {
				parent.children.splice(selfIdx, 1);
			}

			child.parent = parent;
			emitDirty(parent);
			parent.children.push(child);

			const index = parent.children.indexOf(child);
			pushOp(deps, { op: "move", id: child.id, parentId: parent.id, index });
		},

		appendChildToContainer(container: Instance, child: Instance) {
			hostConfig.appendChild?.(container, child);
		},

		insertBefore(parent, child, beforeChild) {
			const beforeIndex = parent.children.indexOf(beforeChild);
			const selfIdx = parent.children.indexOf(child);

			if (selfIdx != -1) {
				parent.children.splice(selfIdx, 1);
			}

			if (beforeIndex != -1) {
				parent.children.splice(beforeIndex, 0, child);
				child.parent = parent;
				emitDirty(parent);

				const index = parent.children.indexOf(child);
				pushOp(deps, { op: "move", id: child.id, parentId: parent.id, index });
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

			emitDirty(instance.parent);
			instance.propsDirty = true;
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

const serializeFromShadow = (nodes: Map<string, ShadowNode>, nodeId: string): SerializedInstance => {
	const node = nodes.get(nodeId)!;
	const obj: SerializedInstance = {
		props: node.props,
		type: node.type,
		dirty: node.dirty,
		propsDirty: node.propsDirty,
		children: node.childIds.map((id) => serializeFromShadow(nodes, id)),
	};

	node.dirty = false;
	node.propsDirty = false;

	return obj;
};

const createContainer = (): Container => {
	return {
		id: "root",
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
	const ops: Op[] = [];
	const shadowNodes = new Map<string, ShadowNode>();

	shadowNodes.set(container.id, {
		type: container.type,
		props: {},
		dirty: true,
		propsDirty: false,
		parentId: null,
		childIds: [],
	});

	let debounce: NodeJS.Timer | null = null;
	const debounceInterval = 1000 / MAX_RENDER_PER_SECOND;
	let lastRender = performance.now();

	const renderImpl = () => {
		if (!debounce) {
			debounce = setTimeout(() => {
				debounce = null;

				const start = performance.now();
				const root = serializeInstance(container);
				const shadowRoot = serializeFromShadow(shadowNodes, container.id);

				const rootJson = JSON.stringify(root);
				const shadowJson = JSON.stringify(shadowRoot);
				if (rootJson !== shadowJson) {
					console.error("[RECONCILER] Shadow tree mismatch!");
				} else {
					console.error(`[RECONCILER] Shadow tree OK (${ops.length} ops, ${shadowNodes.size} nodes)`);
				}

				const views = root.children.map<ViewData>((child) => ({
					root: child.children.at(-1),
				}));

				config.onUpdate?.(views);
				callbackManager.flushDeferredRemovals();
				ops.length = 0;

				const end = performance.now();
				lastRender = end;
			}, debounceInterval);
		}
	};

	const deps: HostConfigDeps = { ops, shadowNodes };
	const hostConfig = createHostConfig({}, renderImpl, deps);
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
