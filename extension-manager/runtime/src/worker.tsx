import { parentPort, workerData } from "worker_threads";
import { createRenderer } from './reconciler';
import { NavigationProvider, bus, environment } from '@omnicast/api';
import React, { ComponentType, ReactNode } from "react";

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

const App: React.FC<{ component: ComponentType }> = ({ component: Component }) => {
	return (
		<ErrorBoundary>
			<NavigationProvider root={<Component />} />
		</ErrorBoundary>
	)
}

const loadEnviron = () => {
	environment.textSize = 'medium';
	environment.appearance = 'dark';
	environment.canAccess = (api) => false,
	environment.assetsPath = "";
	environment.isDevelopment = false;
	environment.commandMode = 'view';
	environment.supportPath = '/tmp';
	environment.raycastVersion = '1.0.0';
	environment.launchType = {} as any;
}

export const main = async () => {
	if (!parentPort) {
		console.error(`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`)
		return ;
	}

	loadEnviron();

	const module = await import(workerData.component);
	const Component = module.default.default;

	process.on('uncaughtException', (error) => {
		console.error('uncaught exception:', error);
	});

	const renderer = createRenderer({
		onInitialRender: (views) => {
			bus!.emit('render', { views });
		},
		onUpdate: (views) => {
			console.log('[DEBUG] render');
			bus!.emit('render', { views });
		}
	});

	renderer.render(<App component={Component} />);
}

main();
