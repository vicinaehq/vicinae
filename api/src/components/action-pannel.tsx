import { ReactNode } from "react";

export type ActionPannelProps = {
	title: string;
	children: ReactNode;
};

export const ActionPannel: React.FC<ActionPannelProps> = ({ children, ...props }) => {
	return (
		<action-pannel {...props}>
			{children}
		</action-pannel>
	);
}
