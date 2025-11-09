import { Color, Icon, Image, List } from "@vicinae/api";
import {
	DATA_URI_PNG_BASE64,
	DATA_URI_SVG_BASE64,
	DATA_URI_SVG_PLAIN,
	DATA_URI_SVG_URLENCODED,
} from "./constants";

export default function ListIcons() {
	return (
		<List>
			{/* Icon Source Types */}
			<List.Section title="Icon Source Types">
				{/* Strings */}
				<List.Item
					title="URL string"
					icon="https://raycast.com/uploads/avatar.png"
				/>
				<List.Item title="Asset string" icon="transparent_icon.png" />
				<List.Item title="Icon enum" icon={Icon.CircleEllipsis} />
				<List.Item title="Data URI SVG plain" icon={DATA_URI_SVG_PLAIN} />
				<List.Item
					title="Data URI SVG urlencoded"
					icon={DATA_URI_SVG_URLENCODED}
				/>
				<List.Item title="Data URI SVG base64" icon={DATA_URI_SVG_BASE64} />
				<List.Item title="Data URI png" icon={DATA_URI_PNG_BASE64} />

				{/* Objects */}
				<List.Item
					title="Image object with source"
					icon={{
						source: Icon.Emoji,
					}}
				/>
				<List.Item
					title="Image object with themed source (dark/light)"
					icon={{
						source: {
							dark: "https://docs.vicinae.com/vicinae-dark.svg",
							light: "https://docs.vicinae.com/vicinae.svg",
						},
					}}
				/>
				<List.Item
					title="Non-existent Asset image with fallback to icon enum"
					icon={{
						source: "non_existent_image.png",
						fallback: Icon.QuestionMark,
					}}
				/>
				<List.Item
					title="Non-existent Asset image with fallback to asset"
					icon={{
						source: "non_existent_image.png",
						fallback: "transparent_icon.png",
					}}
				/>
				<List.Item
					title="Non-existent Asset image with fallback to themed source (dark/light)"
					icon={{
						source: "non_existent_image.png",
						fallback: {
							dark: "https://docs.vicinae.com/vicinae-dark.svg",
							light: "https://docs.vicinae.com/vicinae.svg",
						},
					}}
				/>
			</List.Section>

			{/* Icon Tint Color */}
			<List.Section title="Icon Tint Color">
				<List.Item
					title="URL image with Color enum tint"
					icon={{
						source: "https://raycast.com/uploads/avatar.png",
						tintColor: Color.Red,
					}}
				/>
				<List.Item
					title="Data URI image with Color enum tint"
					icon={{
						source: DATA_URI_SVG_PLAIN,
						tintColor: Color.Purple,
					}}
				/>
				<List.Item
					title="Asset image with hex string tint"
					icon={{
						source: "transparent_icon.png",
						tintColor: "#00FF00",
					}}
				/>
				<List.Item
					title="Icon enum with dynamic color tint (dark/light)"
					icon={{
						source: Icon.Monitor,
						tintColor: {
							light: Color.Blue,
							dark: Color.Yellow,
						},
					}}
					accessories={[
						{ tag: { color: Color.Blue, value: "Light" } },
						{ tag: { color: Color.Yellow, value: "Dark" } },
					]}
				/>
			</List.Section>

			{/* Icon with Mask */}
			<List.Section title="Icon with Mask">
				<List.Item
					title="URL image with Circle mask"
					icon={{
						source: "https://raycast.com/uploads/avatar.png",
						mask: Image.Mask.Circle,
					}}
				/>
				<List.Item
					title="Data URI image with RoundedRectangle mask"
					icon={{
						source: DATA_URI_PNG_BASE64,
						mask: Image.Mask.RoundedRectangle,
					}}
				/>
				<List.Item
					title="Asset image with Circle mask"
					icon={{
						source: "extension_icon.png",
						mask: Image.Mask.Circle,
					}}
				/>
				<List.Item
					title="Icon enum with RoundedRectangle mask"
					icon={{
						source: Icon.AppWindowGrid3x3,
						mask: Image.Mask.RoundedRectangle,
					}}
				/>
			</List.Section>
		</List>
	);
}
