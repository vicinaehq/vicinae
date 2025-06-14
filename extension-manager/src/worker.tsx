import { parentPort, workerData } from "worker_threads";
import { createRenderer } from './reconciler';
import { NavigationProvider, bus, environment } from '@omnicast/api';
import type { ComponentType, ReactNode } from "react";
import * as React from 'react';
import { patchRequire } from "./patch-require";

class ErrorBoundary extends React.Component<{ children: ReactNode }, { error: string }> {
  constructor(props: { children: ReactNode }) {
    super(props);
    this.state = {error: ""};
  }

  componentDidCatch(error: Error) {
    this.setState({error: `${error.name}: ${error.message}`});
  }

  render() {
    const {error} = this.state;

    if (error) {
		console.error(`FUCK THE ERROR! ${error}`);
      return null;
    } else {
      return <>{this.props.children}</>;
    }
  }
}

const App: React.FC<{ component: ComponentType, launchProps: any }> = ({ component: Component, launchProps }) => {
	return (
		<ErrorBoundary>
			<NavigationProvider root={<Component {...launchProps} />} />
		</ErrorBoundary>
	)
}

const loadEnviron = () => {
	process.env.NODE_ENV = 'development';
	environment.textSize = 'medium';
	environment.appearance = 'dark';
	environment.canAccess = (api) => false,
	environment.assetsPath = "";
	environment.isDevelopment = false;
	environment.commandMode = workerData.commandMode;
	environment.supportPath = '/tmp';
	environment.raycastVersion = '1.0.0';
	environment.launchType = {} as any;
}

const loadView = async () => {
	const module = await import(workerData.entrypoint);
	const Component = module.default.default;

	process.on('uncaughtException', (error) => {
		console.error('uncaught exception:', error);
	});

	let lastRender = performance.now();

	const renderer = createRenderer({
		onInitialRender: (views) => {
			bus!.emit('render', { views });
		},
		onUpdate: (views) => {
			const now = performance.now();
			const elapsed =  now - lastRender;

			console.debug(`[PERF] Render update (last update ${elapsed}ms ago)`);

			lastRender = now;
			
			bus!.emit('ui.render', { views });
		}
	});

	renderer.render(<App launchProps={workerData.launchProps} component={Component} />);
}

const loadNoView = async () => {
	const module = await import(workerData.entrypoint);
	const entrypoint = module.default.default;

	if (typeof entrypoint !== 'function') {
		throw new Error(`no-view command does not export a function as its default export`);
	}

	await entrypoint(workerData.launchProps);
}

export const main = async () => {
	if (!parentPort) {
		console.error(`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`)
		return ;
	}

	patchRequire();
	loadEnviron();

	if (environment.commandMode == 'view') {
		await loadView();
	}

	else if (environment.commandMode == 'no-view') {
		await loadNoView();
	}

	bus.emit('exit', {});
}
