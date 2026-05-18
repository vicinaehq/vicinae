import { existsSync, readFileSync } from "node:fs";
import { join } from "node:path";
import type { CommandDef } from "../../cli.js";
import ManifestSchema from "../../schemas/manifest.js";
import { Logger } from "../../utils/logger.js";

const lint: CommandDef = {
	description: "Validate the extension manifest (package.json)",
	flags: {
		src: {
			short: "s",
			description: "Path to the extension source directory",
			default: process.cwd(),
		},
	},

	async run(flags) {
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
	},
};

export default lint;
