import { createRenderer, type ViewData } from "../reconciler";
import type { LaunchEventData } from "../proto/extension";
import { type ComponentType, Suspense } from "react";
import * as React from "react";
import { NavigationProvider, bus } from "@vicinae/api";

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
			bus.emitCrash(error);
			return null;
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
			<Suspense fallback={<></>}>
				<NavigationProvider root={<Component {...launchProps} />} />
			</Suspense>
		</ErrorBoundary>
	);
};

export default async function(data: LaunchEventData) {
	const module = await import(data.entrypoint);
	const Component = module.default.default;
	const sendRender = (views: ViewData[]) => {
		bus.request("ui.render", { json: JSON.stringify({ views }) });
	};
	const renderer = createRenderer({
		onInitialRender: sendRender,
		onUpdate: sendRender,
	});

	renderer.render(
		<App
			launchProps={{ arguments: data.argumentValues }}
			component={Component}
		/>,
	);
}
