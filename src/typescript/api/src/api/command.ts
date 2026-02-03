import { bus } from "./bus";

/**
 * Update the values of properties declared in the manifest of the current command.
 * Currently only `subtitle` is supported. Pass `null` to clear the custom subtitle.
 *
 * Raycast API: https://developers.raycast.com/api-reference/command#updatecommandmetadata
 */
export async function updateCommandMetadata(metadata: {
	subtitle?: string | null;
}): Promise<void> {
	const payload: { subtitle?: string | undefined } = {};
	if (Object.prototype.hasOwnProperty.call(metadata, "subtitle")) {
		payload.subtitle = metadata.subtitle ?? undefined;
	}

	await bus.request("command.updateCommandMetadata", payload);
}
