import {
	Color,
	environment,
	Icon,
	LaunchType,
	sendDesktopNotification,
	showToast,
} from "@vicinae/api";

export default async function DesktopNotificationInterval() {
	if (environment.launchType === LaunchType.Background) {
		await sendDesktopNotification({
			title: "Vicinae Extension Boilerplate Notification",
			body: "This is the body of the notification, hopefully it displays correctly!",
			icon: { source: Icon.Cog, tintColor: Color.Red },
		});
	} else {
		await showToast({ title: "Manually triggered, sent as a toast!" });
	}
}
