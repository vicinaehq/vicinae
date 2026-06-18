import {
	Action,
	ActionPanel,
	Grid,
	Icon,
	type SetWallpaperOptions,
	setWallpaper,
	showToast,
	Toast,
	WindowManagement,
} from "@vicinae/api";
import { readdir } from "node:fs/promises";
import { homedir } from "node:os";
import { basename, extname, join } from "node:path";
import { useEffect, useState } from "react";

const IMAGE_EXTENSIONS = new Set([".jpg", ".jpeg", ".png", ".webp", ".bmp", ".gif"]);
const FIT_MODES = ["Cover", "Contain", "Stretch", "Center", "Tile"] as const;

async function findWallpapers(): Promise<string[]> {
	const dir = join(homedir(), "Pictures");

	try {
		const entries = await readdir(dir, { recursive: true, withFileTypes: true });

		return entries
			.filter((e) => e.isFile() && IMAGE_EXTENSIONS.has(extname(e.name).toLowerCase()))
			.map((e) => join(e.parentPath, e.name))
			.sort((a, b) => a.localeCompare(b));
	} catch {
		return [];
	}
}

export default function WallpaperGrid() {
	const [paths, setPaths] = useState<string[]>([]);
	const [screens, setScreens] = useState<WindowManagement.Screen[]>([]);
	const [loading, setLoading] = useState(true);

	useEffect(() => {
		findWallpapers().then((found) => {
			setPaths(found);
			setLoading(false);
		});
		WindowManagement.getScreens()
			.then(setScreens)
			.catch(() => setScreens([]));
	}, []);

	return (
		<Grid
			columns={4}
			fit={Grid.Fit.Fill}
			aspectRatio="16/9"
			isLoading={loading}
			searchBarPlaceholder="Search wallpapers in ~/Pictures..."
		>
			{paths.map((path) => (
				<Grid.Item
					key={path}
					content={{ source: `file://${path}` }}
					title={basename(path)}
					actions={<WallpaperActions path={path} screens={screens} />}
				/>
			))}
		</Grid>
	);
}

function WallpaperActions({
	path,
	screens,
}: {
	path: string;
	screens: WindowManagement.Screen[];
}) {
	const apply = async (summary: string, options: SetWallpaperOptions = {}) => {
		try {
			await setWallpaper(path, options);
			await showToast({ style: Toast.Style.Success, title: "Wallpaper set", message: summary });
		} catch (error) {
			await showToast({
				style: Toast.Style.Failure,
				title: "Could not set wallpaper",
				message: String(error),
			});
		}
	};

	return (
		<ActionPanel>
			{/* Default action: set with no options (Cover, all screens, not persisted). */}
			<Action title="Set Wallpaper" icon={Icon.Image} onAction={() => apply(basename(path))} />

			<Action
				title="Set & Persist"
				icon={Icon.Pin}
				onAction={() => apply("persisted across restarts", { persist: true })}
			/>

			<ActionPanel.Submenu title="Set with Fit" icon={Icon.Crop}>
				{FIT_MODES.map((fit) => (
					<Action key={fit} title={fit} onAction={() => apply(`fit: ${fit}`, { fit })} />
				))}
			</ActionPanel.Submenu>

			{screens.length > 0 && (
				<ActionPanel.Submenu title="Set on Screen" icon={Icon.Monitor}>
					{screens.map((screen) => (
						<Action
							key={screen.name}
							title={screen.name}
							onAction={() => apply(`screen: ${screen.name}`, { screen: screen.name })}
						/>
					))}
				</ActionPanel.Submenu>
			)}

			<Action.CopyToClipboard title="Copy Path" content={path} />
		</ActionPanel>
	);
}
