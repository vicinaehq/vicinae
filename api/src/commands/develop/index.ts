import { Command, Flags } from "@oclif/core";
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
import { VicinaeClient } from "../../utils/vicinae.js";
import ManifestSchema from '../../schemas/manifest.js';

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

    if (!existsSync(pkgPath)) {
      logger.logError(
        `No package.json found at ${pkgPath}. Does this location point to a valid extension repository?`
      );
      process.exit(1);
    }

	const json = JSON.parse(readFileSync(pkgPath, 'utf8'));

	const e = ManifestSchema.safeParse(json);

	if (e.error) {
		logger.logError(`${pkgPath} is not a valid extension manifest: ${e.error}`);
		process.exit(1);
	}

	const manifest = e.data;


    const vicinae = new VicinaeClient();

    const typeCheck = async (): Promise<TypeCheckResult> => {
      const spawned = spawn("npx", ["tsc", "--noEmit"]);
      let stderr = Buffer.from("");

      return new Promise<TypeCheckResult>((resolve) => {
        spawned.stderr.on("data", (buf) => {
          stderr = Buffer.concat([stderr, buf]);
        });

        spawned.on("exit", (status) =>
          resolve({ error: stderr.toString(), ok: status === 0 })
        );
      });
    };

    const build = async (outDir: string) => {
      logger.logInfo("Started type checking in background thread");

      typeCheck().then(({ error, ok }) => {
        if (!ok) {
          logger.logInfo(`Type checking error: ${error}`);
        }

        logger.logInfo("Done type checking");
      });

      const entryPoints = manifest.commands.map((cmd) =>
        join("src", `${cmd.name}.tsx`)
      );

      logger.logInfo(`entrypoints [${entryPoints.join(", ")}]`);

      const promises = manifest.commands.map((cmd) => {
        const source = join(process.cwd(), "src", `${cmd.name}.tsx`);
        return esbuild.build({
          bundle: true,
          entryPoints: [source],
          external: ["react", "@vicinae/api"],
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

    process.chdir(flags.target);

    const dataDir = extensionDataDir();
    const id = `${manifest.name}.dev`;
    const extensionDir = join(dataDir, id);
    const logFile = join(extensionDir, "dev.log");
    const pidFile = join(extensionDir, "cli.pid");

    mkdirSync(extensionDir, { recursive: true });
    await build(extensionDir);
    logger.logReady("built extension successfully");
    writeFileSync(pidFile, `${process.pid}`);
    writeFileSync(logFile, "");

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
      .on("all", (_, path) => {
        logger.logEvent(`changed file ${path}:`);

        try {
          build(extensionDir);
          logger.logReady("built extension successfully");
          vicinae.refreshDevSession(id);
        } catch (error: unknown) {
          logger.logEvent(`Failed to build extension: ${error}`);
        }
      });

    const logFiles = new Map<string, LogFileData>();

    chokidar.watch(logFile).on("all", async (_, path) => {
      const stats = await stat(path);

      if (!stats.isFile()) return;

      if (!logFiles.has(path)) {
        logger.logInfo(`Monitoring new log file at ${path}`);
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
