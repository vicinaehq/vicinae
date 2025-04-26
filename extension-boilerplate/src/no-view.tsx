import { open, showToast, LocalStorage } from "@omnicast/api";

export default async function Command() {
    open('/etc/passwd');

	await LocalStorage.setItem('something', 'hello');
	const value = await LocalStorage.getItem('something');

	await showToast({ title: `Hello from no-view: ${value}` });
}
