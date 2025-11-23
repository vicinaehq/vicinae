import { bus } from "./bus";
import { Image, serializeProtoImage } from "./image";
import { ConfirmAlertActionStyle, ConfirmAlertRequest } from "./proto/ui";

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

const styleMap: Record<Alert.ActionStyle, ConfirmAlertActionStyle> = {
	[Alert.ActionStyle.Default]: ConfirmAlertActionStyle.Default,
	[Alert.ActionStyle.Destructive]: ConfirmAlertActionStyle.Destructive,
	[Alert.ActionStyle.Cancel]: ConfirmAlertActionStyle.Cancel,
};

/**
 * @category Alert
 */
export const confirmAlert = async (
	options: Alert.Options,
): Promise<boolean> => {
	return new Promise<boolean>((resolve) => {
		const req = ConfirmAlertRequest.create({
			title: options.title,
			description: options.message ?? "Are you sure?",
			icon: options.icon && serializeProtoImage(options.icon),
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
		});

		bus.request("ui.confirmAlert", req).then((res) => {
			if (!res.ok) return false;

			if (res.value.confirmed) {
				options.primaryAction?.onAction?.();
			} else {
				options.dismissAction?.onAction?.();
			}

			resolve(res.value.confirmed);
		});
	});
};
