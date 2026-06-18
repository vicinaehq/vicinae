import { getClient } from "./client";
import type { WallpaperFit } from "./proto/api";

export namespace Wallpaper {
	export type Fit = WallpaperFit;

	export type SetOptions = {
		/**
		 * How to fit the wallpaper on the screen. Some fits may not be supported by the active
		 * backend and will silently degrade to the closest one.
		 * @default `Cover`
		 */
		fit?: WallpaperFit;

		/**
		 * Name of the screen to apply the wallpaper to (see {@link WindowManagement.getScreens}).
		 * Ignored by backends that cannot set a different wallpaper per screen.
		 */
		screen?: string;
	};

	/**
	 * Set the desktop wallpaper to the image at `path`, using whichever backend fits the current
	 * desktop environment or running wallpaper daemon.
	 *
	 * @throws if no backend is available to serve the request
	 */
	export async function set(
		path: string,
		options: SetOptions = {},
	): Promise<void> {
		await getClient().Wallpaper.set(path, options);
	}
}
