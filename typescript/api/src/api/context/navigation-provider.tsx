import React, { ReactNode, useEffect, useState } from "react";
import context from "./navigation-context";
import { bus } from "../bus";

const View: React.FC<{ children: ReactNode }> = ({ children }) => {
	return <view>{children}</view>;
};

export const NavigationProvider: React.FC<{ root: ReactNode }> = ({ root }) => {
	const [navStack, setNavStack] = useState<ReactNode[]>([root]);

	const pop = () => {
		// we ask Vicinae to pop the current view, but we need to wait
		// for the pop-view event to be fired in order to dismount it from
		// our local view stack: otherwise Vicinae might miss a render.
		bus.turboRequest("ui.popView", {});
	};

	const push = (node: ReactNode) => {
		bus.turboRequest("ui.pushView", {}).then(() => {
			setNavStack((cur) => [...cur, node]);
		});
	};

	useEffect(() => {
		const listener = bus.subscribe("pop-view", () => {
			setNavStack((cur) => cur.slice(0, -1));
		});

		return () => listener.unsubscribe();
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
