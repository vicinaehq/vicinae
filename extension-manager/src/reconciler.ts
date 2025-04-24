import Reconciler, { OpaqueRoot } from 'react-reconciler';
import { setTimeout, clearTimeout } from 'node:timers';
import { DefaultEventPriority } from 'react-reconciler/constants';
import { ReactElement } from 'react';
import { deepClone, compare, Operation } from 'fast-json-patch';
import { writeFileSync } from 'node:fs';

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Container = { _root?: OpaqueRoot, children: Instance[] };
type Instance = {
	type: InstanceType,
	props: InstanceProps;
	children: Instance[];
	dirty: boolean;
	propsDirty: boolean;
	parent?: Instance;
};
type TextInstance = any;
type SuspenseInstance = any;
type HydratableInstance = any;
type PublicInstance = Instance;
type HostContext = {};
type UpdatePayload = any[];
type ChildSet = {};
type MyTimeoutHandle = number;
type NoTimeout = number;

const ctx: HostContext = {
};

const isDeepEqual = (a: Record<any, any>, b: Record<any, any>): boolean => {
	for (const key in a) {
		if (typeof b[key] === 'undefined') return false;
	}

	for (const key in b) {
		if (typeof a[key] === 'undefined') return false;
	}

	for (const key in a) {
		const value = a[key];

		if (typeof b[key] !== typeof value) { return false; }

		if (typeof value === "object") {
			if (Array.isArray(value) && value.length !== b[key].length) {
				console.debug(`array shortcircuit optimization`);
				return false;
			}

			if (!isDeepEqual(value, b[key])) {
				return false;
			}

			continue ;
		}

		if (a[key] != b[key]) {
			return false;
		}
	}

	return true;
}

const emitDirty = (instance?: Instance) => {
	let current: Instance | undefined = instance;

	while (current && !current.dirty) {
		current.dirty = true;
		current = current.parent;
	}
}

const createHostConfig = (hostCtx: HostContext, callback: () => void) => {
	const hostConfig: Reconciler.HostConfig<
		InstanceType,
		InstanceProps,
		Container,
		Instance,
		TextInstance,
		SuspenseInstance,
		HydratableInstance,
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

		createInstance(type, props, root, ctx, handle) {
			const { children, ...rest } = props;

			return {
				type,
				props: rest,
				children: [],
				dirty: true,
				propsDirty: true,
			}
		},

		createTextInstance() {
			throw new Error(`createTextInstance is not supported`);
		},

		appendInitialChild(parent, child) {
			hostConfig.appendChild?.(parent, child)
		},

		finalizeInitialChildren(instance, type, props, root, ctx) {
			return false;
		},

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

		shouldSetTextContent() {
			return false;
		},

		getRootHostContext(root) { 
			return ctx;
		},

		getChildHostContext(parentHostContext, type, root) {
			return ctx;
		},

		getPublicInstance(instance) { return instance; },

		prepareForCommit(container) {
			return null;
		},

		resetAfterCommit() {
			callback();
			return null;
		},

		preparePortalMount(container) {},

		scheduleTimeout: setTimeout,
		cancelTimeout: (id) => clearTimeout(id),
		noTimeout: -1,
		supportsMicrotasks: false,
		scheduleMicrotask: queueMicrotask,

		isPrimaryRenderer: true,

		getCurrentEventPriority() {
			return DefaultEventPriority;
		},

		getInstanceFromNode() { return null; },

		beforeActiveInstanceBlur() {},
		afterActiveInstanceBlur() {},

		prepareScopeUpdate(scope, instance) {},
		getInstanceFromScope(scope) { return null },

		detachDeletedInstance(instance) {
			// todo: detach handlers
		},

		// mutation methods
		appendChild(parent: Instance, child: Instance) {
			child.parent = parent;
			emitDirty(parent);
			parent.children.push(child);
		},

		appendChildToContainer(container: Instance, child: Instance) {
			child.parent = container;
			emitDirty(container);
			container.children.push(child);
		},

		insertBefore(parent, child, beforeChild) {
			child.parent = parent;
			emitDirty(parent);

			const beforeIdx = parent.children.indexOf(beforeChild);

			if (beforeIdx == -1) {
				throw new Error(`insertBefore: beforeChild not found `);
			}

			if (beforeIdx == 0) parent.children.unshift(child);
			else parent.children.splice(beforeIdx - 1, 0, child);
		},

		insertInContainerBefore(container, child) {
			throw new Error(`root container can only have one child`);
		},

		removeChild(parent: Instance, child: Instance) {
			const childIdx = parent.children.indexOf(child);

			emitDirty(child.parent);
			delete child.parent;
			parent.children.splice(childIdx, 1);
		},

		removeChildFromContainer(container: Instance, child: Instance) {
			const childIdx = container.children.indexOf(child);

			emitDirty(child.parent);
			delete child.parent;
			container.children.splice(childIdx, 1);
		},

		resetTextContent() {},

		commitTextUpdate() {},

		commitMount() {},

		commitUpdate(instance: Instance, payload, type, prevProps, nextProps, handle) {
			let i = 0;
			
			if (payload.length > 0) {
				instance.propsDirty = true;
				emitDirty(instance.parent);
			}

			while (i < payload.length) {
				const key = payload[i++];
				const value = payload[i++];

				instance.props[key] = value;
			}
		},

		hideInstance() {},

		hideTextInstance() {},
		
		unhideInstance() {},

		unhideTextInstance() {},

		clearContainer(container) {
			container.children = [];
		},
	};

	return hostConfig;
}

export type ViewData = {
	root: Instance;
	changes: Operation[];
};

export type RendererConfig = {
	maxRendersPerSecond?: number,
	onInitialRender: (views: ViewData[]) => void
	onUpdate?: (views: ViewData[]) => void;
};

const stripParent = (instance: Instance): Instance => {
	const obj: Instance = {
		props: instance.props,
		type: instance.type,
		dirty: instance.dirty,
		propsDirty: instance.propsDirty,
		children: [],
	};

	instance.dirty = false;
	instance.propsDirty = false;

	for (const child of instance.children) {
		obj.children.push(stripParent(child));
	}
	
	return obj;
}

export const createRenderer = (config: RendererConfig) => {
	const { maxRendersPerSecond = 60 } = config;
	const frameTime = Math.floor(1000 / maxRendersPerSecond);
	const container: Container = { children: [] };
	let oldTree: Instance[] = [];
	let debounceTimeout: NodeJS.Timeout | null = null;

	const render = () => {
		if (debounceTimeout) { return; }

		debounceTimeout = setTimeout(() => {
			const start = performance.now();
			const views: ViewData[] = [];
			const rootNodes = container.children.map(stripParent);
			const changes = compare(oldTree, rootNodes);
			const didTreeChange = changes.length > 0;

			writeFileSync('/tmp/render.txt', JSON.stringify(rootNodes, null, 2));

			if (didTreeChange) {
				for (let i = 0; i != rootNodes.length; ++i) {
					const view = rootNodes[i];

					views.push({ root: view, changes: []});
				}

				config.onUpdate?.(views)
				oldTree = deepClone(rootNodes);
			}

			const end = performance.now();

			console.error(`[PERF] processed render frame in ${end - start}ms`);
			debounceTimeout = null;
		}, frameTime);

	}

	const hostConfig = createHostConfig({}, render);

	const reconciler = Reconciler(hostConfig);

	return {
		render(element: ReactElement) {
			if (!container._root) {
				container._root = reconciler.createContainer(container, 0, null, false, null, '', (error) => {console.error('recoverable error', error)}, null);
			}

			reconciler.updateContainer(element, container._root, null, render);
		}
	}
}
