import * as esbuild from "esbuild";
import { cpSync, existsSync, mkdirSync, readFileSync } from "node:fs";
import { join } from "node:path";
import type { CommandDef } from "../../cli.js";
import ManifestSchema from "../../schemas/manifest.js";
import { updateExtensionTypes } from "../../utils/extension-types.js";
import { Logger } from "../../utils/logger.js";
import { typeCheck } from "../../utils/typecheck.js";
import { extensionDataDir } from "../../utils/utils.js";

const build: CommandDef = {
	description: "Build an extension for distribution",
	flags: {
		out: {
			short: "o",
			description:
				"Path to output the compiled extension bundle to. Defaults to Vicinae extension directory.",
		},
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
		const e = ManifestSchema.safeParse(json);

		if (e.error) {
			logger.logError(
				`${pkgPath} is not a valid extension manifest: ${e.error}`,
			);
			process.exit(1);
		}

		const manifest = e.data;
		const outDir = flags.out ?? join(extensionDataDir(), manifest.name);

		const doBuild = async (outDir: string) => {
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
					outdir: outDir,
					platform: "node",
					minify: true,
					loader: {
						".node": "file",
					},
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
		const check = typeCheck(src);

		if (!check.ok) {
			logger.logError(`Type check failed:\n${check.output}`);
			process.exit(1);
		}

		mkdirSync(outDir, { recursive: true });
		await doBuild(outDir);
		logger.logReady(`built extension successfully - output at ${outDir}`);
	},
};

export default build;
