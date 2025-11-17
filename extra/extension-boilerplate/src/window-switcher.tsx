import {
	WindowManagement as wm,
	List,
	Action,
	ActionPanel,
} from "@vicinae/api";
import { useEffect, useState } from "react";

export const useWindows = () => {
	const [windows, setWindows] = useState<wm.Window[]>([]);

	const refreshWindows = () => {
		wm.getWindows().then(setWindows);
	};

	useEffect(() => {
		refreshWindows();
	}, []);

	return { windows };
};

export default function WindowSwitcher() {
	const { windows } = useWindows();

	return (
		<List>
			{windows.map((win) => (
				<List.Item
					title={win.id}
					actions={
						<ActionPanel>
							<Action title="Focus window" onAction={() => win.focus()} />
						</ActionPanel>
					}
				/>
			))}
		</List>
	);
}
