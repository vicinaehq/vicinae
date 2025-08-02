import { ReactNode } from 'react';
import { Metadata } from './metadata';

export type DetailProps = {
	navigationTitle?: string;
	metadata?: ReactNode;
	markdown: string;
	actions?: ReactNode;
};

const DetailRoot: React.FC<DetailProps> = ({ metadata, ...props }) => {
	const nativeProps: React.JSX.IntrinsicElements['detail'] = props;

	return (
		<detail {...nativeProps}>
			{metadata}
		</detail>
	);
}

export const Detail = Object.assign(DetailRoot, {
	Metadata
});
