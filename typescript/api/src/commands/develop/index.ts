import { Command, Flags } from "@oclif/core";
import * as chokidar from "chokidar";
import * as esbuild from "esbuild";
import { spawn } from "node:child_process";
import * as fs from "node:fs";
import * as fsp from "node:fs/promises";
import * as path from "node:path";
import { Logger } from "../../utils/logger.js";
import {
	extensionDataDir,
	extensionInternalSupportDir,
} from "../../utils/utils.js";
import { updateExtensionTypes } from "../../utils/extension-types.js";
import { VicinaeClient } from "../../utils/vicinae.js";
import ManifestSchema from "../../schemas/manifest.js";
import { Tail } from "../../utils/tail.js";

type TypeCheckResult = {
	error: string;
	ok: boolean;
};

export default class Develop extends Command {
	static args = {};
	static description = "Start an extension development session";
	static examples = [
		`<%= config.bin %> <%= command.id %> --target /path/to/extension`,
	];
	static flags = {
		target: Flags.string({
			aliases: ["input"],
			char: "i",
			default: process.cwd(),
			defaultHelp: "The current working directory",
			description: "Path to the extension directory",
			required: false,
		}),
	};

	async run(): Promise<void> {
		const { flags } = await this.parse(Develop);
		const logger = new Logger();
		const pkgPath = path.join(flags.target, "package.json");
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
		updateExtensionTypes(manifest, flags.target);

		const typeCheck = async (): Promise<TypeCheckResult> => {
			const spawned = spawn("npx", ["tsc", "--noEmit"]);
			let stderr = Buffer.from("");

			return new Promise<TypeCheckResult>((resolve) => {
				spawned.stderr.on("data", (buf) => {
					stderr = Buffer.concat([stderr, buf]);
				});

				spawned.on("exit", (status) =>
					resolve({ error: stderr.toString(), ok: status === 0 }),
				);
			});
		};

		const build = async (outDir: string) => {
			/*
	  logger.logInfo("Started type checking in background thread");
	  typeCheck().then(({ error, ok }) => {
		if (!ok) {
		  logger.logInfo(`Type checking error: ${error}`);
		}

		logger.logInfo("Done type checking");
	  });
	  */

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

				// we allow .ts or .tsx for no-view
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
					outfile: path.join(outDir, `${cmd.name}.js`),
					platform: "node",
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

		const pingError = vicinae.ping();

		if (pingError) {
			console.error(`Failed to ping vicinae\n`, pingError.message);
			return;
		}

		const safeBuild = async (extensionDir: string) => {
			try {
				const start = performance.now();
				await build(extensionDir);
				const time = performance.now() - start;
				logger.logReady(`Extension built in ${Math.round(time)}ms 🚀`);
				vicinae.refreshDevSession(id);
			} catch (error: unknown) {
				if (error instanceof Error) {
					logger.logError(`Failed to build extension: ${error.message}`);
				} else {
					logger.logError(`Failed to build extension: ${error}`);
				}
			}
		};

		process.chdir(flags.target);

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
		outStream.on("error", () => { });

		errStream.on("line", (data) => {
			logger.logExtensionError(data.toString());
		});
		errStream.on("error", () => { });

		await safeBuild(extensionDir);

		process.on("SIGINT", () => {
			logger.logInfo("Shutting down...");
			vicinae.stopDevSession(id);
			throw new Error(`Development session interrupted`);
		});

		const error = vicinae.startDevSession(id);

		if (error) {
			console.error(`Failed to invoke vicinae`, error);
			return;
		}

		chokidar
			.watch(["src", "package.json", "assets"], {
				awaitWriteFinish: { pollInterval: 100, stabilityThreshold: 100 },
				ignoreInitial: true,
			})
			.on("all", async (_, path) => {
				if (path.endsWith("package.json")) {
					manifest = parseManifest();
					logger.logInfo("Generating extension types...");
					updateExtensionTypes(manifest, flags.target);
				}

				logger.logEvent(`changed file ${path}`);
				await safeBuild(extensionDir);
			});
	}
}
