import { readFileSync } from 'node:fs';

import { banner, type Provenance } from './emit.ts';

// Bit values match emoji::EmojiProp in src/emoji-properties.hpp.
export const EMOJI_PROP = {
	Emoji: 1,
	Emoji_Presentation: 2,
	Emoji_Modifier: 4,
	Emoji_Modifier_Base: 8,
} as const;

export type EmojiProperties = Map<number, number>;

export function parseEmojiProperties(path: string): EmojiProperties {
	const props: EmojiProperties = new Map();

	for (const raw of readFileSync(path, 'utf-8').split('\n')) {
		const line = raw.split('#')[0].trim();
		if (!line) continue;

		const [codes, property] = line.split(';').map((s) => s.trim());
		const flag = EMOJI_PROP[property as keyof typeof EMOJI_PROP];
		if (!flag) continue;

		const range = codes.match(/^([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?$/);
		if (!range) continue;

		const start = parseInt(range[1], 16);
		const end = range[2] ? parseInt(range[2], 16) : start;
		for (let cp = start; cp <= end; cp++) props.set(cp, (props.get(cp) ?? 0) | flag);
	}

	return props;
}

interface PropRange {
	lo: number;
	hi: number;
	flags: number;
}

function coalesce(props: EmojiProperties): PropRange[] {
	const ranges: PropRange[] = [];
	for (const cp of [...props.keys()].sort((a, b) => a - b)) {
		const flags = props.get(cp)!;
		const last = ranges[ranges.length - 1];
		if (last && last.flags === flags && cp === last.hi + 1) last.hi = cp;
		else ranges.push({ lo: cp, hi: cp, flags });
	}
	return ranges;
}

const hex = (cp: number) => `0x${cp.toString(16).toUpperCase().padStart(4, '0')}`;

const CPP_FLAG_NAMES: readonly [number, string][] = [
	[EMOJI_PROP.Emoji, 'Emoji'],
	[EMOJI_PROP.Emoji_Presentation, 'EmojiPresentation'],
	[EMOJI_PROP.Emoji_Modifier, 'EmojiModifier'],
	[EMOJI_PROP.Emoji_Modifier_Base, 'EmojiModifierBase'],
];

const flagExpr = (flags: number) =>
	CPP_FLAG_NAMES.filter(([bit]) => flags & bit)
		.map(([, name]) => name)
		.join(' | ');

export function emitEmojiProperties(props: EmojiProperties, provenance: Provenance): string {
	const ranges = coalesce(props);

	const out: string[] = [];
	out.push(banner(provenance));
	out.push('#include "emoji-properties.hpp"');
	out.push('');
	out.push('#include <algorithm>');
	out.push('#include <array>');
	out.push('');
	out.push('using enum emoji::EmojiProp;');
	out.push('');
	out.push('namespace {');
	out.push('');
	out.push('struct PropRange {');
	out.push('  char32_t lo;');
	out.push('  char32_t hi;');
	out.push('  std::uint8_t flags;');
	out.push('};');
	out.push('');
	out.push(`constexpr std::array<PropRange, ${ranges.length}> g_props = {{`);
	for (const r of ranges) out.push(`  {${hex(r.lo)}, ${hex(r.hi)}, ${flagExpr(r.flags)}},`);
	out.push('}};');
	out.push('');
	out.push('} // namespace');
	out.push('');
	out.push('std::uint8_t emoji::lookupProperties(char32_t cp) {');
	out.push('  auto it = std::upper_bound(');
	out.push('      g_props.begin(), g_props.end(), cp,');
	out.push('      [](char32_t key, const PropRange &r) { return key < r.lo; });');
	out.push('  if (it == g_props.begin()) return 0;');
	out.push('  --it;');
	out.push('  return cp <= it->hi ? it->flags : 0;');
	out.push('}');
	out.push('');

	return out.join('\n');
}
