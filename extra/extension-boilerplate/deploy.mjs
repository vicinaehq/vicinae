import { copyFileSync, cpSync, existsSync, mkdirSync, readFileSync, rmSync, statSync } from 'fs';
import { homedir } from 'os';
import { join } from 'path';
import { cwd } from 'process';

const EXT_BASE = join(homedir(), '.local', 'share', 'omnicast', 'extensions', 'installed');
const assetDir = join(cwd(), "assets");
const pkg = JSON.parse(readFileSync('./package.json', 'utf-8'));

mkdirSync(EXT_BASE, { recursive: true });
rmSync(join(EXT_BASE, pkg.name), { recursive: true });
mkdirSync(join(EXT_BASE, pkg.name), { recursive: true });
copyFileSync('./package.json', join(EXT_BASE, pkg.name, 'package.json'));

const installDir = join(EXT_BASE, pkg.name);
const installAssetDir = join(installDir, "assets");

if (existsSync(assetDir) && statSync(assetDir).isDirectory()) {
	cpSync(assetDir, installAssetDir, { recursive: true });
}

for (const command of pkg.commands) {
	copyFileSync(`dist/${command.name}.js`, join(installDir, `${command.name}.js`));
}
