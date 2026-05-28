import { Color, Icon, sendDesktopNotification } from "@vicinae/api";

export default async function DesktopNotificationInterval() {
	await sendDesktopNotification({
		title: "This is the title",
		body: "This is the body of the notification, hopefully it displays correctly!",
		icon: { source: Icon.Cog, tintColor: Color.Red },
	});
}
