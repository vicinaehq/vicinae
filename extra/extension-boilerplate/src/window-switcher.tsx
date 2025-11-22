import {
	WindowManagement as wm,
	List,
	Action,
	ActionPanel,
	Detail,
	Icon,
} from "@vicinae/api";
import { useEffect, useState } from "react";

const REFRESH_INTERVAL_MS = 1000;

const useWindowList = () => {
	const [loading, setLoading] = useState(true);
	const [windows, setWindows] = useState<wm.Window[]>([]);
	const [error, setError] = useState<Error | null>(null);

	const refreshWindows = () => {
		console.error(`refreshing windows2`);
		setLoading(true);
		wm.getWindows()
			.then(setWindows)
			.catch(setError)
			.finally(() => setLoading(false));
	};

	useEffect(() => {
		refreshWindows();
		const id = setInterval(refreshWindows, REFRESH_INTERVAL_MS);
		return () => clearInterval(id);
	}, []);

	return { windows, loading, error };
};

export default function WindowSwitcher() {
	const { windows, loading, error } = useWindowList();

	if (error) {
		return (
			<Detail
				markdown={`An error occured while fetching windows:\n\`\`\`\n${error}\n\`\`\``}
			/>
		);
	}

	return (
		<List isLoading={loading} searchBarPlaceholder="Search windows">
			<List.Section title={"Open Windows"}>
				{windows.map((win) => (
					<List.Item
						key={win.id}
						title={win.title}
						subtitle={win.id}
						accessories={
							win.workspaceId ? [{ text: `WS ${win.workspaceId}` }] : []
						}
						icon={win.application?.icon ?? Icon.Window}
						actions={
							<ActionPanel>
								<Action title="Focus window" onAction={() => win.focus()} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}
