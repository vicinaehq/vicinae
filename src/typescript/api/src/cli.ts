import build from "./commands/build/index.js";
import develop from "./commands/develop/index.js";
import lint from "./commands/lint/index.js";

interface FlagDef {
	short?: string;
	description: string;
	default?: string;
}

export interface CommandDef {
	description: string;
	flags: Record<string, FlagDef>;
	run: (flags: Record<string, string | undefined>) => Promise<void>;
}

function parseFlags(
	argv: string[],
	defs: Record<string, FlagDef>,
): Record<string, string | undefined> {
	const result: Record<string, string | undefined> = {};

	for (const [name, def] of Object.entries(defs)) {
		if (def.default !== undefined) result[name] = def.default;
	}

	const shortToLong = new Map<string, string>();
	for (const [name, def] of Object.entries(defs)) {
		if (def.short) shortToLong.set(def.short, name);
	}

	for (let i = 0; i < argv.length; i++) {
		const arg = argv[i];

		if (arg.startsWith("--")) {
			const eqIdx = arg.indexOf("=");
			if (eqIdx !== -1) {
				result[arg.slice(2, eqIdx)] = arg.slice(eqIdx + 1);
			} else {
				result[arg.slice(2)] = argv[++i];
			}
		} else if (arg.startsWith("-") && arg.length === 2) {
			const long = shortToLong.get(arg[1]);
			if (long) result[long] = argv[++i];
		}
	}

	return result;
}

function printHelp(commands: Record<string, CommandDef>) {
	console.log("vici - Vicinae extension development CLI\n");
	console.log("USAGE");
	console.log("  $ vici <command> [flags]\n");
	console.log("COMMANDS");
	for (const [name, cmd] of Object.entries(commands)) {
		console.log(`  ${name.padEnd(12)} ${cmd.description}`);
	}
	console.log(
		`\nRun "vici <command> --help" for more information about a command.`,
	);
}

function printCommandHelp(name: string, cmd: CommandDef) {
	console.log(`vici ${name} - ${cmd.description}\n`);
	console.log("USAGE");
	console.log(`  $ vici ${name} [flags]\n`);

	const flagEntries = Object.entries(cmd.flags);
	if (flagEntries.length === 0) return;

	console.log("FLAGS");
	for (const [flagName, def] of flagEntries) {
		const short = def.short ? `-${def.short}, ` : "    ";
		const defaultHint =
			def.default !== undefined
				? ` (default: ${def.default === process.cwd() ? "cwd" : def.default})`
				: "";
		console.log(
			`  ${short}--${flagName.padEnd(10)} ${def.description}${defaultHint}`,
		);
	}
}

export async function main() {
	const commands: Record<string, CommandDef> = { build, develop, lint };
	const [commandName, ...rest] = process.argv.slice(2);

	if (!commandName || commandName === "--help" || commandName === "-h") {
		printHelp(commands);
		return;
	}

	const cmd = commands[commandName];
	if (!cmd) {
		console.error(`Unknown command: ${commandName}\n`);
		printHelp(commands);
		process.exit(1);
	}

	if (rest.includes("--help") || rest.includes("-h")) {
		printCommandHelp(commandName, cmd);
		return;
	}

	const flags = parseFlags(rest, cmd.flags);
	await cmd.run(flags);
}
