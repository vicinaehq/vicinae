import { readFileSync } from 'node:fs';

const USEFUL_TYPES = new Set(['abbreviation', 'correction', 'alternate']);

export function parseNameAliases(path: string): Map<number, string[]> {
	const aliases = new Map<number, string[]>();

	for (const raw of readFileSync(path, 'utf-8').split('\n')) {
		const line = raw.trim();
		if (!line || line.startsWith('#')) continue;

		const [cp, alias, type] = line.split(';');
		if (!alias || !USEFUL_TYPES.has(type)) continue;

		const codepoint = parseInt(cp, 16);
		if (Number.isNaN(codepoint)) continue;

		const list = aliases.get(codepoint);
		if (list) list.push(alias);
		else aliases.set(codepoint, [alias]);
	}

	return aliases;
}
