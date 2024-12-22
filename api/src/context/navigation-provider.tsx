import React, { ReactNode, useEffect, useState } from 'react';
import context from './navigation-context';
import { bus } from '../bus';

export const NavigationProvider: React.FC<{ root: ReactNode }>= ({ root }) => {
	const [navStack, setNavStack] = useState<ReactNode[]>([root]);

	const pop = () => {
		bus!.request('pop-view', {}).then(() => {
			setNavStack((cur) => cur.slice(0, -1));
		});
	}

	const push = (node: ReactNode) => {
		bus!.request('push-view', {}).then(() => {
			setNavStack((cur) => [...cur, node]);
		});
	}

	useEffect(() => {
		const listener = bus!.subscribe('pop-view', () => {
			setNavStack((cur) => cur.slice(0, -1));
		});

		return () => listener.unsubscribe();
	}, []);
	
	const currentView = navStack.at(-1) ?? null;

	return (
		<context.Provider
			value={{
				push,
				pop
			}}
		>
			{currentView}
		</context.Provider>
	);
} 
