import { readFileSync } from 'node:fs';

import { buildKeywords } from './keywords.ts';
import { lookupAnnotation, type Annotations } from './cldr.ts';
import { EMOJI_GROUP_CATEGORY, type Item } from './categories.ts';
import { EMOJI_PROP, type EmojiProperties } from './emoji-properties.ts';

const isSkinTone = (cp: number) => cp >= 0x1f3fb && cp <= 0x1f3ff;

export interface EmojiOptions {
	annotations: Annotations;
	properties: EmojiProperties;
}

// Keeps fully-qualified emoji in emoji-test order. Skin-tone variants are dropped; their base is
// flagged `skinnable` and tones are composed at runtime.
export function parseEmoji(emojiTestPath: string, opts: EmojiOptions): Item[] {
	const emojis: Item[] = [];

	let group = '';
	let subgroup = '';

	for (const raw of readFileSync(emojiTestPath, 'utf-8').split('\n')) {
		const groupMatch = raw.match(/^# group:\s*(.+)$/);
		if (groupMatch) {
			group = groupMatch[1].trim();
			continue;
		}
		const subgroupMatch = raw.match(/^# subgroup:\s*(.+)$/);
		if (subgroupMatch) {
			subgroup = subgroupMatch[1].trim();
			continue;
		}
		if (!raw || raw.startsWith('#')) continue;

		const [codes, rest] = raw.split(';');
		if (!rest) continue;
		if (rest.split('#')[0].trim() !== 'fully-qualified') continue;
		if (group === 'Component') continue;

		const codepoints = codes.trim().split(/\s+/).map((hex) => parseInt(hex, 16));
		if (codepoints.some(isSkinTone)) continue;

		const category = EMOJI_GROUP_CATEGORY[group];
		if (!category) continue;

		const glyph = String.fromCodePoint(...codepoints);
		const comment = raw.slice(raw.indexOf('#') + 1).trim();
		const testName = comment.match(/^\S+\s+E[0-9.]+\s+(.+)$/)?.[1]?.trim() ?? '';
		const annotation = lookupAnnotation(opts.annotations, glyph);
		const name = annotation?.tts?.[0] ?? testName;

		emojis.push({
			character: glyph,
			name,
			keywords: buildKeywords({
				displayName: name,
				unicodeName: testName,
				cldr: annotation?.default ?? [],
				aliases: [subgroup.replace(/-/g, ' ')],
			}),
			kind: 'Emoji',
			category,
			skinnable: codepoints.some((cp) => ((opts.properties.get(cp) ?? 0) & EMOJI_PROP.Emoji_Modifier_Base) !== 0),
		});
	}

	return emojis;
}
