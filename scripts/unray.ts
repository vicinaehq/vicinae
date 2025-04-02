import { spawnSync } from "child_process";
import { PathLike, cp, cpSync, readFileSync, readdirSync, rmSync, statSync, writeFileSync } from "fs";
import { basename, join } from 'path';

/**
 * Attempts to convert a raycast extension to an omnicast extension
 */

const traverse = (root: string, cb: (path: string) => void) => {
	const queue = [root];

	while (queue.length > 0) {
		const dir = queue.pop()!;

		for (const entry of readdirSync(dir)) {
			const path = join(dir, entry);
			const stat = statSync(path);

			if (stat.isDirectory()) {
				queue.push(path);
			} else {
				cb(path);
			}
		}
	}
}

const installLocalApiUtils = (utilsPath: string, destPath: string) => {
	spawnSync('npx', ['tsc', '--outDir', 'dist'], { cwd: utilsPath });
	spawnSync('npm', ['pack'], { cwd: utilsPath });
	const name = join(utilsPath, 'omnicast-utils-1.0.0.tgz');
	spawnSync('npm', ['install', name], { cwd: destPath });
};

const installLocalApi = (apiPath: string, destPath: string) => {
	const build = spawnSync('npx', ['tsc', '--outDir', 'dist'], { cwd: apiPath });
	spawnSync('npm', ['pack'], { cwd: apiPath });
	const name = join(apiPath, 'omnicast-api-1.0.0.tgz');
	spawnSync('npm', ['install', name], { cwd: destPath });
};

const substituteImports = (path: string) => {
	const data = readFileSync(path, 'utf8');
	const final = data.replace(/@raycast\//g, '@omnicast/');

	console.log(`substituted imports for file ${path}`);
	writeFileSync(path, final);
}

const unray = (source: string, dest: string) => {
	const pkg = JSON.parse(readFileSync(join(source, "package.json"), 'utf8'));

	for (const key of Object.keys(pkg.dependencies)) {
		if (key.startsWith('@raycast/')) {
			delete pkg.dependencies[key];
		}
	}

	for (const key of Object.keys(pkg.devDependencies)) {
		if (key == "@types/react") {
			pkg.devDependencies[key] = '^19.0.0';
		}
	}


	rmSync(dest, { recursive: true });
	//cpSync(join(__dirname, "..", "extension-boilerplate"), dest, { recursive: true });

	cpSync(source, dest, { recursive: true, filter: (path) => !path.endsWith("node_modules") });

	writeFileSync(join(dest, 'package.json'), JSON.stringify(pkg, null, 2));

	const targetSourceDir = join(dest, "src");

	traverse(targetSourceDir, substituteImports);

	installLocalApi(join(__dirname, '..', 'api'), dest);
	installLocalApiUtils(join(__dirname, '..', 'api-utils'), dest);

	//console.log(pkg);
}

if (process.argv.length != 4) {
	console.error(`${basename(process.argv[1])} <path/to/raycast/extension> <target>`);
	process.exit(0);
}

unray(process.argv[2], process.argv[3]);
