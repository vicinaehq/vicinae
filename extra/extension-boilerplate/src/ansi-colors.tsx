import { Action, ActionPanel, Detail, Icon, List, showToast } from "@vicinae/api";
import {
	bold,
	italic,
	underline,
	strikethrough,
	dim,
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	redBright,
	greenBright,
	yellowBright,
	blueBright,
	magentaBright,
	cyanBright,
	bgRed,
	bgGreen,
	bgBlue,
} from "./yoctocolors.js";

const items = [
	{ title: red("Red text"), subtitle: "Standard red foreground" },
	{ title: green("Green text"), subtitle: "Standard green foreground" },
	{ title: yellow("Yellow text"), subtitle: "Standard yellow foreground" },
	{ title: blue("Blue text"), subtitle: "Standard blue foreground" },
	{ title: magenta("Magenta text"), subtitle: "Standard magenta foreground" },
	{ title: cyan("Cyan text"), subtitle: "Standard cyan foreground" },
	{ title: redBright("Bright red"), subtitle: "Bright red foreground" },
	{ title: greenBright("Bright green"), subtitle: "Bright green foreground" },
	{ title: yellowBright("Bright yellow"), subtitle: "Bright yellow foreground" },
	{ title: blueBright("Bright blue"), subtitle: "Bright blue foreground" },
	{ title: magentaBright("Bright magenta"), subtitle: "Bright magenta foreground" },
	{ title: cyanBright("Bright cyan"), subtitle: "Bright cyan foreground" },
	{ title: bold("Bold text"), subtitle: "Bold attribute" },
	{ title: italic("Italic text"), subtitle: "Italic attribute" },
	{ title: underline("Underlined text"), subtitle: "Underline attribute" },
	{ title: strikethrough("Strikethrough text"), subtitle: "Strikethrough attribute" },
	{ title: dim("Dim text"), subtitle: "Dim attribute" },
	{ title: bold(red("Bold red")), subtitle: "Combined bold + color" },
	{ title: italic(cyan("Italic cyan")), subtitle: "Combined italic + color" },
	{
		title: red("Red ") + green("Green ") + blue("Blue"),
		subtitle: "Multiple colors in one line",
	},
	{ title: bgRed(" White on red "), subtitle: "Background color" },
	{ title: bgGreen(" White on green "), subtitle: "Background color" },
	{ title: bgBlue(" White on blue "), subtitle: "Background color" },
];

export default function AnsiColors() {
	return (
		<List searchBarPlaceholder="Search ANSI colors...">
			<List.Section title="ANSI Escape Sequence Showcase">
				{items.map((item, i) => (
					<List.Item
						key={i.toString()}
						title={item.title}
						subtitle={item.subtitle}
						actions={
							<ActionPanel>
								<Action
									title="Show Toast"
									icon={Icon.Eye}
									onAction={() =>
										showToast({ title: item.title })
									}
								/>
								<Action.CopyToClipboard
									title="Copy Colored Text"
									content={item.title}
								/>
							</ActionPanel>
						}
					/>
				))}
			</List.Section>
		</List>
	);
}
