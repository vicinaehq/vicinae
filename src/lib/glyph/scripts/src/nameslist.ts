import { readFileSync } from 'node:fs';

// Collects the `= synonym` and `% formal alias` lines of each record. Cross-references (`x`) are
// skipped: they point to related-but-different characters.
export function parseNamesList(path: string): Map<number, string[]> {
	const aliases = new Map<number, string[]>();
	let current: number | null = null;

	for (const line of readFileSync(path, 'utf-8').split('\n')) {
		const header = line.match(/^([0-9A-Fa-f]{4,6})\t/);
		if (header) {
			current = parseInt(header[1], 16);
			continue;
		}
		if (current === null) continue;

		const alias = line.match(/^\t[=%]\s+(.+)$/);
		if (!alias) continue;

		const parts = alias[1].split(',').map((s) => s.trim()).filter(Boolean);
		const list = aliases.get(current);
		if (list) list.push(...parts);
		else aliases.set(current, parts);
	}

	return aliases;
}
