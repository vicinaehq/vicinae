import { copyFileSync, mkdirSync, readFileSync, writeFileSync } from 'fs';
import { homedir } from 'os';
import { join } from 'path';

const EXT_BASE = join(homedir(), '.local', 'share', 'omnicast', 'extensions', 'installed');

const pkg = JSON.parse(readFileSync('./package.json', 'utf-8'));

mkdirSync(EXT_BASE, { recursive: true });
mkdirSync(join(EXT_BASE, pkg.name), { recursive: true });

copyFileSync('./package.json', join(EXT_BASE, pkg.name, 'package.json'));

for (const command of pkg.commands) {
	copyFileSync(`dist/${command.name}.js`, join(EXT_BASE, pkg.name, `${command.name}.js`));
}
