import { Color, Icon, type ImageLike, List } from "@vicinae/api";
import type { FC, ReactNode } from "react";

/**
 * A full-page message for states that block the command's regular content.
 *
 * @category UI Components
 * @public
 */
export namespace Message {
	export type Status = "info" | "error";

	export type Props = {
		title: string;
		description?: string;
		navigationTitle?: string;
		status?: Status;
		icon?: ImageLike;
		actions?: ReactNode;
	};
}

export const Message: FC<Message.Props> = ({
	actions,
	icon,
	status = "info",
	navigationTitle,
	...emptyViewProps
}) => {
	const defaultIcon: ImageLike = {
		source: status === "error" ? Icon.XMarkCircle : Icon.Info01,
		tintColor: status === "error" ? Color.Red : Color.PrimaryText,
	};

	return (
		<List navigationTitle={navigationTitle} filtering={false}>
			<List.EmptyView
				{...emptyViewProps}
				icon={icon ?? defaultIcon}
				actions={actions}
			/>
		</List>
	);
};
