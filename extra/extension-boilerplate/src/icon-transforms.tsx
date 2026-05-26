import { Color, Icon, Image, List } from "@vicinae/api";
import { DATA_URI_PNG_BASE64, DATA_URI_SVG_BASE64 } from "./constants";

const HTTP_URL = "https://raycast.com/uploads/avatar.png";
const ASSET = "transparent_icon.png";
const THEMED = {
	light: "https://docs.vicinae.com/vicinae.svg",
	dark: "https://docs.vicinae.com/vicinae-dark.svg",
};

const TEXT_FILE = "/etc/hostname";
const EXECUTABLE = "/usr/bin/ls";
const DIRECTORY = "/tmp";
const MISSING_FILE = "/this/path/does/not/exist.zzz";

const OPAQUE_SQUARE = `data:image/svg+xml;utf8,${encodeURIComponent(
	`<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64'><rect width='64' height='64' fill='#a78bfa'/></svg>`,
)}`;

type Variant = {
	key: string;
	title: string;
	// biome-ignore lint/suspicious/noExplicitAny: ImageLike is a union; this command spreads
	// arbitrary props on top, which doesn't narrow cleanly without per-case branching.
	icon: any;
};

const sources: Variant[] = [
	{ key: "opaque-square", title: "Opaque square (SVG)", icon: OPAQUE_SQUARE },
	{ key: "enum", title: "Icon enum", icon: Icon.AppWindowGrid3x3 },
	{ key: "emoji", title: "Emoji", icon: "🐢" },
	{ key: "url", title: "URL (HTTP)", icon: HTTP_URL },
	{ key: "asset", title: "Asset", icon: ASSET },
	{ key: "datauri-svg", title: "Data URI (SVG)", icon: DATA_URI_SVG_BASE64 },
	{ key: "datauri-png", title: "Data URI (PNG)", icon: DATA_URI_PNG_BASE64 },
	{ key: "themed", title: "Themed source", icon: { source: THEMED } },
	{
		key: "file-text",
		title: "File icon (text)",
		icon: { fileIcon: TEXT_FILE },
	},
	{
		key: "file-exe",
		title: "File icon (executable)",
		icon: { fileIcon: EXECUTABLE },
	},
	{
		key: "file-dir",
		title: "File icon (directory)",
		icon: { fileIcon: DIRECTORY },
	},
];

const withProps = (variant: Variant, extra: Record<string, unknown>) => {
	const isObj =
		typeof variant.icon === "object" &&
		variant.icon !== null &&
		!Array.isArray(variant.icon);
	const base = isObj ? variant.icon : { source: variant.icon };
	return { ...base, ...extra };
};

const renderRow = (variant: Variant, transform: Record<string, unknown>) => (
	<List.Item
		key={variant.key}
		title={variant.title}
		icon={withProps(variant, transform)}
	/>
);

export default function IconTransforms() {
	return (
		<List>
			<List.Section title="Baseline (no transforms)">
				{sources.map((v) => (
					<List.Item key={v.key} title={v.title} icon={v.icon} />
				))}
			</List.Section>

			<List.Section title="Mask: Circle">
				{sources.map((v) => renderRow(v, { mask: Image.Mask.Circle }))}
			</List.Section>

			<List.Section title="Mask: RoundedRectangle">
				{sources.map((v) =>
					renderRow(v, { mask: Image.Mask.RoundedRectangle }),
				)}
			</List.Section>

			<List.Section title="Tint: solid (red)">
				{sources.map((v) => renderRow(v, { tintColor: Color.Red }))}
			</List.Section>

			<List.Section title="Tint: themed (light=blue, dark=yellow)">
				{sources.map((v) =>
					renderRow(v, {
						tintColor: { light: Color.Blue, dark: Color.Yellow },
					}),
				)}
			</List.Section>

			<List.Section title="Tint + Circle mask (green)">
				{sources.map((v) =>
					renderRow(v, {
						tintColor: Color.Green,
						mask: Image.Mask.Circle,
					}),
				)}
			</List.Section>

			<List.Section title="Fallback chains">
				<List.Item
					title="Bad asset → Icon enum"
					icon={{ source: "nope.png", fallback: Icon.QuestionMarkCircle }}
				/>
				<List.Item
					title="Bad URL → Asset"
					icon={{
						source: "https://example.invalid/nope.png",
						fallback: ASSET,
					}}
				/>
				<List.Item
					title="Bad URL → themed"
					icon={{
						source: "https://example.invalid/nope.png",
						fallback: THEMED,
					}}
				/>
				<List.Item
					title="File icon for missing path"
					icon={{ fileIcon: MISSING_FILE }}
				/>
			</List.Section>
		</List>
	);
}
