import { getClient } from "./client";
import { environment, LaunchType } from "./environment";
import type * as api from "./proto/api";

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

type LaunchOptionsBase = {
	name: string;
	type: LaunchType;
	context?: Record<string, string>;
	arguments?: Record<string, string>;
	fallbackText?: string;
};

export type IntraExtensionLaunchOptions = LaunchOptionsBase;

export type InterExtensionLaunchOptions = LaunchOptionsBase & {
	extensionName: string;
	ownerOrAuthorName: string;
};

export type LaunchOptions =
	| InterExtensionLaunchOptions
	| IntraExtensionLaunchOptions;

/**
 * Launch another command from the current extension or a different one.
 *
 * The `context` object can be used to pass arbitrary JSON stringifiable data
 * to the other command. Note that buffer or date objects NEED to be encoded/stringified before
 * passing them.
 *
 * @returns Returns when the other command has been successfully launched. Note that control is fully transferred
 * to the other command and that the current command from which `launchCommand` has been called is effectively unloaded
 * if the call succeeds.
 *
 * @throws If the command doesn't exist or couldn't be started, for any reason.
 */
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

	const transformLaunchType = (type: LaunchType): api.LaunchType => {
		if (type === LaunchType.UserInitiated) return "User";
		if (type === LaunchType.Background) return "Background";
		return "CommandLine";
	};

	await getClient().Command.launchCommand({
		extensionName: extensionName,
		ownerOrAuthorName: ownerOrAuthorName,
		name: options.name,
		arguments: options.arguments,
		context: options.context,
		type: transformLaunchType(options.type),
	});
}
