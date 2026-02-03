import type { ReactNode } from "react";
import { Metadata } from "./metadata";

/**
 * ![](../../../assets/detail.png)
 *
 * A Detail component is commonly used to render markdown with an optional metadata side panel on the right.
 *
 * @remarks
 * The markdown renderer is currently very bare bones and does not have good support for most fancy markdown features. This is actively being worked on.
 *
 * @example
 * ```typescript
 * import { Detail } from '@vicinae/api';
 *
 * export default function MyCommand() {
 *  return <Detail markdown={'# Hello world!'} />;
 * }
 * ```
 *
 * @category UI Components
 * @public
 */
export namespace Detail {
	export type Props = {
		navigationTitle?: string;
		metadata?: ReactNode;
		markdown: string;
		actions?: ReactNode;
	};
}

const DetailRoot: React.FC<Detail.Props> = ({
	metadata,
	actions,
	...props
}) => {
	const nativeProps: React.JSX.IntrinsicElements["detail"] = props;

	return (
		<detail {...nativeProps}>
			{actions}
			{metadata}
		</detail>
	);
};

/**
 * @category UI Components
 */
export const Detail = Object.assign(DetailRoot, {
	Metadata,
});
