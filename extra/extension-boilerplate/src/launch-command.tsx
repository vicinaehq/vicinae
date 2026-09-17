import {
	Action,
	ActionPanel,
	launchCommand,
	LaunchType,
	List,
} from "@vicinae/api";

const intraCommands: any[] = [
	{ name: "window-switcher", context: {}, arguments: {} },
	{
		name: "context-echo",
		context: {
			myString: "SomeString",
			myNumber: 3,
			someArray: [1, 2, true, { key: 1, arr: [1, 2, 42] }],
		},
		arguments: { key: "value" },
	},
];
const interCommands = [
	{
		extensionName: "wallhaven",
		name: "downloaded-wallpapers",
		author: "aurelleb",
		context: {},
		arguments: {},
	},
];

export default function LaunchIntra() {
	return (
		<List>
			<List.Section title="Intra launch">
				{intraCommands.map((cmd) => (
					<List.Item
						key={cmd.name}
						title={cmd.name}
						actions={
							<ActionPanel>
								<Action
									title="Launch command"
									onAction={() => {
										launchCommand({
											name: cmd.name,
											type: LaunchType.UserInitiated as any,
											context: cmd.context ?? {},
											arguments: cmd.arguments ?? {},
										});
									}}
								/>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
			<List.Section title="Inter launch">
				{interCommands.map((cmd) => (
					<List.Item
						key={cmd.name}
						title={cmd.name}
						actions={
							<ActionPanel>
								<Action
									title="Launch command"
									onAction={() => {
										launchCommand({
											name: cmd.name,
											extensionName: cmd.extensionName,
											ownerOrAuthorName: cmd.author,
											type: LaunchType.UserInitiated as any,
											context: cmd.context ?? {},
											arguments: cmd.arguments ?? {},
										});
									}}
								/>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}
