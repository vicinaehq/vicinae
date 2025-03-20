const { spawnSync } = require('child_process');
const { mkdirSync, existsSync, readdirSync, rmSync, copyFileSync } = require('fs');
const { join } = require('path');
const DEVICON_REPOSITORY = 'https://github.com/devicons/devicon';
const OMNI_ICON_DIR = join("..", "omnicast", "icons");

const clone = (url, dest) => {
	const result = spawnSync('git', ['clone', url, dest], { stdio: 'inherit' });

	return !result.error;
}

const tmpDir = ".tmp";
const deviconDir = join(tmpDir, "devicon");

if (!existsSync(deviconDir)) {
	mkdirSync(tmpDir, { recursive: true });

	if (!clone(DEVICON_REPOSITORY, join(".tmp", "devicon"))) {
		throw new Error('Failed to clone devicon repository');
	}
}

const iconDir = join(deviconDir, "icons");

for (const brand of readdirSync(iconDir)) {
	const brandDir = join(iconDir, brand);
	const candidates = readdirSync(brandDir).filter((s) => s.endsWith(".svg"));

	if (candidates.length === 0) {
		console.error(`No fitting icon for brand ${brand}`);
		continue ;
	}

	let match = null;

	match = candidates.find(v => v.endsWith("plain.svg"));

	if (!match) {
		match = candidates.find(v => v.endsWith("original.svg"));
	}

	if (!match) {
		match = candidates[0];
	}

	const matchPath = join(brandDir, match);
	const destPath = join(OMNI_ICON_DIR, `${brand}.svg`);

	copyFileSync(matchPath, destPath);
	console.log(`=> ${destPath}`);
}

//rmSync(tmpDir, { recursive: true }); 
