import { createRenderer, type ViewData } from "../reconciler";
import { type ComponentType, Suspense } from "react";
import * as React from "react";
import { NavigationProvider } from "../navigation-provider";
import type * as extensionServer from "../proto/extension-manager";
import { globalState } from "../globals";
import { pathToFileURL } from "node:url";

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
	return (
		<ErrorBoundary>
			<Suspense fallback={null}>
				<NavigationProvider root={<Component {...launchProps} />} />
			</Suspense>
		</ErrorBoundary>
	);
};

export default async function (data: extensionServer.LaunchEventData) {
	const module = await import(pathToFileURL(data.entrypoint).href);
	const Component = module.default.default;
	const sendRender = (views: ViewData[]) => {
		globalState.client.UI.render(JSON.stringify({ views }));
	};
	const renderer = createRenderer({
		onInitialRender: sendRender,
		onUpdate: sendRender,
	});
	globalState.renderer = renderer;

	renderer.render(
		<App
			launchProps={{
				arguments: data.argumentValues,
				launchContext: data.launch_context,
			}}
			component={Component}
		/>,
	);
}
