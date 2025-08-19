import { randomUUID } from 'node:crypto';
import { bus } from './bus';
import { Image } from './image';
import { ConfirmAlertActionStyle, ConfirmAlertRequest } from './proto/ui';
import { unsubscribe } from 'node:diagnostics_channel';

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
		Default = 'default',
		Destructive = 'destructive',
		Cancel = 'cancel'
	}

	export type ActionOptions = {
		title: string;
		style?: ActionStyle;
		onAction?: () => void;
	};
};

const styleMap: Record<Alert.ActionStyle, ConfirmAlertActionStyle> = {
	[Alert.ActionStyle.Default]: ConfirmAlertActionStyle.Default,
	[Alert.ActionStyle.Destructive]: ConfirmAlertActionStyle.Destructive,
	[Alert.ActionStyle.Cancel]: ConfirmAlertActionStyle.Cancel,
};

export const confirmAlert = async (options: Alert.Options): Promise<boolean> => {
	return new Promise<boolean>(async (resolve) => {
		const handle = randomUUID();
		let confirmCallback = () => {}
		let cancelCallback = () => {}

		const { unsubscribe } = bus.subscribe(handle, (...args) => {
			callback(!!args[0]);
		});

		const callback = (value: boolean) => {
			if (value) confirmCallback();
			else cancelCallback();

			unsubscribe();
			resolve(value);
		}

		const req = ConfirmAlertRequest.create({
			handle,
			title: options.title,
			description: options.message ?? "Are you sure?",
			rememberUserChoice: false,
			primaryAction: {
				title: options.primaryAction?.title ?? 'Confirm',
				style: styleMap[options.primaryAction?.style ?? Alert.ActionStyle.Default],
			},
			dismissAction: {
				title: options.dismissAction?.title ?? 'Cancel',
				style: styleMap[options.dismissAction?.style ?? Alert.ActionStyle.Cancel],
			}
		});

		if (options.primaryAction?.onAction) {
			confirmCallback = options.primaryAction.onAction;
		}

		if (options.dismissAction?.onAction) {
			cancelCallback = options.dismissAction.onAction;
		}

		await bus.turboRequest('ui.confirmAlert', req);
	});
}
