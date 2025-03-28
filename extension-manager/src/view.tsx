import { workerData } from "worker_threads";
import { createRenderer } from './reconciler';
import { NavigationProvider, bus } from '@omnicast/api';
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

export const renderView = async () => {
	if (!bus) throw new Error(`no parent port, is this view worker operating inside a worker`);

	const module = await import(workerData.component);
	const Component = module.default.default;

	process.on('uncaughtException', (error) => {
		console.error('uncaught exception:', error);
	});

	const renderer = createRenderer({
		onInitialRender: (views) => {
			bus!.emit('render', { views });
		},
		onUpdate: (views, changes) => {
			bus!.emit('render', { views, changes });
		}
	});

	renderer.render(<App component={Component} />);
}
