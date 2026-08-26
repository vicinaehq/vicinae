import React from "react";
import {
	AI,
	BrowserExtension,
	Color,
	environment,
	FileSearch,
	Icon,
	ImageLike,
	List,
	Wallpaper,
	WindowManagement,
} from "@vicinae/api";

export default function Capabilities() {
	const supportedIcon = (b: boolean): ImageLike =>
		b
			? { source: Icon.CheckCircle, tintColor: Color.Green }
			: { source: Icon.XMarkCircle, tintColor: Color.Red };

	return (
		<List searchBarPlaceholder={"Search capabilities"}>
			<List.Item
				title="File Search"
				icon={supportedIcon(environment.canAccess(FileSearch))}
			/>
			<List.Item
				title="Window Management"
				icon={supportedIcon(environment.canAccess(WindowManagement))}
			/>
			<List.Item
				title="Wallpaper"
				icon={supportedIcon(environment.canAccess(Wallpaper))}
			/>
			<List.Item
				title="Browser Extension"
				icon={supportedIcon(environment.canAccess(BrowserExtension))}
			/>
			<List.Item title="AI" icon={supportedIcon(environment.canAccess(AI))} />
		</List>
	);
}
