import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, join, relative } from "node:path";
import { createInterface, type Interface } from "node:readline/promises";
import type { CommandDef } from "../../cli.js";
import { Logger } from "../../utils/logger.js";
import { toolSource } from "../../utils/tools.js";

const TOOL_NAME_PATTERN = /^[a-z0-9-][a-zA-Z0-9-_]*$/;
const PLACEHOLDER_DESCRIPTION =
	"Describe what this tool does and when the AI should use it.";

type Validator = (value: string) => string | undefined;

const validateName: Validator = (name) =>
	name.length >= 2 && name.length <= 64 && TOOL_NAME_PATTERN.test(name)
		? undefined
		: "use 2 to 64 letters, digits, '-' or '_', starting with a lowercase letter, a digit or '-'";

const validateTitle: Validator = (title) =>
	title.length >= 2 && title.length <= 255 && !/\s{2,}/.test(title)
		? undefined
		: "use 2 to 255 characters, without consecutive spaces";

const validateDescription: Validator = (description) =>
	description.length >= 12 && description.length <= 2048
		? undefined
		: "use 12 to 2048 characters";

const titleFromName = (name: string) =>
	name
		.split(/[-_]+/)
		.filter(Boolean)
		.map((word) => word[0].toUpperCase() + word.slice(1))
		.join(" ");

/**
 * Asks for the values missing from the command line when running in a terminal. Outside of one
 * (scripts, CI), missing values fall back to their defaults, or are an error when there is none.
 */
class Prompter {
	private readonly rl = process.stdin.isTTY
		? createInterface({ input: process.stdin, output: process.stdout })
		: undefined;

	constructor(private readonly logger: Logger) {
		this.rl?.on("SIGINT", () => {
			process.stdout.write("\n");
			process.exit(130);
		});
	}

	async ask(
		label: string,
		given: string | undefined,
		validate: Validator,
		fallback?: string,
	): Promise<string> {
		if (given !== undefined) return this.check(label, given.trim(), validate);

		if (!this.rl) {
			if (fallback !== undefined) return fallback;
			this.logger.logError(`Missing ${label}`);
			process.exit(1);
		}

		for (;;) {
			const hint = fallback !== undefined ? ` (${fallback})` : "";
			const answer = (await this.question(this.rl, `${label}${hint}: `)).trim();
			const value = answer || fallback;
			if (value === undefined) continue;
			const error = validate(value);
			if (!error) return value;
			this.logger.logError(`Invalid ${label}: ${error}`);
		}
	}

	private async question(rl: Interface, query: string) {
		try {
			return await rl.question(query);
		} catch {
			// Ctrl+D
			process.stdout.write("\n");
			process.exit(130);
		}
	}

	close() {
		this.rl?.close();
	}

	private check(label: string, value: string, validate: Validator) {
		const error = validate(value);
		if (error) {
			this.logger.logError(`Invalid ${label} "${value}": ${error}`);
			process.exit(1);
		}
		return value;
	}
}

const toolTemplate = () => `type Input = {
	/** Describe each parameter: this is what the AI reads to know what to pass. */
	query: string;
};

/**
 * Instructions for the AI, on top of the tool's description in package.json.
 */
export default async function tool(input: Input) {
	return { query: input.query };
}

// To ask the user before the tool runs, import \`Tool\` from "@vicinae/api" and export:
//
// export const confirmation: Tool.Confirmation<Input> = async (input) => ({
// 	message: \`Run with "\${input.query}"?\`,
// });
`;

type Generator = (
	logger: Logger,
	prompter: Prompter,
	src: string,
	name: string | undefined,
	flags: Record<string, string | undefined>,
) => Promise<void>;

const generateTool: Generator = async (
	logger,
	prompter,
	src,
	givenName,
	flags,
) => {
	const pkgPath = join(src, "package.json");
	if (!existsSync(pkgPath)) {
		logger.logError(
			`No package.json found at ${pkgPath}. Does this location point to a valid extension repository?`,
		);
		process.exit(1);
	}

	const raw = readFileSync(pkgPath, "utf8");
	const pkg = JSON.parse(raw);
	const tools: { name: string }[] = pkg.tools ?? [];

	const validateNewName: Validator = (name) => {
		const error = validateName(name);
		if (error) return error;
		if (tools.some((tool) => tool.name === name)) {
			return "a tool with this name is already declared in package.json";
		}
		const existing = toolSource(src, name);
		if (existing) return `${relative(src, existing)} already exists`;
		return undefined;
	};

	const name = await prompter.ask("tool name", givenName, validateNewName);
	const title = await prompter.ask(
		"title",
		flags.title,
		validateTitle,
		titleFromName(name),
	);
	const description = await prompter.ask(
		"description",
		flags.description,
		validateDescription,
		process.stdin.isTTY ? undefined : PLACEHOLDER_DESCRIPTION,
	);

	const sourcePath = join(src, "src", "tools", `${name}.ts`);
	mkdirSync(dirname(sourcePath), { recursive: true });
	writeFileSync(sourcePath, toolTemplate());

	pkg.tools = [...tools, { name, title, description }];

	const indent = raw.match(/^[ \t]+(?=")/m)?.[0] ?? "\t";
	writeFileSync(pkgPath, `${JSON.stringify(pkg, null, indent)}\n`);

	logger.logReady(
		`created ${relative(src, sourcePath)} and declared it in package.json`,
	);
	if (description === PLACEHOLDER_DESCRIPTION) {
		logger.logInfo(
			"Don't forget to write the tool's description in package.json: the AI relies on it to pick the tool.",
		);
	}
};

const GENERATORS: Record<string, Generator> = {
	tool: generateTool,
};

const generate: CommandDef = {
	description: "Generate extension boilerplate (tool)",
	aliases: ["g"],
	usage: "tool [name]",
	flags: {
		title: {
			short: "t",
			description:
				"Title shown in the Vicinae UI. Prompted for, or derived from the name.",
		},
		description: {
			short: "d",
			description: "What the generated item does. Prompted for if missing.",
		},
		src: {
			short: "s",
			description: "Path to the extension source directory",
			default: process.cwd(),
		},
	},

	async run(flags, args) {
		const logger = new Logger();
		const [kind, name] = args;
		const generator = kind ? GENERATORS[kind] : undefined;

		if (!generator) {
			logger.logError(
				`Unknown generator "${kind ?? ""}". Available: ${Object.keys(GENERATORS).join(", ")}`,
			);
			process.exit(1);
		}

		const prompter = new Prompter(logger);
		try {
			await generator(
				logger,
				prompter,
				flags.src ?? process.cwd(),
				name,
				flags,
			);
		} finally {
			prompter.close();
		}
	},
};

export default generate;
