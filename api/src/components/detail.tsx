import { Metadata, MetadataProps } from './metadata';

export type DetailProps = {
	navigationTitle?: string;
	metadata?: React.FC<MetadataProps>;
	markdown: string;
};

const DetailRoot: React.FC<DetailProps> = (props) => {
	const nativeProps: React.JSX.IntrinsicElements['detail'] = props;

	return (
		<detail {...nativeProps} />
	);
}

export const Detail = Object.assign(DetailRoot, {
	Metadata
});
