// @ts-nocheck
import Reconciler from 'react-reconciler';
import { DefaultEventPriority } from 'react-reconciler/constants.js';
import type { ReactElement } from 'react';
import { bus } from '@omnicast/api';
import { randomUUID } from 'crypto';

const instanceTypes = [
	'list',
	'list-item',
	'detail',
	'detail-metadata',
	'detail-metadata-label',
];

type InstanceType = typeof instanceTypes[number];

type Instance = {
	type: InstanceType;
	props: Record<string, any>;
	children: Instance[];
};

type Container = {
	children: Instance[];
	_root: any | null;
}

type RenderCallback = (current: Instance[], previous?: Instance[]) => void;

type HostContext = {
	afterCommitCallbacks: RenderCallback[];
};

class HostConfig {
	cb: Function | null = null

	onResetAfterCommit = (cb: Function) => { this.cb = cb; }

	constructor(private readonly ctx: HostContext) {}

			supportsMutation = true

			createInstance(type, props, root, hostCtx, handle) {
				try {
					const { children, ...rest } = props;

					for (const [k, v] of Object.entries(rest)) {
						if (k.startsWith('on')) {
							rest[k] = randomUUID();
							bus!.subscribe(rest[k], v);
						}

						if (v instanceof URL) {
							rest[k] = v.toString();
						}
					}

					return {
						type,
						props: rest,
					};
				} catch (error) {
					console.error('failed to create instance', error);
				}
			}

			createTextInstance(text, root, hostCtx, handle) {
				throw new Error(`Text nodes are not supported: ${text}`);
			}

			appendInitialChild = (parent, child) => {
				this.appendChild(parent, child);
			}

			finalizeInitialChildren(instance, type, props, rootContainer, hostContext) {
				return false;
			}

			shouldSetTextContent(type, props) {
				return false;
			}

			onUncaughtError(error) {
				throw error;
			}

			getRootHostContext = () => {
				return this.ctx;
			}

			getChildHostContext = (parentHostCtx, type, root) => {
				return this.ctx;
			}

			getPublicInstance(instance) {
				return instance;
			}

			prepareForCommit(containerInfo) {
				return null;
			}

			resetAfterCommit = (containerInfo) => {
				if (this.cb) this.cb();
			} 

			preparePortalMount(containerInfo) {}

			scheduleTimeout = setTimeout

			cancelTimeout = clearTimeout

			noTimeout = -1

			supportsMicrotasks = true

			scheduleMicrotask = queueMicrotask

			isPrimaryRenderer = true

			getCurrentEventPriority() {
				return DefaultEventPriority;
			}

			getCurrentUpdatePriority() {
				return DefaultEventPriority;
			}

			setCurrentUpdatePriority() {
			}

			resolveUpdatePriority() {
				return DefaultEventPriority;
			}

			appendChild(parent, child) {
				try {
					if (parent.type == "list-item" && child.type == "list-item-detail") {
						parent.props.detail = child;
						return ;
					}

					if (parent.type == "list-item" && child.type == "action-pannel") {
						parent.props.actions = child;
					}

					if (parent.type == "list-item-detail" && child.type == "list-item-detail-metadata") {
						parent.props.metadata = child;
						return ;
					}
					
					if (parent.children) {
						parent.children.push(child);
						return ;
					}

					parent.children = [child];
				} catch (error) {
					console.error(`Failed to appendChild`, error);
				}
			}

			appendChildToContainer(container, child) {
				container.children = [child];
				//hostConfig.appendChild(container, child);
			}

			insertBefore(parent, child, beforeChild) {
				try {
					const childIdx = parent.children.indexOf(beforeChild);

					if (childIdx == -1) throw new Error(`childIdx == -1`);
					if (childIdx == 0) parent.children.unshift(child);
					else parent.children.splice(childIdx - 1, 0, child);
				} catch (error) {
					console.error(`Failed to insertBefore`, error);
				}
			}

			insertInContainerBefore(container, child, beforeChild) {
				this.insertBefore(container, child, beforeChild);
			}

			removeChild(parent, child) {
				try {
					const childIdx = parent.children.indexOf(child);

					if (childIdx == -1) throw new Error(`Attempt to remove non existant child`);

					parent.children.splice(childIdx, 1);
				} catch (error) {
					console.error(`Failed to remove child`, error);
				}
			}

			detachDeletedInstance(instance: Instance) {
				try {
					for (const [k, v] of Object.entries(instance.props)) {
						if (k.startsWith('on')) {
							bus!.unsubscribe(v);
						}
					}
				} catch (error) {
					console.error('failed to detachDeletedInstance', error);
				}
			}

			removeChildFromContainer = (container, child) => {
				this.removeChild(container, child);
			}

			resetTextContent(instance) {
			}

			commitTextUpdate(textInstance, prevText, nextText) {}

			commitMount(instance, type, props, internalHandle) {}

			commitUpdate(instance, type, prevProps, nextProps, internalHandle) {
				try {
					const { children, ...rest } = nextProps;

					for (const [k, v] of Object.entries(instance.props)) {
						if (k.startsWith('on')) {
							rest[k] = instance.props[k];
						}
					}

					instance.props = rest;
				} catch (error) {
					console.error(`failed to commitUpdate`, error);
				}
			}

			hideInstance(instance) {}

			unhideInstance(instance, props) {}

			unhideTextInstance(textInstance, text) {}

			clearContainer(container) {
				container.children = [];
			}

			maySuspendCommit(type, props) {
			}

			preloadInstance(type, props) {}

			startSuspendingCommit() {}

			suspendInstance(type, props) {}

			waitForCommitToBeReady() { return null }

};

export class Renderer {
 	renderer: any
	container: Container
	ctx: HostContext
	hostConfig: HostConfig

	constructor() {
		this.container = { children: [], _root: null };
		this.ctx = {
			afterCommitCallbacks: []
		};
		this.hostConfig = new HostConfig(this.ctx);

		this.renderer = Reconciler(this.hostConfig);
	}

	afterCommit(cb: RenderCallback) {
		this.ctx.afterCommitCallbacks.push(cb);
	}

	render(element: ReactElement, cb: (tree: InstanceType[]) => void) {
		if (!this.container._root) {
			const root = this.renderer.createContainer(this.container, false, false);

			root.onUncaughtError = (error: unknown) => { console.error(error); }

			this.container._root = root;
		}

		this.renderer.updateContainer(element, this.container._root, null, () => {
			try {
			cb(this.container.children)
			this.hostConfig.cb = () => cb(this.container.children);
			} catch (error) {
				console.error(`update failed`, error);
			}
		});
	}
};
