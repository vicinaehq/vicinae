import { client } from "./client";
import type { Image } from "./image";
import type * as api from "./proto/api";

/**
 * @category Alert
 */
export namespace Alert {
	export type Options = {
		title: string;
		dismissAction?: ActionOptions;
		icon?: Image.ImageLike;
		message?: string;
		primaryAction?: ActionOptions;
		rememberUserChoice?: boolean;
	};

	export enum ActionStyle {
		Default = "default",
		Destructive = "destructive",
		Cancel = "cancel",
	}

	export type ActionOptions = {
		title: string;
		style?: ActionStyle;
		onAction?: () => void;
	};
}

const styleMap: Record<Alert.ActionStyle, api.ConfirmAlertActionStyle> = {
	[Alert.ActionStyle.Default]: "Default",
	[Alert.ActionStyle.Destructive]: "Destructive",
	[Alert.ActionStyle.Cancel]: "Cancel",
};

/**
 * Display a confirmation dialog with a cancel and confirm options.
 * The action is canceled if the cancel button is pressed or if the alert is dismissed for any reason.
 *
 * @remarks Calling this function when another alert is currently pending will result in the pending alert
 * to be automatically canceled, replacing it with the new one.
 *
 * @returns - whether the action was confirmed.
 *
 * @example
 * ```typescript
 * if (await confirmAlert({ title: 'Are you sure?', description: 'This can't be undone' })) {
 * 	performDangerousAction();
 * }
 * ```
 *
 * @category Alert
 */
export const confirmAlert = async (
	options: Alert.Options,
): Promise<boolean> => {
	return new Promise<boolean>((resolve) => {
		client.UI.confirmAlert({
			title: options.title,
			description: options.message ?? "Are you sure?",
			//icon: options.icon && serializeProtoImage(options.icon),
			rememberUserChoice: false,
			primaryAction: {
				title: options.primaryAction?.title ?? "Confirm",
				style:
					styleMap[options.primaryAction?.style ?? Alert.ActionStyle.Default],
			},
			dismissAction: {
				title: options.dismissAction?.title ?? "Cancel",
				style:
					styleMap[options.dismissAction?.style ?? Alert.ActionStyle.Cancel],
			},
		}).then((confirmed) => {
			if (confirmed) {
				options.primaryAction?.onAction?.();
			} else {
				options.dismissAction?.onAction?.();
			}

			resolve(confirmed);
		});
	});
};
