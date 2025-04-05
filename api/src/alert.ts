import { bus } from './bus';
import { Image } from './image';

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

export const confirmAlert = async (options: Alert.Options): Promise<boolean> => {
	const res = await bus.request<{ value: boolean }>('confirm-alert', {});

	return res.data.value;
}
