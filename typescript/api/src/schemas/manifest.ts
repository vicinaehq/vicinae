import { z } from "zod";

// Preference schema for extension and command preferences
const preferenceBase = {
	name: z.string().min(1).describe("A unique id for the preference."),
	title: z
		.string()
		.describe(
			"The display name of the preference shown in Raycast preferences. For 'checkbox', 'textfield' and 'password', it is shown as a section title above the respective input element. If you want to group multiple checkboxes into a single section, set the 'title' of the first checkbox and leave the 'title' of the other checkboxes empty.",
		),
	description: z
		.string()
		.describe(
			"It helps users understand what the preference does. It will be displayed as a tooltip when hovering over it.",
		),
	required: z
		.boolean()
		.describe(
			"Indicates whether the value is required and must be entered by the user before the extension is usable.",
		),
};

const preferenceSchema = z.discriminatedUnion("type", [
	// textfield preference
	z
		.object({
			...preferenceBase,
			type: z.literal("textfield"),
			placeholder: z
				.string()
				.describe(
					"Text displayed in the preference's field when no value has been input.",
				)
				.optional(),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For textfields, this is a string value. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// password preference
	z
		.object({
			...preferenceBase,
			type: z.literal("password"),
			placeholder: z
				.string()
				.describe(
					"Text displayed in the preference's field when no value has been input.",
				)
				.optional(),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For passwords, this is a string value. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// checkbox preference
	z
		.object({
			...preferenceBase,
			type: z.literal("checkbox"),
			label: z
				.string()
				.min(1)
				.describe("The label of the checkbox. Shown next to the checkbox."),
			default: z
				.union([
					z.boolean(),
					z.object({
						macOS: z.boolean().optional(),
						Windows: z.boolean().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For checkboxes, this is a boolean value. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// dropdown preference
	z
		.object({
			...preferenceBase,
			type: z.literal("dropdown"),
			data: z
				.array(
					z.object({
						title: z.string(),
						value: z.string(),
					}),
				)
				.min(1)
				.describe(
					"An array of objects with 'title' and 'value' properties, e.g.: [{'title': 'Item 1', 'value': '1'}]",
				),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For dropdowns, this is the value of an object in the data array. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// appPicker preference
	z
		.object({
			...preferenceBase,
			type: z.literal("appPicker"),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For appPickers, this is an application name, bundle ID or path. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// file preference
	z
		.object({
			...preferenceBase,
			type: z.literal("file"),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For file preferences, this is a file path. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
	// directory preference
	z
		.object({
			...preferenceBase,
			type: z.literal("directory"),
			default: z
				.union([
					z.string(),
					z.object({
						macOS: z.string().optional(),
						Windows: z.string().optional(),
					}),
				])
				.describe(
					"The optional default value for the field. For directory preferences, this is a directory path. Additionally, you can specify a different value per platform by passing an object: { 'macOS': ..., 'Windows': ... }.",
				)
				.optional(),
		})
		.strict(),
]);

// Argument schema for command arguments
const argumentBase = {
	name: z
		.string()
		.min(1)
		.describe(
			"A unique id for the argument. This value will be used to as the key in the object passed as top-level prop.",
		),
	placeholder: z
		.string()
		.min(1)
		.describe("Placeholder for the argument's input field."),
	required: z
		.boolean()
		.describe(
			"Indicates whether the value is required and must be entered by the user before the command is opened. Default value for this is 'false'.",
		)
		.optional(),
};

const argumentSchema = z.discriminatedUnion("type", [
	// text argument
	z
		.object({
			...argumentBase,
			type: z.literal("text"),
		})
		.strict(),
	// password argument
	z
		.object({
			...argumentBase,
			type: z.literal("password"),
		})
		.strict(),
	// dropdown argument
	z
		.object({
			...argumentBase,
			type: z.literal("dropdown"),
			data: z
				.array(
					z.object({
						title: z.string(),
						value: z.string(),
					}),
				)
				.min(1)
				.describe(
					"An array of objects with 'title' and 'value' properties, e.g.: [{'title': 'Item 1', 'value': '1'}]",
				),
		})
		.strict(),
]);

export default z.object({
	icon: z
		.string()
		.regex(new RegExp("^[^\\s]+(?:[ ]*[^\\s]+)*$"))
		.describe(
			"A 512x512 icon representing the extension. It will be displayed in the Store and in Preferences. If any of the extension's commands doesn't have an icon, it will 'inherit' the extension's icon. Please note that light and dark themes are supported; just append '@dark' to the filename for the dark theme and the correct icon will be picked at run-time. For example, set for this property 'icon.png' and put in the assets folder the 'icon.png' and 'icon@dark.png' files.",
		),
	name: z
		.string()
		.regex(new RegExp("^(@workaround/)?[a-z0-9-~][a-z0-9-_~]*$"))
		.min(3)
		.max(255)
		.describe(
			"The slugged extension's name used internally as identifier and in the store as part of the URL.",
		),
	debug: z
		.object({
			reloadShortcut: z
				.object({
					key: z
						.string()
						.min(1)
						.max(2)
						.describe(
							"The keyboard key pressed with the modifier to trigger an action (e.g. ⌘ + B)",
						),
					modifiers: z
						.array(z.enum(["command", "option", "control", "shift"]))
						.min(1)
						.max(4)
						.describe(
							"Such as the command modifier (⌘), option modifier (⌥), control modifier (⌃), or shift modifier (⇧).",
						),
				})
				.strict()
				.describe(
					"Keyboard shortcut used to refresh/reload a command while the Vicinae window is focused.",
				)
				.optional(),
		})
		.describe(
			"Compilation and run-time options to improve development experience.",
		)
		.optional(),
	owner: z
		.string()
		.regex(new RegExp("^[a-zA-Z0-9-*~][a-zA-Z0-9-*._~]*$"))
		.min(2)
		.max(75)
		.describe(
			"User ultimately responsible for the extension. The extension's store URL is composed of the owner username and the extension's name.",
		)
		.optional(),
	title: z
		.string()
		.regex(new RegExp("^[^\\s]+(?: [^\\s]+)*$"))
		.min(2)
		.max(255)
		.describe(
			"The extension title will be displayed in the Store and in Preferences.",
		),
	access: z
		.enum(["public", "private"])
		.describe(
			"Public extensions are downloadable by anybody, while private extensions can only be downloaded by a member of a given organization.",
		)
		.optional(),
	author: z
		.string()
		.regex(new RegExp("^[a-zA-Z0-9-*~][a-zA-Z0-9-*._~]*$"))
		.min(2)
		.max(75)
		.describe("User currently contributing the most to the extension."),
	license: z
		.literal("MIT")
		.describe(
			"Currently only MIT is accepted, although more licenses will probably be available in the future.",
		),
	platforms: z
		.array(z.enum(["macOS", "Windows"]))
		.min(1)
		.describe(
			"Currently only `macOS` and `Windows` are accepted. If not present, the extension is assumed to be available on all platforms.",
		)
		.optional(),
	commands: z
		.array(
			z
				.object({
					icon: z
						.string()
						.regex(new RegExp("^[^\\s]+(?:[ ]*[^\\s]+)*$"))
						.describe(
							"A 512x512 icon representing the command. It will be displayed in Preferences and Vicinae root search. If a command doesn't have an icon, it will 'inherit' the extension's icon. Please note that light and dark themes are supported; just append '@dark' or '@light' to the icons name and the correct icon will be picked at run-time. For example, write for this property 'icon.png' and have in the assets folder the 'icon@light.png' and 'icon@dark.png' assets.",
						)
						.optional(),
					mode: z
						.enum(["view", "no-view", "menu-bar"])
						.describe(
							"A value of 'view' indicates that the command will show a main view when performed. 'no-view' means that the command does not push a view to the main navigation stack in Vicinae. The latter is handy for directly opening URL or other API functionality that doesn't require a user interface. 'menu-bar' renders an extra item in the macOS system menu bar at the top of the screen.",
						),
					name: z
						.string()
						.regex(new RegExp("^[a-z0-9-~][a-zA-Z0-9-._~]*$"))
						.min(2)
						.max(255)
						.describe(
							"The name directly maps to the entry point file for the command. So a command named 'index' would map to 'src/index.ts' (or any other supported TypeScript or JavaScript file extension such as .tsx, .js, .jsx).",
						),
					title: z
						.string()
						.regex(new RegExp("^[^\\s]+(?: [^\\s]+)*$"))
						.min(2)
						.max(255)
						.describe(
							"The command title will be displayed in the Store, Preferences, and in Vicinae's root search.",
						),
					keywords: z.any().optional(),
					subtitle: z
						.string()
						.regex(new RegExp("^[^\\s]+(?: [^\\s]+)*$"))
						.min(2)
						.max(255)
						.describe(
							"The subtitle (if any) will be displayed next to the command name in the root search. It helps user differentiate potentially similar commands. For example, a Google Suite extension may define 2 commands titled 'Create Document'. To differentiate them, one of the subtitle can be 'Google Docs' while the other can be 'Google Sheets'.",
						)
						.optional(),
					description: z
						.string()
						.regex(new RegExp("^[^\\s]+(\\s+[^\\s]+)*$"))
						.min(0)
						.max(2048)
						.describe(
							"It helps users understand what the command does. It will be displayed in the Store and in Preferences.",
						),
					interval: z
						.string()
						.regex(new RegExp("^(\\d+)(s|m|h|d)$"))
						.describe(
							"The value specifies that a no-view or menu-bar command should be launched in the background every X seconds (s), minutes (m), hours (h) or days (d). Examples: 90s, 1m, 12h, 1d. The minimum value is 10 seconds (10s).",
						)
						.optional(),
					preferences: z
						.array(preferenceSchema)
						.describe(
							'Commands can optionally contribute preferences that are shown in Vicinae Preferences > Extensions when selecting the command. You can use preferences for configuration values and passwords or personal access tokens. Commands automatically "inherit" extension preferences and can also override entries with the same `name`.',
						)
						.optional(),
					arguments: z
						.array(argumentSchema)
						.describe(
							"An optional array of arguments that are requested from user when the command is called",
						)
						.optional(),
					disabledByDefault: z
						.boolean()
						.describe("Defaults to `false`")
						.optional(),
				})
				.catchall(z.any()),
		)
		.min(1)
		.max(100)
		.describe(
			"List of all commands vended by this extensions. An extension must contain at least one command.",
		),
	tools: z
		.array(
			z
				.object({
					icon: z
						.string()
						.regex(new RegExp("^[^\\s]+(?:[ ]*[^\\s]+)*$"))
						.describe(
							"A 512x512 icon representing the tool. It will be displayed in various places across Vicinae. If a tool doesn't have an icon, it will 'inherit' the extension's icon. Please note that light and dark themes are supported; just append '@dark' or '@light' to the icons name and the correct icon will be picked at run-time. For example, write for this property 'icon.png' and have in the assets folder the 'icon@light.png' and 'icon@dark.png' assets.",
						)
						.optional(),
					name: z
						.string()
						.regex(new RegExp("^[a-z0-9-][a-zA-Z0-9-_]*$"))
						.min(2)
						.max(64)
						.describe(
							"The name directly maps to the entry point file for the tool. So a tool named 'index' would map to 'src/tools/index.ts' (or any other supported TypeScript or JavaScript file extension such as .tsx, .js, .jsx).",
						),
					title: z
						.string()
						.regex(new RegExp("^[^\\s]+(?: [^\\s]+)*$"))
						.min(2)
						.max(255)
						.describe(
							"The tool title will be displayed in the Store, Preferences, and any other places the tool might be referenced in the Vicinae UI.",
						),
					keywords: z.any().optional(),
					description: z
						.string()
						.regex(new RegExp("^[^\\s]+(\\s+[^\\s]+)*$"))
						.min(12)
						.max(2048)
						.describe(
							"It helps users (and other actors like AI) understand what the tool does. It will be displayed in the Store and in Preferences.",
						),
					functionalities: z
						.array(z.enum(["AI attachment provider", "AI tool"]))
						.describe(
							"Limits the tool to the specified functionalities. If not specified, the tool can be used in any context (if it matches the requirements for each of them).",
						)
						.optional(),
					preferences: z
						.array(preferenceSchema)
						.describe(
							'Tools can optionally contribute preferences that are shown in Vicinae Preferences > Extensions when selecting the AI Extension item. You can use preferences for configuration values and passwords or personal access tokens. Tools automatically "inherit" extension preferences and can also override entries with the same `name`.',
						)
						.optional(),
				})
				.catchall(z.any()),
		)
		.min(0)
		.max(100)
		.describe(
			"List of all tools that the AI can use to interact with this extension.",
		)
		.optional(),
	ai: z
		.object({
			instructions: z
				.string()
				.describe(
					"Additional system instructions added when the tools are used in AI",
				)
				.optional(),
			evals: z
				.array(
					z.object({
						input: z.string().describe("The prompt to evaluate"),
						usedAsExample: z
							.boolean()
							.describe(
								"Whether the eval can be used as an example in Vicinae (default `true`)",
							)
							.optional(),
					}),
				)
				.describe(
					"List of tests to evaluate the reliability of the tools when used in AI",
				)
				.optional(),
		})
		.describe(
			"Additional information related to the AI capabilities of the extension",
		)
		.optional(),
	keywords: z.any().optional(),
	description: z
		.string()
		.regex(new RegExp("^[^\\s]+(\\s+[^\\s]+)*$"))
		.min(16)
		.max(2048)
		.describe(
			"It helps users understand what the extension does. It will be displayed in the Store and in Preferences.",
		),
	preferences: z
		.array(preferenceSchema)
		.describe(
			"Extensions can contribute preferences that are shown in Vicinae Preferences > Extensions. You can use preferences for configuration values and passwords or personal access tokens.",
		)
		.optional(),
	categories: z
		.array(
			z.enum([
				"Applications",
				"Communication",
				"Data",
				"Documentation",
				"Design Tools",
				"Developer Tools",
				"Finance",
				"Fun",
				"Media",
				"News",
				"Productivity",
				"Security",
				"System",
				"Web",
				"Other",
			]),
		)
		.optional(),
	contributors: z
		.array(
			z
				.string()
				.regex(new RegExp("^[a-zA-Z0-9-*~][a-zA-Z0-9-*._~]*$"))
				.min(2)
				.max(75),
		)
		.min(0)
		.describe(
			"Users who have meaningfully contributed to the extension's commands.",
		)
		.optional(),
	pastContributors: z
		.array(
			z
				.string()
				.regex(new RegExp("^[a-zA-Z0-9-*~][a-zA-Z0-9-*._~]*$"))
				.min(2)
				.max(75),
		)
		.min(0)
		.describe(
			"Users who have meaningfully contributed to the extension's commands but do not maintain it anymore.",
		)
		.optional(),
	dependencies: z
		.object({
			"@vicinae/api": z
				.string()
				.describe("The Vicinae API version used by this extension.")
				.optional(),
		})
		.describe(
			"Source dependencies following the npm package.json dependency format.",
		),
	external: z
		.array(z.string())
		.describe(
			"An Array of package or file names that should be excluded from the build. The package will not be bundled, but the import is preserved and will be evaluated at runtime.",
		)
		.optional(),
});
