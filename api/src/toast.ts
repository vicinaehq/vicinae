import { bus } from "./bus";
import { Keyboard } from "./keyboard";

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
export declare class Toast {
    private options;
    private id;
    private callbacks;
    /**
     * Deprecated - Use `showToast` instead
     */
    constructor(props: Toast.Options);
    /**
     * The style of a Toast.
     */
    get style(): Toast.Style;
    set style(style: Toast.Style);
    /**
     * The title of a Toast. Displayed on the top.
     */
    get title(): string;
    set title(title: string);
    /**
     * An additional message for the Toast. Useful to show more information, e.g. an identifier of a newly created asset.
     */
    get message(): string | undefined;
    set message(message: string | undefined);
    /**
     * The primary Action the user can take when hovering on the Toast.
     */
    get primaryAction(): Toast.ActionOptions | undefined;
    set primaryAction(action: Toast.ActionOptions | undefined);
    /**
     * The secondary Action the user can take when hovering on the Toast.
     */
    get secondaryAction(): Toast.ActionOptions | undefined;
    set secondaryAction(action: Toast.ActionOptions | undefined);
    /**
     * Shows the Toast.
     *
     * @returns A Promise that resolves when the toast is shown.
     */
    show(): Promise<void>;
    /**
     * Hides the Toast.
     *
     * @returns A Promise that resolves when toast is hidden.
     */
    hide(): Promise<void>;
    private update;
}

export declare namespace Toast {
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
        Success = "SUCCESS",
        Failure = "FAILURE",
        Animated = "ANIMATED"
    }
}

/**
 * @deprecated Use {@link Toast.ActionOptions} instead
 */
export declare interface ToastActionOptions extends Toast.ActionOptions {
}

/**
 * @deprecated Use {@link Toast.Options} instead
 */
export declare interface ToastOptions extends Toast.Options {
}

/**
 * @deprecated Use {@link Toast.Style} instead
 */
export declare const ToastStyle: typeof Toast.Style;

/*
export const showToast = (options: Toast.Options): Promise<Toast> => {
	return {} as any;
}
*/

export const showToast = async (init: Toast.Style | Toast.Options, title?: string, message?: string): Promise<Toast> => {
	// we are dealing with the style (preferred) overload
	if (typeof init === "string") {
	} else {
		const opts = init as Toast.Options;
	}

	const toast = await bus?.request("show-toast", {});

	return {} as any;
};

