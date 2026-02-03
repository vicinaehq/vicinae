import { Command, Flags } from "@oclif/core";
import { existsSync, readFileSync } from "node:fs";
import { join } from "node:path";
import { Logger } from "../../utils/logger.js";
import ManifestSchema from "../../schemas/manifest.js";

export default class Lint extends Command {
	static args = {};
	static description = "Validate the extension manifest (package.json)";
	static examples = [
		`<%= config.bin %> <%= command.id %>`,
		`<%= config.bin %> <%= command.id %> --src /path/to/extension`,
	];
	static flags = {
		src: Flags.string({
			aliases: ["src"],
			char: "s",
			default: process.cwd(),
			defaultHelp: "The current working directory",
			description: "Path to the extension source directory",
			required: false,
		}),
	};

	async run(): Promise<void> {
		const { flags } = await this.parse(Lint);
		const logger = new Logger();
		const src = flags.src ?? process.cwd();
		const pkgPath = join(src, "package.json");

		if (!existsSync(pkgPath)) {
			logger.logError(
				`No package.json found at ${pkgPath}. Does this location point to a valid extension repository?`,
			);
			process.exit(1);
		}

		const json = JSON.parse(readFileSync(pkgPath, "utf8"));

		const result = ManifestSchema.safeParse(json);

		if (result.error) {
			logger.logError(
				`${pkgPath} is not a valid extension manifest: ${result.error}`,
			);
			process.exit(1);
		}

		logger.logReady(`Manifest is valid`);
	}
}
