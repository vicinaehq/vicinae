import { createRenderer, type Op } from "../reconciler";
import { type ComponentType, Suspense } from "react";
import * as React from "react";
import { NavigationProvider } from "../navigation-provider";
import type * as extensionServer from "../proto/extension-manager";
import { globalState } from "../globals";
import { callbackManager } from "../callback";

class ErrorBoundary extends React.Component<
	{ children: React.ReactNode },
	{ error: string }
> {
	constructor(props: { children: React.ReactNode }) {
		super(props);
		this.state = { error: "" };
	}

	componentDidCatch(error: Error) {
		this.setState({ error: `${error.name}: ${error.message}` });
	}

	render() {
		const { error } = this.state;

		if (error) {
			throw error;
		}

		return <>{this.props.children}</>;
	}
}

const App: React.FC<{ component: ComponentType; launchProps: any }> = ({
	component: Component,
	launchProps,
}) => {
	const [isRunning, setIsRunning] = React.useState(true);

	React.useEffect(() => {
		callbackManager.setHandler("shutdown", () => {
			setIsRunning(false);
		});

		return () => {
			callbackManager.removeHandler("shutdown");
		};
	}, []);

	return (
		<ErrorBoundary>
			<Suspense fallback={null}>
				{isRunning && (
					<NavigationProvider root={<Component {...launchProps} />} />
				)}
			</Suspense>
		</ErrorBoundary>
	);
};

export default async function(data: extensionServer.LaunchEventData) {
	const module = await import(data.entrypoint);
	const Component = module.default.default;

	const sendOps = (ops: Op[]) => {
		globalState.client.UI.applyOps(JSON.stringify(ops));
	};

	const renderer = createRenderer({
		onUpdate: sendOps,
	});
	globalState.renderer = renderer;

	renderer.render(
		<App
			launchProps={{ arguments: data.argumentValues }}
			component={Component}
		/>,
	);
}
