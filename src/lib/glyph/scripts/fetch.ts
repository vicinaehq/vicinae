// Downloads the Unicode / Emoji / CLDR sources into ./assets and records their versions.
// UNICODE_VERSION=17.0.0 pins a UCD release (default: latest); CLDR_REF=48.2.0 pins a cldr-json ref (default: main).
import { mkdirSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const ASSETS = join(dirname(fileURLToPath(import.meta.url)), 'assets');

const unicodeVersion = process.env.UNICODE_VERSION ?? 'latest';
const cldrRef = process.env.CLDR_REF ?? 'main';

const ucdRoot = `https://www.unicode.org/Public/UCD/${unicodeVersion}/ucd`;
const emojiVersion = unicodeVersion === 'latest' ? 'latest' : unicodeVersion.split('.').slice(0, 2).join('.');
const emojiRoot = `https://www.unicode.org/Public/emoji/${emojiVersion}`;
const cldrRoot = `https://raw.githubusercontent.com/unicode-org/cldr-json/${cldrRef}/cldr-json`;

const FILES: Record<string, string> = {
	'UnicodeData.txt': `${ucdRoot}/UnicodeData.txt`,
	'Blocks.txt': `${ucdRoot}/Blocks.txt`,
	'NamesList.txt': `${ucdRoot}/NamesList.txt`,
	'NameAliases.txt': `${ucdRoot}/NameAliases.txt`,
	'emoji-data.txt': `${ucdRoot}/emoji/emoji-data.txt`,
	'emoji-test.txt': `${emojiRoot}/emoji-test.txt`,
	'cldr-annotations.json': `${cldrRoot}/cldr-annotations-full/annotations/en/annotations.json`,
	'cldr-annotations-derived.json': `${cldrRoot}/cldr-annotations-derived-full/annotationsDerived/en/annotations.json`,
};

async function download(url: string): Promise<string> {
	const res = await fetch(url);
	if (!res.ok) throw new Error(`${url}: HTTP ${res.status}`);
	return res.text();
}

mkdirSync(ASSETS, { recursive: true });

const contents: Record<string, string> = {};
await Promise.all(
	Object.entries(FILES).map(async ([name, url]) => {
		contents[name] = await download(url);
		writeFileSync(join(ASSETS, name), contents[name]);
		console.log(`fetched ${name.padEnd(30)} ${url}`);
	}),
);

const unicode = contents['Blocks.txt'].match(/^# Blocks-([\d.]+)\.txt/m)?.[1] ?? unicodeVersion;
const emoji = contents['emoji-test.txt'].match(/^# Version:\s*([\d.]+)/m)?.[1] ?? 'unknown';
const cldr = JSON.parse(await download(`${cldrRoot}/cldr-annotations-full/package.json`)).version as string;

writeFileSync(join(ASSETS, 'manifest.json'), JSON.stringify({ unicode, emoji, cldr, sources: FILES }, null, 2) + '\n');
console.log(`Unicode ${unicode}, Emoji ${emoji}, CLDR ${cldr}`);
