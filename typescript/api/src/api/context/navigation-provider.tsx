import { type ReactNode, useEffect, useRef, useState } from "react";
import context from "./navigation-context";
import { bus } from "../bus";

const View: React.FC<{ children: ReactNode }> = ({ children }) => {
	return <view>{children}</view>;
};

/**
 * @ignore
 */
export const NavigationProvider: React.FC<{ root: ReactNode }> = ({ root }) => {
	const [navStack, setNavStack] = useState<ReactNode[]>([root]);
	const pendingShutdown = useRef<boolean>(false);

	const pop = () => {
		if (pendingShutdown.current) return;

		// we ask Vicinae to pop the current view, but we need to wait
		// for the pop-view event to be fired in order to dismount it from
		// our local view stack: otherwise Vicinae might miss a render.
		bus.request("ui.popView", {});
	};

	const push = (node: ReactNode) => {
		if (pendingShutdown.current) return;

		bus.request("ui.pushView", {}).then(() => {
			setNavStack((cur) => [...cur, node]);
		});
	};

	useEffect(() => {
		if (pendingShutdown.current && navStack.length === 0) {
			process.exit(0);
		}
	}, [navStack]);

	useEffect(() => {
		const shutdown = bus.subscribe("shutdown", () => {
			pendingShutdown.current = true;
			setNavStack([]);
		});

		const listener = bus.subscribe("pop-view", () => {
			setNavStack((cur) => cur.slice(0, -1));
		});

		return () => {
			shutdown.unsubscribe();
			listener.unsubscribe();
		};
	}, []);

	return (
		<context.Provider
			value={{
				push,
				pop,
			}}
		>
			{navStack.map((el, idx) => (
				<View key={idx}>{el}</View>
			))}
		</context.Provider>
	);
};
