import { readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

import { parseUnicodeData } from './src/unicode-data.ts';
import { parseBlocks, makeBlockResolver } from './src/blocks.ts';
import { loadAnnotations } from './src/cldr.ts';
import { parseNamesList } from './src/nameslist.ts';
import { parseNameAliases } from './src/name-aliases.ts';
import { parseEmoji } from './src/emoji.ts';
import { parseEmojiProperties, emitEmojiProperties } from './src/emoji-properties.ts';
import { buildSymbols } from './src/curate.ts';
import { CATEGORIES, type Item } from './src/categories.ts';
import { emitCpp, emitHpp, type Provenance } from './src/emit.ts';

function validate(all: Item[]): void {
	const seen = new Set<string>();
	for (const item of all) {
		if (seen.has(item.character)) throw new Error(`duplicate glyph: ${JSON.stringify(item.character)}`);
		seen.add(item.character);
	}
	const present = new Set(all.map((i) => i.category));
	for (const { category } of CATEGORIES) {
		if (!present.has(category)) throw new Error(`empty category: ${category}`);
	}
	const declared = new Set(CATEGORIES.map((c) => c.category));
	for (const item of all) {
		if (!declared.has(item.category)) throw new Error(`undeclared category: ${item.category}`);
	}
}

const SCRIPTS_DIR = dirname(fileURLToPath(import.meta.url));
const LIB_DIR = join(SCRIPTS_DIR, '..');
const asset = (name: string) => join(SCRIPTS_DIR, 'assets', name);

const manifest = JSON.parse(readFileSync(asset('manifest.json'), 'utf-8'));
const provenance: Provenance = { unicode: manifest.unicode, emoji: manifest.emoji, cldr: manifest.cldr };

const entries = parseUnicodeData(asset('UnicodeData.txt'));
const blocks = parseBlocks(asset('Blocks.txt'));
const annotations = loadAnnotations(asset('cldr-annotations.json'), asset('cldr-annotations-derived.json'));
const namesList = parseNamesList(asset('NamesList.txt'));
const nameAliases = parseNameAliases(asset('NameAliases.txt'));
const emojiProps = parseEmojiProperties(asset('emoji-data.txt'));

let symbols = buildSymbols(entries, { resolveBlock: makeBlockResolver(blocks), annotations, namesList, nameAliases });
const emojis = parseEmoji(asset('emoji-test.txt'), { annotations, properties: emojiProps });

// Emoji win glyph collisions (e.g. ⌛ is both a curated symbol and an emoji): the lookup index must be unique.
const emojiGlyphs = new Set(emojis.map((e) => e.character));
const collisions = symbols.filter((s) => emojiGlyphs.has(s.character)).length;
symbols = symbols.filter((s) => !emojiGlyphs.has(s.character));

const all = [...emojis, ...symbols];
validate(all);

const outputs: Record<string, string> = {
	'include/glyph/glyph.hpp': emitHpp(provenance),
	'src/glyph.cpp': emitCpp(symbols, emojis, provenance),
	'src/emoji-properties.cpp': emitEmojiProperties(emojiProps, provenance),
};
for (const [rel, content] of Object.entries(outputs)) writeFileSync(join(LIB_DIR, rel), content);

const byCategory = new Map<string, number>();
for (const item of all) byCategory.set(item.category, (byCategory.get(item.category) ?? 0) + 1);
const skinnable = emojis.filter((e) => e.skinnable).length;

console.log(`Unicode ${provenance.unicode}, Emoji ${provenance.emoji}, CLDR ${provenance.cldr}`);
console.log(`generated ${all.length} items (${emojis.length} emoji, ${symbols.length} symbols) -> src/glyph.cpp`);
console.log(`  dropped ${collisions} symbols colliding with emoji glyphs; ${skinnable} skinnable`);
console.log(`generated ${emojiProps.size} emoji-property codepoints -> src/emoji-properties.cpp`);
for (const { category, kind, label } of CATEGORIES) {
	console.log(`  ${label.padEnd(20)} ${String(byCategory.get(category) ?? 0).padStart(5)}  (${kind})`);
}
