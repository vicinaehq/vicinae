import Reconciler, { OpaqueRoot } from 'react-reconciler';
import { setTimeout, clearTimeout } from 'node:timers';
import { DefaultEventPriority } from 'react-reconciler/constants';
import { ReactElement } from 'react';
import { bus } from '@omnicast/api';
import { randomBytes, randomUUID } from 'node:crypto';
import { deepClone, compare, Operation } from 'fast-json-patch';
import { inspect } from 'node:util';

type InstanceType = string;
type InstanceProps = Record<string, any>;
type Container = { _root?: OpaqueRoot, children: Instance[] };
type Instance = {
	type: InstanceType,
	props: InstanceProps;
	children: Instance[];
};
type TextInstance = any;
type SuspenseInstance = any;
type HydratableInstance = any;
type PublicInstance = Instance;
type HostContext = {};
type UpdatePayload = {};
type ChildSet = {};
type MyTimeoutHandle = number;
type NoTimeout = number;

const ctx: HostContext = {
};

const createEventHandlerId = () => {
	return `handler-${randomBytes(8).toString('hex')}`;
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

			for (const [k, v] of Object.entries(rest)) {
				if (typeof v == 'function') {
					const id = createEventHandlerId();

					bus!.subscribe(id, v);
					rest[k] = id;
				}
				else if (v instanceof URL) {
					rest[k] = `${v}`;
				}
			}

			return {
				type,
				props: rest,
				children: []
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
			return {};
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
		appendChild(parent, child) {
			parent.children.push(child);
		},

		appendChildToContainer(container, child) {
			container.children.push(child);
		},

		insertBefore(parent, child, beforeChild) {
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

		removeChild(parent, child) {
			const childIdx = parent.children.indexOf(child);

			parent.children.splice(childIdx, 1);
		},

		removeChildFromContainer(container, child) {
			const childIdx = container.children.indexOf(child);

			container.children.splice(childIdx, 1);
		},

		resetTextContent() {},

		commitTextUpdate() {},

		commitMount() {},

		commitUpdate(instance, payload, type, prevProps, nextProps, handle) {
			const { children, ...rest } = nextProps;

			for (const [k, v] of Object.entries(rest)) {
				if (typeof v == 'function') rest[k] = instance.props[k];
				if (v instanceof URL) rest[k] = instance.props[k];
			}

			instance.props = rest;
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

export type RendererConfig = {
	onInitialRender: (tree: Instance) => void
	onUpdate?: (tree: Instance, changes: Operation[]) => void;
};

export const createRenderer = (config: RendererConfig) => {
	const container: Container = { children: [] };
	let oldTree: Instance | null = null;

	const hostConfig = createHostConfig({}, () => {
		if (!oldTree) return ;

		const tree = container.children[container.children.length - 1];

		if (!tree) {
			console.error('no tree, WTF!!');
			return ;
		}

		const ops = compare(oldTree ?? {}, tree);

		for (const op of ops) {
			const unsafeOp = op as any;
			if (unsafeOp.value) delete unsafeOp.value;
		}

		config.onUpdate?.(tree, ops)
		oldTree = deepClone(tree);
	});
	const reconciler = Reconciler(hostConfig);

	return {
		render(element: ReactElement) {
			if (!container._root) {
				container._root = reconciler.createContainer(container, 0, null, false, null, '', (error) => {console.error('recoverable error', error)}, null);
			}

			reconciler.updateContainer(element, container._root, null, () => {
				const tree = container.children[container.children.length - 1];

				config.onInitialRender(tree)
				oldTree = deepClone(tree);
			});
		}
	};
}
