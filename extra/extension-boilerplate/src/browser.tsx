import {
	Action,
	ActionPanel,
	BrowserExtension,
	Icon,
	List,
} from "@vicinae/api";
import { useEffect, useState } from "react";

export default function BrowserExt() {
	const [tabs, setTabs] = useState<BrowserExtension.Tab[]>();

	useEffect(() => {
		BrowserExtension.getTabs().then(setTabs);
	}, []);

	return (
		<List searchBarPlaceholder="Search for browser tabs...">
			{tabs?.map((t) => (
				<List.Item
					title={t.title ?? "Unnamed"}
					subtitle={t.url}
					icon={Icon.Tag}
					actions={
						<ActionPanel>
							<Action title="Focus" onAction={() => t.focus()} />
						</ActionPanel>
					}
				/>
			))}
		</List>
	);
}
