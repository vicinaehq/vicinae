import { open, showToast, LocalStorage, LaunchProps, closeMainWindow } from "@omnicast/api";

export default async function Command(props: LaunchProps) {
	console.log({ props });
    open('/etc/passwd');

	await LocalStorage.setItem('something', 'hello');
	const value = await LocalStorage.getItem('something');

	await showToast({ title: `Hello from no-view: ${value}` });
	await closeMainWindow();
}
