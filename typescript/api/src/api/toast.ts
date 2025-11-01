import { randomBytes } from "crypto";
import { bus } from "./bus";
import { Keyboard } from "./keyboard";
import * as ui from "./proto/ui";

/**
 * A Toast with a certain style, title, and message.
 *
 * @example
 * ```typescript
 * import { showToast, Toast } from "@raycast/api";
 * import { setTimeout } from "timers/promises";
 *
 * export default async () => {
 *   const toast = await showToast({ style: Toast.Style.Animated, title: "Uploading image" });
 *
 *   await setTimeout(1000);
 *
 *   toast.style = Toast.Style.Success;
 *   toast.title = "Uploaded image";
 * };
 * ```
 */

export class Toast {
	private options: {
		title: string;
		style: Toast.Style;
		message?: string;
		primaryAction?: Toast.ActionOptions;
		secondaryAction?: Toast.ActionOptions;
	};
	private callbacks: {
		primary?: string;
		secondary?: string;
	} = {};
	private id: string;

	private styleMap: Record<Toast.Style, ui.ToastStyle> = {
		[Toast.Style.Success]: ui.ToastStyle.Success,
		[Toast.Style.Failure]: ui.ToastStyle.Error,
		[Toast.Style.Animated]: ui.ToastStyle.Dynamic,
	};

	/**
	 * Deprecated - Use `showToast` instead
	 */

	constructor(props: Toast.Options) {
		this.id = `toast_${randomBytes(16).toString("hex")}`;
		this.options = {
			title: props.title,
			style: props.style ?? Toast.Style.Success,
			message: props.message,
		};

		if (props.primaryAction) {
			const { onAction } = props.primaryAction;
			const { id } = bus.addEventHandler(() => onAction(this));

			this.options.primaryAction = props.primaryAction;
			this.callbacks.primary = id;
		}

		if (props.secondaryAction) {
			const { onAction } = props.secondaryAction;
			const { id } = bus.addEventHandler(() => onAction(this));

			this.options.secondaryAction = props.secondaryAction;
			this.callbacks.secondary = id;
		}
	}

	/**
	 * The style of a Toast.
	 */
	get style(): Toast.Style {
		return this.options.style;
	}
	set style(style: Toast.Style) {
		this.options.style = style;
		this.update();
	}
	/**
	 * The title of a Toast. Displayed on the top.
	 */
	get title(): string {
		return this.options.title;
	}
	set title(title: string) {
		this.options.title = title;
		this.update();
	}

	/**
	 * An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.
	 */
	get message(): string | undefined {
		return this.options.message;
	}
	set message(message: string | undefined) {
		this.options.message = message;
	}
	/**
	 * The primary Action the user can take when hovering on the Toast.
	 */
	get primaryAction(): Toast.ActionOptions | undefined {
		return this.options.primaryAction;
	}
	set primaryAction(action: Toast.ActionOptions | undefined) {
		this.options.primaryAction = action;
	}
	/**
	 * The secondary Action the user can take when hovering on the Toast.
	 */
	get secondaryAction(): Toast.ActionOptions | undefined {
		return this.options.secondaryAction;
	}
	set secondaryAction(action: Toast.ActionOptions | undefined) {
		this.options.secondaryAction = action;
	}

	async update() {
		await bus.turboRequest("ui.showToast", {
			id: this.id,
			title: this.title,
			style: this.styleMap[this.style],
		});
	}

	/**
	 * Shows the Toast.
	 *
	 * @returns A Promise that resolves when the toast is shown.
	 */
	async show(): Promise<void> {
		const payload: SerializedShowToastPayload = {
			title: this.options.title,
			message: this.options.message,
			style: this.options.style,
		};

		if (this.options.primaryAction && this.callbacks.primary) {
			const { title, shortcut } = this.options.primaryAction;

			payload.primaryAction = {
				title,
				shortcut,
				onAction: this.callbacks.primary,
			};
		}

		if (this.options.secondaryAction && this.callbacks.secondary) {
			const { title, shortcut } = this.options.secondaryAction;

			payload.secondaryAction = {
				title,
				shortcut,
				onAction: this.callbacks.secondary,
			};
		}

		await bus.turboRequest("ui.showToast", {
			id: this.id,
			title: payload.title,
			style: this.styleMap[payload.style ?? Toast.Style.Success],
		});
	}

	/**
	 * Hides the Toast.
	 *
	 * @returns A Promise that resolves when toast is hidden.
	 */
	async hide(): Promise<void> {
		await bus.turboRequest("ui.hideToast", { id: this.id });
	}
}

export namespace Toast {
	/**
	 * The options to create a {@link Toast}.
	 *
	 * @example
	 * ```typescript
	 * import { showToast, Toast } from "@raycast/api";
	 *
	 * export default async () => {
	 *   const options: Toast.Options = {
	 *     style: Toast.Style.Success,
	 *     title: "Finished cooking",
	 *     message: "Delicious pasta for lunch",
	 *     primaryAction: {
	 *       title: 'Do something',
	 *       onAction: () => {
	 *         console.log("The toast action has been triggered")
	 *       }
	 *     }
	 *   };
	 *   await showToast(options);
	 * };
	 * ```
	 */
	export interface Options {
		/**
		 * The title of a Toast. Displayed on the top.
		 */
		title: string;
		/**
		 * An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.
		 */
		message?: string;
		/**
		 * The style of a Toast.
		 */
		style?: Style;
		/**
		 * The primary Action the user can take when hovering on the Toast.
		 */
		primaryAction?: ActionOptions;
		/**
		 * The secondary Action the user can take when hovering on the Toast.
		 */
		secondaryAction?: ActionOptions;
	}
	/**
	 * The options to create a {@link Toast} Action.
	 */
	export interface ActionOptions {
		/**
		 * The title of the action.
		 */
		title: string;
		/**
		 * The keyboard shortcut for the action.
		 */
		shortcut?: Keyboard.Shortcut;
		/**
		 * A callback called when the action is triggered.
		 */
		onAction: (toast: Toast) => void;
	}
	/**
	 * Defines the visual style of the Toast.
	 *
	 * @remarks
	 * Use {@link Toast.Style.Success} for confirmations and {@link Toast.Style.Failure} for displaying errors.
	 * Use {@link Toast.Style.Animated} when your Toast should be shown until a process is completed.
	 * You can hide it later by using {@link Toast.hide} or update the properties of an existing Toast.
	 */
	export enum Style {
		Success = "success",
		Failure = "failure",
		Animated = "animated",
	}
}

/**
 * @deprecated Use {@link Toast.ActionOptions} instead
 */
export declare interface ToastActionOptions extends Toast.ActionOptions {}

/**
 * @deprecated Use {@link Toast.Options} instead
 */
export declare interface ToastOptions extends Toast.Options {}

/**
 * @deprecated Use {@link Toast.Style} instead
 */
export declare const ToastStyle: typeof Toast.Style;

type SerializedShowToastPayload = {
	title: string;
	message?: string;
	style?: Toast.Style;
	primaryAction?: {
		title: string;
		shortcut?: Keyboard.Shortcut;
		onAction: string;
	};
	secondaryAction?: {
		title: string;
		shortcut?: Keyboard.Shortcut;
		onAction: string;
	};
};

export const showToast = async (
	init: Toast.Style | Toast.Options,
	title = "",
	message?: string,
): Promise<Toast> => {
	const toast =
		typeof init === "string"
			? new Toast({ style: init, message, title })
			: new Toast(init);

	await toast.show();

	return toast;
};
