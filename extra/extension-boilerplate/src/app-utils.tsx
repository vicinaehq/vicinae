import {
	Action,
	ActionPanel,
	Application,
	Color,
	Detail,
	Form,
	Icon,
	List,
	WindowManagement,
	closeMainWindow,
	getDefaultApplication,
	getApplications,
	open,
	runInTerminal,
} from "@vicinae/api";
import { useEffect, useState } from "react";

// Playground to try out app-related stuff (list, open default, run in terminal...)

const OpenInView = ({ target }: { target: string }) => {
	const [apps, setApps] = useState<Application[]>([]);

	useEffect(() => {
		getApplications(target).then(setApps);
	}, []);

	return (
		<List>
			<List.Section title={"Open in..."}>
				{apps.map((app) => (
					<List.Item
						title={app.name}
						icon={app.icon}
						actions={
							<ActionPanel>
								<Action.Open title="Open" app={app} target={target} />
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
};

export default function AppUtils() {
	const [searchText, setSearchText] = useState("");
	const [defaultApp, setDefaultApp] = useState<Application | null>(null);
	const shell = process.env.SHELL ?? "/bin/sh";

	useEffect(() => {
		getDefaultApplication(searchText)
			.then((app) => setDefaultApp(app))
			.catch(() => setDefaultApp(null));
	}, [searchText]);

	return (
		<List searchText={searchText} onSearchTextChange={setSearchText}>
			<List.EmptyView
				title="Start typing"
				icon={Icon.Keyboard}
				description="What do you want to open?"
			/>
			{searchText && (
				<>
					{defaultApp ? (
						<List.Item
							title={defaultApp.name}
							subtitle={"Default opener"}
							icon={defaultApp.icon}
							actions={
								<ActionPanel>
									<Action.Open
										title={`Open in ${defaultApp.name}`}
										icon={defaultApp.icon}
										target={searchText}
									/>
									<Action.Push
										title="Open in..."
										target={<OpenInView target={searchText} />}
									/>
								</ActionPanel>
							}
						/>
					) : (
						<List.Item
							title={"No default app"}
							icon={{ source: Icon.XMarkCircle, tintColor: Color.Red }}
						/>
					)}
					<List.Item
						title="Run in terminal"
						icon={Icon.Terminal}
						subtitle={shell}
						actions={
							<ActionPanel>
								<Action
									title="Run in terminal"
									onAction={() => {
										runInTerminal([shell, "-c", searchText]);
										closeMainWindow();
									}}
								/>
							</ActionPanel>
						}
					/>
					<List.Item
						title="Run in terminal (hold)"
						icon={Icon.Terminal}
						subtitle={shell}
						actions={
							<ActionPanel>
								<Action
									title="Run in terminal (hold)"
									onAction={() => {
										runInTerminal([shell, "-c", searchText], { hold: true });
										closeMainWindow();
									}}
								/>
							</ActionPanel>
						}
					/>
				</>
			)}
		</List>
	);
}
