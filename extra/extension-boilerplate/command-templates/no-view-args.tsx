import { LaunchProps, showToast, clearSearchBar } from '@vicinae/api';

export default async function NoViewWithArgs(props: LaunchProps) {
	await clearSearchBar();
	await showToast(props.arguments.text);
}
