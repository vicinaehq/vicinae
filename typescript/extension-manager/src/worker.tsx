import { parentPort, workerData } from "worker_threads";
import { createRenderer } from "./reconciler";
import { LaunchType, NavigationProvider, bus, environment } from "@vicinae/api";
import { ComponentType, ReactNode, Suspense } from "react";
import * as React from "react";
import { patchRequire } from "./patch-require";

class ErrorBoundary extends React.Component<
	{ children: ReactNode },
	{ error: string }
> {
	constructor(props: { children: ReactNode }) {
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

const loadEnviron = () => {
	const {
		supportPath,
		commandName,
		assetsPath,
		commandMode,
		extensionName,
		ownerOrAuthorName,
		vicinaeVersion,
		isRaycast,
	} = workerData;

	environment.textSize = "medium";
	environment.appearance = "dark";
	environment.canAccess = (api) => false;
	environment.isDevelopment = process.env.NODE_ENV === "development";
	environment.commandName = commandName;
	environment.commandMode = commandMode;
	environment.supportPath = supportPath;
	environment.assetsPath = assetsPath;
	environment.raycastVersion = "1.0.0"; // provided for compatibility only, not meaningful
	environment.launchType = LaunchType.UserInitiated;
	environment.extensionName = extensionName;
	environment.ownerOrAuthorName = ownerOrAuthorName;
	environment.vicinaeVersion = vicinaeVersion;
	environment.isRaycast = isRaycast;
};

const loadView = async () => {
	const module = await import(workerData.entrypoint);
	const Component = module.default.default;

	let lastRender = performance.now();

	const renderer = createRenderer({
		onInitialRender: (views) => {
			bus.turboRequest("ui.render", { json: JSON.stringify({ views }) });
		},
		onUpdate: (views) => {
			const now = performance.now();
			lastRender = now;
			bus.turboRequest("ui.render", { json: JSON.stringify({ views }) });
		},
	});

	renderer.render(
		<App launchProps={workerData.launchProps} component={Component} />,
	);
};

const loadNoView = async () => {
	const module = await import(workerData.entrypoint);
	const entrypoint = module.default.default;

	if (typeof entrypoint !== "function") {
		throw new Error(
			`no-view command does not export a function as its default export`,
		);
	}

	await entrypoint(workerData.launchProps);
};

export const main = async () => {
	if (!parentPort) {
		console.error(
			`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`,
		);
		return;
	}

	loadEnviron();
	patchRequire();

	(process as any).noDeprecation = !environment.isDevelopment;

	if (environment.commandMode === "view") {
		await loadView();
	} else if (environment.commandMode === "no-view") {
		await loadNoView();
	}

	bus.emit("exit", {});
};
