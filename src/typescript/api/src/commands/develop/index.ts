import * as chokidar from "chokidar";
import * as esbuild from "esbuild";
import * as fs from "node:fs";
import * as fsp from "node:fs/promises";
import * as path from "node:path";
import type { CommandDef } from "../../cli.js";
import ManifestSchema from "../../schemas/manifest.js";
import { updateExtensionTypes } from "../../utils/extension-types.js";
import { Logger } from "../../utils/logger.js";
import { Tail } from "../../utils/tail.js";
import { typeCheck } from "../../utils/typecheck.js";
import {
	extensionDataDir,
	extensionInternalSupportDir,
} from "../../utils/utils.js";
import { VicinaeClient } from "../../utils/vicinae.js";

const develop: CommandDef = {
	description: "Start an extension development session",
	flags: {
		target: {
			short: "i",
			description: "Path to the extension directory",
			default: process.cwd(),
		},
	},

	async run(flags) {
		const logger = new Logger();
		const target = flags.target ?? process.cwd();
		const pkgPath = path.join(target, "package.json");
		const parseManifest = () => {
			if (!fs.existsSync(pkgPath)) {
				logger.logError(
					`No package.json found at ${pkgPath}. Does this location point to a valid extension repository?`,
				);
				process.exit(1);
			}

			const json = JSON.parse(fs.readFileSync(pkgPath, "utf8"));
			const e = ManifestSchema.safeParse(json);

			if (e.error) {
				logger.logError(
					`${pkgPath} is not a valid extension manifest: ${e.error}`,
				);
				process.exit(1);
			}

			return e.data;
		};

		let manifest = parseManifest();
		const vicinae = new VicinaeClient();

		logger.logInfo("Generating extension types...");
		updateExtensionTypes(manifest, target);

		const build = async (outDir: string) => {
			const entryPoints = manifest.commands
				.map((cmd) => path.join("src", `${cmd.name}.tsx`))
				.filter(fs.existsSync);
			logger.logInfo(`entrypoints [${entryPoints.join(", ")}]`);

			const promises = manifest.commands.map((cmd) => {
				const base = path.join(process.cwd(), "src", `${cmd.name}`);
				const tsxSource = `${base}.tsx`;
				const tsSource = `${base}.ts`;
				let source = tsxSource;

				if (cmd.mode === "view" && !fs.existsSync(tsxSource)) {
					throw new Error(
						`could not find entrypoint src/${cmd.name}.tsx for command ${cmd.name}.`,
					);
				}

				if (cmd.mode === "no-view") {
					if (!fs.existsSync(tsxSource)) {
						source = tsSource;
						if (!fs.existsSync(tsSource)) {
							throw new Error(
								`could not find entrypoint src/${cmd.name}.{ts,tsx} for command ${cmd.name}.`,
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
					loader: {
						".node": "file",
					},
				});
			});

			await Promise.all(promises);

			const targetPkg = path.join(outDir, "package.json");
			const targetAssets = path.join(outDir, "assets");

			fs.cpSync("package.json", targetPkg, { force: true });

			if (fs.existsSync("assets")) {
				fs.cpSync("assets", targetAssets, { force: true, recursive: true });
			} else {
				fs.mkdirSync(targetAssets, { recursive: true });
			}
		};

		try {
			await vicinae.ping();
		} catch (error) {
			console.error(
				`Failed to ping vicinae\n`,
				error instanceof Error ? error.message : error,
			);
			return;
		}

		const safeBuild = async (extensionDir: string) => {
			try {
				const start = performance.now();
				await build(extensionDir);
				const time = performance.now() - start;
				logger.logReady(`Extension built in ${Math.round(time)}ms 🚀`);
				await vicinae.refreshDevSession(id).catch((error: unknown) => {
					logger.logError(
						`Failed to refresh dev session: ${error instanceof Error ? error.message : error}`,
					);
				});

				const check = typeCheck(target);
				if (!check.ok) logger.logError(`Type errors:\n${check.output}`);
			} catch (error: unknown) {
				if (error instanceof Error) {
					logger.logError(`Failed to build extension: ${error.message}`);
				} else {
					logger.logError(`Failed to build extension: ${error}`);
				}
			}
		};

		process.chdir(target);

		const dataDir = extensionDataDir();
		const id = `${manifest.name}`;
		const extensionDir = path.join(dataDir, id);
		const internalSupportDir = extensionInternalSupportDir(id);
		const stdoutPath = path.join(internalSupportDir, "stdout.txt");
		const stderrPath = path.join(internalSupportDir, "stderr.txt");
		const pidFile = path.join(internalSupportDir, "cli.pid");

		await Promise.all(
			[extensionDir, internalSupportDir].map((dir) =>
				fsp.mkdir(dir, { recursive: true }),
			),
		);
		await fsp.writeFile(pidFile, `${process.pid}`);

		const outStream = new Tail(stdoutPath, { forceCreate: true, nLines: 0 });
		const errStream = new Tail(stderrPath, { forceCreate: true, nLines: 0 });

		outStream.on("line", (data) => {
			logger.logExtensionOut(data.toString());
		});
		outStream.on("error", () => {});

		errStream.on("line", (data) => {
			logger.logExtensionError(data.toString());
		});
		errStream.on("error", () => {});

		await safeBuild(extensionDir);

		process.on("SIGINT", async () => {
			logger.logInfo("Shutting down...");
			await vicinae.stopDevSession(id).catch(() => {});
			process.exit(0);
		});

		try {
			await vicinae.startDevSession(id);
		} catch (error) {
			console.error(`Failed to invoke vicinae`, error);
			return;
		}

		chokidar
			.watch(["src", "package.json", "assets"], {
				awaitWriteFinish: { pollInterval: 100, stabilityThreshold: 100 },
				ignoreInitial: true,
			})
			.on("all", async (_, filePath) => {
				if (filePath.endsWith("package.json")) {
					manifest = parseManifest();
					logger.logInfo("Generating extension types...");
					updateExtensionTypes(manifest, target);
				}

				logger.logEvent(`changed file ${filePath}`);
				await safeBuild(extensionDir);
			});
	},
};

export default develop;
