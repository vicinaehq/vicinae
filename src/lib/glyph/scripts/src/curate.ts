import { blockKey } from './blocks.ts';
import { buildKeywords } from './keywords.ts';
import { lookupAnnotation, type Annotations } from './cldr.ts';
import type { Item, SymbolCategory } from './categories.ts';
import type { UcdEntry } from './unicode-data.ts';

interface BlockRule {
	block: string;
	category: SymbolCategory;
	/** Only keep these general categories. */
	include?: string[];
	/** Per-general-category category override. */
	byGc?: Partial<Record<string, SymbolCategory>>;
	/** Only keep codepoints whose name contains one of these. */
	nameKeep?: string[];
}

const RULES: BlockRule[] = [
	{ block: 'Mathematical Operators', category: 'Math' },
	{ block: 'Supplemental Mathematical Operators', category: 'Math' },
	{ block: 'Miscellaneous Mathematical Symbols-A', category: 'Math' },
	{ block: 'Miscellaneous Mathematical Symbols-B', category: 'Math' },

	{ block: 'Arrows', category: 'Arrows' },
	{ block: 'Supplemental Arrows-A', category: 'Arrows' },
	{ block: 'Supplemental Arrows-B', category: 'Arrows' },
	{ block: 'Supplemental Arrows-C', category: 'Arrows' },

	{ block: 'Currency Symbols', category: 'Currency' },

	{ block: 'General Punctuation', category: 'Punctuation' },
	{ block: 'Supplemental Punctuation', category: 'Punctuation' },

	{ block: 'Geometric Shapes', category: 'Shapes' },
	{ block: 'Geometric Shapes Extended', category: 'Shapes' },
	{ block: 'Box Drawing', category: 'Shapes' },
	{ block: 'Block Elements', category: 'Shapes' },

	{ block: 'Miscellaneous Symbols', category: 'MiscSymbols' },
	{ block: 'Dingbats', category: 'MiscSymbols' },
	{ block: 'Ornamental Dingbats', category: 'MiscSymbols' },
	{ block: 'Miscellaneous Technical', category: 'MiscSymbols' },
	{ block: 'Miscellaneous Symbols and Arrows', category: 'MiscSymbols' },

	{
		block: 'Basic Latin',
		category: 'Punctuation',
		include: ['Po', 'Pd', 'Ps', 'Pe', 'Pc', 'Sk', 'Sm', 'Sc'],
		byGc: { Sc: 'Currency', Sm: 'Math' },
	},
	{
		block: 'Latin-1 Supplement',
		category: 'MiscSymbols',
		include: ['Sc', 'Sm', 'Sk', 'So', 'Po', 'Pd', 'Pi', 'Pf', 'No'],
		byGc: { Sc: 'Currency', No: 'NumberForms' },
	},

	{ block: 'Greek and Coptic', category: 'Greek', include: ['Lu', 'Ll', 'Lt', 'Lm', 'Lo'] },

	{ block: 'Number Forms', category: 'NumberForms' },
	{ block: 'Superscripts and Subscripts', category: 'NumberForms' },

	{
		block: 'Mathematical Alphanumeric Symbols',
		category: 'FancyLetters',
		nameKeep: ['DOUBLE-STRUCK', 'FRAKTUR', 'SCRIPT'],
	},
];

const RULE_BY_BLOCK = new Map(RULES.map((rule) => [blockKey(rule.block), rule]));

const INVISIBLE_GC = new Set(['Cc', 'Cf', 'Cs', 'Co', 'Cn', 'Zs', 'Zl', 'Zp']);

function resolveCategory(blockName: string | undefined, gc: string, name: string): SymbolCategory | null {
	const rule = blockName && RULE_BY_BLOCK.get(blockName);
	if (!rule) return null;
	if (rule.include && !rule.include.includes(gc)) return null;
	if (rule.nameKeep && !rule.nameKeep.some((sub) => name.includes(sub))) return null;
	return rule.byGc?.[gc] ?? rule.category;
}

export interface BuildOptions {
	resolveBlock: (cp: number) => string | undefined;
	annotations: Annotations;
	namesList: Map<number, string[]>;
	nameAliases: Map<number, string[]>;
}

export function buildSymbols(entries: UcdEntry[], opts: BuildOptions): Item[] {
	const symbols: Item[] = [];

	for (const entry of entries) {
		if (INVISIBLE_GC.has(entry.gc)) continue;

		const category = resolveCategory(opts.resolveBlock(entry.codepoint), entry.gc, entry.name);
		if (!category) continue;

		const annotation = lookupAnnotation(opts.annotations, entry.symbol);
		const displayName = annotation?.tts?.[0] ?? entry.name.toLowerCase();

		symbols.push({
			character: entry.symbol,
			name: displayName,
			keywords: buildKeywords({
				displayName,
				unicodeName: entry.name,
				cldr: annotation?.default ?? [],
				aliases: [...(opts.namesList.get(entry.codepoint) ?? []), ...(opts.nameAliases.get(entry.codepoint) ?? [])],
			}),
			kind: 'Symbol',
			category,
			skinnable: false,
		});
	}

	return symbols;
}
