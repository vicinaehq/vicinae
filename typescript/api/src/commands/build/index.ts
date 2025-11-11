import { Command, Flags } from "@oclif/core";
import * as esbuild from "esbuild";
import { spawnSync } from "node:child_process";
import { cpSync, existsSync, mkdirSync, readFileSync } from "node:fs";
import { join } from "node:path";
import { Logger } from "../../utils/logger.js";
import { extensionDataDir } from "../../utils/utils.js";
import { updateExtensionTypes } from "../../utils/extension-types.js";
import ManifestSchema from "../../schemas/manifest.js";

export default class Build extends Command {
	static args = {};
	static description = "Start an extension development session";
	static examples = [
		`<%= config.bin %> <%= command.id %> --target /path/to/extension`,
	];
	static flags = {
		out: Flags.string({
			aliases: ["output"],
			char: "o",
			description:
				"Path to output the compiled extension bundle to. Defaults to Vicinae extension directory.",
			required: false,
		}),
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
		const { flags } = await this.parse(Build);
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

		const e = ManifestSchema.safeParse(json);

		if (e.error) {
			logger.logError(
				`${pkgPath} is not a valid extension manifest: ${e.error}`,
			);
			process.exit(1);
		}

		const manifest = e.data;
		const outDir = flags.out ?? join(extensionDataDir(), manifest.name);

		const build = async (outDir: string) => {
			const entryPoints = manifest.commands.map((cmd) =>
				join("src", `${cmd.name}.tsx`),
			);

			logger.logInfo(`entrypoints [${entryPoints.join(", ")}]`);

			const promises = manifest.commands.map((cmd) => {
				const base = join(process.cwd(), "src", `${cmd.name}`);
				const tsxSource = `${base}.tsx`;
				const tsSource = `${base}.ts`;
				let source = tsxSource;

				if (cmd.mode === "view" && !existsSync(tsxSource)) {
					throw new Error(
						`Unable to find view command ${cmd.name} at ${tsxSource}`,
					);
				}

				// we allow .ts or .tsx for no-view
				if (cmd.mode === "no-view") {
					if (!existsSync(tsxSource)) {
						source = tsSource;
						if (!existsSync(tsSource)) {
							throw new Error(
								`Unable to find no-view command ${cmd.name} at ${base}.{ts,tsx}`,
							);
						}
					}
				}

				return esbuild.build({
					bundle: true,
					entryPoints: [source],
					external: ["react", "@vicinae/api", "@raycast/api"],
					format: "cjs",
					outfile: join(outDir, `${cmd.name}.js`),
					platform: "node",
					minify: true,
				});
			});

			await Promise.all(promises);

			const targetPkg = join(outDir, "package.json");
			const targetAssets = join(outDir, "assets");

			cpSync("package.json", targetPkg, { force: true });

			if (existsSync("assets")) {
				cpSync("assets", targetAssets, { force: true, recursive: true });
			} else {
				mkdirSync(targetAssets, { recursive: true });
			}
		};

		process.chdir(src);

		logger.logInfo("Generating extension types...");
		updateExtensionTypes(manifest, src);

		logger.logInfo("Checking types...");
		const typeCheck = spawnSync("npx", ["tsc", "--noEmit"]);

		if (typeCheck.error) {
			logger.logError(`Type check failed: ${typeCheck.error}`);
			process.exit(1);
		}

		mkdirSync(outDir, { recursive: true });
		await build(outDir);
		logger.logReady(`built extension successfully - output at ${outDir}`);
	}
}
