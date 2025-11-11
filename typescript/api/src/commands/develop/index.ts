import { Command, CommandHelp, Flags } from "@oclif/core";
import * as chokidar from "chokidar";
import * as esbuild from "esbuild";
import { spawn } from "node:child_process";
import {
	cpSync,
	existsSync,
	mkdirSync,
	read,
	readFileSync,
	writeFileSync,
} from "node:fs";
import { open, stat } from "node:fs/promises";
import { join } from "node:path";
import { Logger } from "../../utils/logger.js";
import { extensionDataDir } from "../../utils/utils.js";
import { updateExtensionTypes } from "../../utils/extension-types.js";
import { VicinaeClient } from "../../utils/vicinae.js";
import ManifestSchema from "../../schemas/manifest.js";

type TypeCheckResult = {
	error: string;
	ok: boolean;
};

type LogFileData = {
	cursor: number;
	path: string;
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
		const pkgPath = join(flags.target, "package.json");
		const parseManifest = () => {
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
				.map((cmd) => join("src", `${cmd.name}.tsx`))
				.filter(existsSync);
			logger.logInfo(`entrypoints [${entryPoints.join(", ")}]`);

			const promises = manifest.commands.map((cmd) => {
				const base = join(process.cwd(), "src", `${cmd.name}`);
				const tsxSource = `${base}.tsx`;
				const tsSource = `${base}.ts`;
				let source = tsxSource;

				if (cmd.mode === "view" && !existsSync(tsxSource)) {
					throw new Error(
						`could not find entrypoint src/${cmd.name}.tsx for command ${cmd.name}.`,
					);
				}

				// we allow .ts or .tsx for no-view
				if (cmd.mode === "no-view") {
					if (!existsSync(tsxSource)) {
						source = tsSource;
						if (!existsSync(tsSource)) {
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
					outfile: join(outDir, `${cmd.name}.js`),
					platform: "node",
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
		const extensionDir = join(dataDir, id);
		const logFile = join(extensionDir, "dev.log");
		const pidFile = join(extensionDir, "cli.pid");

		mkdirSync(extensionDir, { recursive: true });
		writeFileSync(pidFile, `${process.pid}`);
		writeFileSync(logFile, "");
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

		const logFiles = new Map<string, LogFileData>();

		chokidar.watch(logFile).on("all", async (_, path) => {
			const stats = await stat(path);

			if (!stats.isFile()) return;

			if (!logFiles.has(path)) {
				//logger.logInfo(`Monitoring new log file at ${path}`);
				logFiles.set(path, { cursor: 0, path });
			}

			const info = logFiles.get(path)!;

			if (info.cursor > stats.size) {
				info.cursor = 0;
			}

			if (stats.size === info.cursor) return;

			const handle = await open(path, "r");
			const buffer = Buffer.alloc(stats.size - info.cursor);

			read(handle.fd, buffer, 0, buffer.length, info.cursor, (error, nRead) => {
				if (error) return;

				info.cursor += nRead;
				logger.logTimestamp(buffer.toString());
				handle.close();
			});
		});
	}
}
