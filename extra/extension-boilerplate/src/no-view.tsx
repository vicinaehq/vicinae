import { showToast, clearSearchBar } from "@vicinae/api";

export default async function NoView() {
	await clearSearchBar();
	await showToast({ title: "Hello from no view!" });
}
