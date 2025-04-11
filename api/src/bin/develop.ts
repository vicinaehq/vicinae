import chokidar from 'chokidar';
import esbuild from 'esbuild';
import { join } from 'path';
import { readFileSync } from 'fs';

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
};

const logger = new Logger();

const build = async () => {
	const pkg = JSON.parse(readFileSync("package.json", 'utf-8'));
	const outdir = "dist";
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
		  outfile: join(outdir, `${cmd.name}.js`),
		  format: 'cjs',
		  platform: 'node'
		});
	}
}

export const developExtension = (extensionPath: string) => {
	process.chdir(extensionPath);

	chokidar.watch('src', { ignoreInitial: true }).on('all', (event, path) => {
		logger.logEvent(`changed file ${path}`);
		build();
		logger.logReady('built extension successfully');
		// TODO: notify omnicast
	});
}

developExtension(process.argv[2] ?? process.cwd());
