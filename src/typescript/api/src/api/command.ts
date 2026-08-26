import { getClient } from "./client";
import { environment } from "./environment";
import type { LaunchType } from "./proto/api";

/**
 * Update the values of properties declared in the manifest of the current command.
 * Currently only `subtitle` is supported. Pass `null` to clear the custom subtitle.
 *
 * Raycast API: https://developers.raycast.com/api-reference/command#updatecommandmetadata
 */
export async function updateCommandMetadata(metadata: {
	subtitle?: string | null;
}): Promise<void> {
	await getClient().Command.updateCommandMetadata({
		subtitle: metadata.subtitle ?? undefined,
	});
}

export type IntraExtensionLaunchOptions = {
	name: string;
	type: LaunchType;
	arguments?: Record<string, string>;
	context?: Record<string, string>;
	fallbackText?: string;
};

export type InterExtensionLaunchOptions = {
	extensionName: string;
	name: string;
	ownerOrAuthorName: string;
	type: LaunchType;
	arguments?: Record<string, string>;
	context?: Record<string, string>;
	fallbackText?: string;
};

export type LaunchOptions =
	| InterExtensionLaunchOptions
	| IntraExtensionLaunchOptions;

export async function launchCommand(options: LaunchOptions) {
	const isInter = (t: LaunchOptions): t is InterExtensionLaunchOptions => {
		return Object.hasOwn(t, "extensionName");
	};
	const extensionName = isInter(options)
		? options.extensionName
		: environment.extensionName;
	const ownerOrAuthorName = isInter(options)
		? options.ownerOrAuthorName
		: environment.ownerOrAuthorName;

	await getClient().Command.launchCommand({
		extensionName: extensionName,
		ownerOrAuthorName: ownerOrAuthorName,
		name: options.name,
		arguments: options.arguments,
		type: options.type,
		context: options.context,
	});
}
