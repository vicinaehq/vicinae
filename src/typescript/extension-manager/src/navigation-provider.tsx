import { type ReactNode, useEffect, useRef, useState } from "react";
import { getNavigationContext } from "@vicinae/api";
import { globalState } from "./globals";

const View: React.FC<{ children: ReactNode }> = ({ children }) => {
	return <view>{children}</view>;
};

export const NavigationProvider: React.FC<{ root: ReactNode }> = ({ root }) => {
	const [navStack, setNavStack] = useState<ReactNode[]>([root]);
	const pendingShutdown = useRef<boolean>(false);
	const NavigationContext = getNavigationContext();

	const pop = async () => {
		if (pendingShutdown.current) return;
		await globalState.client.UI.popView();
	};

	const push = async (node: ReactNode) => {
		if (pendingShutdown.current) return;
		await globalState.client.UI.pushView();
		setNavStack((cur) => [...cur, node]);
	};

	useEffect(() => {
		if (pendingShutdown.current && navStack.length === 0) {
			process.exit(0);
		}
	}, [navStack]);

	useEffect(() => {
		const listener = globalState.client.UI.viewPoped(() => {
			setNavStack((cur) => cur.slice(0, -1));
		});

		return () => {
			listener.unsubscribe();
		};
	}, []);

	return (
		<NavigationContext.Provider
			value={{
				push,
				pop,
			}}
		>
			{navStack.map((el, idx) => (
				<View key={idx}>{el}</View>
			))}
		</NavigationContext.Provider>
	);
};
