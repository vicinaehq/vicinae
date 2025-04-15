import chokidar from 'chokidar';
import esbuild from 'esbuild';
import { join } from 'path';
import {  cpSync, existsSync, mkdirSync, read, readFileSync, writeFileSync } from 'fs';
import { open, stat } from 'fs/promises';
import { spawn, spawnSync } from 'child_process';
import { extensionDataDir } from './utils';
import { pid } from 'process';

const OMNICAST_BIN = "/home/aurelle/prog/perso/omnicast/build/omnicast/omnicast";

const invokeOmnicast = (endpoint: string): Error | null => {
	const url = new URL(endpoint, 'omnicast://');
	const result = spawnSync(OMNICAST_BIN, [url.toString()]);

	if (result.error) return result.error;

	return result.status === 0 ? null : new Error(result.stderr.toString());
}

class Logger {
	prefixes = {
		'info': '\x1b[36minfo\x1b[0m',
		'event': '\x1b[35mevent\x1b[0m',
		'ready': '\x1b[32mready\x1b[0m',
	};

	constructor() {}

	logInfo(message: string) {
		console.log(`${this.prefixes.info.padEnd(15)} - ${message}`);
	}

	logEvent(message: string) {
		console.log(`${this.prefixes.event.padEnd(15)} - ${message}`);
	}

	logReady(message: string) {
		console.log(`${this.prefixes.ready.padEnd(15)} - ${message}`);
	}

	logTimestamp(s: string) {
		const ts = (new Date).toJSON();
		const lines = s.split('\n');

		for (let i = 0; i !== lines.length; ++i) {
			const line = lines[i];

			if (i === lines.length - 1 && line.length === 0) continue ;

			console.log(`\x1b[90m${ts.padEnd(20)}\x1b[0m - ${line}`);
		}
	}
};

const logger = new Logger();

type TypeCheckResult = {
	ok: boolean;
	error: string;
}

const typeCheck = async (): Promise<TypeCheckResult> => {
	const spawned = spawn('npx', ['tsc', '--noEmit']);
	let stderr = Buffer.from('');

	return new Promise<TypeCheckResult>((resolve) => {
		spawned.stderr.on('data', (buf) => {
			stderr = Buffer.concat([stderr, buf]);
		});

		spawned.on('exit', (status) => resolve({ error: stderr.toString(), ok: status === 0 }));
	});
}

const build = async (outDir: string) => {
	logger.logInfo("Started type checking in background thread");

	typeCheck().then(({ ok, error }) => {
		if (!ok) {
			logger.logInfo(`Type checking error: ${error}`);
		}
		logger.logInfo("Done type checking");
	});

	const pkg = JSON.parse(readFileSync("package.json", 'utf-8'));
	const entryPoints = pkg.commands.map(cmd => join('src', `${cmd.name}.tsx`));

	logger.logInfo(`entrypoints [${entryPoints.join(', ')}]`);

    for (const cmd of pkg.commands) {
		const source = join(process.cwd(), 'src', `${cmd.name}.tsx`);

		await esbuild.build({
		  entryPoints: [source],
		  bundle: true,
		  external: [
			  "react",
			  "@omnicast/api"
		  ],
		  outfile: join(outDir, `${cmd.name}.js`),
		  format: 'cjs',
		  platform: 'node',
		});
	}

	const targetPkg = join(outDir, 'package.json');
	const targetAssets = join(outDir, 'assets');

	cpSync("package.json", targetPkg, { force: true });

	if (existsSync("assets")) {
		cpSync("assets", targetAssets, { recursive: true, force: true });
	} else {
		mkdirSync(targetAssets, { recursive: true });
	}
}

type LogFileData = {
	path: string;
	cursor: number;
};

export const developExtension = (extensionPath: string) => {
	process.chdir(extensionPath);
	const pkg = JSON.parse(readFileSync("package.json", 'utf-8'));
	const dataDir = extensionDataDir();
	const id = `${pkg.name}.dev`;
	const extensionDir = join(dataDir, id);
	const logFile = join(extensionDir, "dev.log");
	const pidFile = join(extensionDir, "cli.pid");

	mkdirSync(extensionDir, { recursive: true });
	build(extensionDir);
	logger.logReady('built extension successfully');
	writeFileSync(pidFile, `${pid}`)
	writeFileSync(logFile, '');

	process.on('SIGINT', () => {
		logger.logInfo('Shutting down...');
		process.exit(0);
	});

	const error = invokeOmnicast(`/api/extensions/develop/start?id=${id}`);

	if (error) {
		console.error(`Failed to invoke omnicast`, error);
		return ;
	}

	chokidar.watch(['src', 'package.json', 'assets'], { ignoreInitial: true, awaitWriteFinish: { stabilityThreshold: 100, pollInterval: 100 } }).on('all', (event, path) => {
		logger.logEvent(`changed file ${path}:`);
		build(extensionDir);
		logger.logReady('built extension successfully');
		invokeOmnicast(`/api/extensions/develop/refresh?id=${id}`);
	});

	const logFiles = new Map<string, LogFileData>();

	chokidar.watch(logFile).on('all', async (event, path) => {
		const stats = await stat(path);

		if (!stats.isFile()) return ;

		if (!logFiles.has(path)) {
			logger.logInfo(`Monitoring new log file at ${path}`);
			logFiles.set(path, { path, cursor: 0 });
		}

		const info = logFiles.get(path)!;
		
		if (info.cursor > stats.size) {
			info.cursor = 0;
		}

		if (stats.size == info.cursor) return ;

		const handle = await open(path, 'r');
		const buffer = Buffer.alloc(stats.size - info.cursor);

		read(handle.fd, buffer, 0, buffer.length, info.cursor, (error, nRead) => {
			if (error) return ;

			info.cursor += nRead;
			logger.logTimestamp(buffer.toString());
			handle.close();
		});
	});
}
