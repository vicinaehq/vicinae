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

const useScreens = () => {
	const [loading, setLoading] = useState(true);
	const [screens, setScreens] = useState<wm.Screen[]>([]);
	const [error, setError] = useState<Error | null>(null);

	const refreshScreens = () => {
		setLoading(true);
		wm.getScreens()
			.then(setScreens)
			.catch(setError)
			.finally(() => setLoading(false));
	};

	useEffect(() => {
		refreshScreens();
	}, []);

	return { screens, loading, error };
};

const useWindowList = () => {
	const [loading, setLoading] = useState(true);
	const [windows, setWindows] = useState<wm.Window[]>([]);
	const [error, setError] = useState<Error | null>(null);

	const refreshWindows = () => {
		setLoading(true);
		wm.getWindows()
			.then(setWindows)
			.catch(setError)
			.finally(() => setLoading(false));
	};

	useEffect(() => {
		refreshWindows();
		const interval = setInterval(refreshWindows, REFRESH_INTERVAL_MS);
		return () => {
			clearInterval(interval);
		};
	}, []);

	return { windows, loading, error };
};

const ScreenInfo = ({ screen }: { screen: wm.Screen }) => {
	const wrapCode = (s: string) => {
		return `\`\`\`\n${s}\n\`\`\``;
	};

	return <Detail markdown={wrapCode(JSON.stringify(screen, null, 2))} />;
};

const ScreenListItem = ({ screen }: { screen: wm.Screen }) => {
	return (
		<List.Item
			title={screen.name}
			subtitle={`${screen.model} - ${screen.model}`}
			actions={
				<ActionPanel>
					<Action.Push
						title="Show screen info"
						target={<ScreenInfo screen={screen} />}
					/>
				</ActionPanel>
			}
		/>
	);
};

export default function WindowSwitcher() {
	const {
		windows,
		loading: windowLoading,
		error: windowError,
	} = useWindowList();
	const { screens, loading: screenLoading, error: screenError } = useScreens();
	const loading = windowLoading || screenLoading;
	const error = windowError || screenError;

	if (error) {
		return (
			<Detail
				markdown={`An error occured while fetching windows:\n\`\`\`\n${error}\n\`\`\``}
			/>
		);
	}

	return (
		<List isLoading={loading} searchBarPlaceholder="Search windows">
			<List.Section title={"Screens"}>
				{screens.map((sc) => (
					<ScreenListItem key={sc.name} screen={sc} />
				))}
			</List.Section>
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
								<Action title="Focus window" onAction={() => process.exit(0)} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}
