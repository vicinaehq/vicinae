import { readFileSync } from 'node:fs';

export interface UcdEntry {
	codepoint: number;
	symbol: string;
	name: string;
	gc: string;
}

// "<..., First>"/"<..., Last>" range markers are not expanded: no curated block uses them.
export function parseUnicodeData(path: string): UcdEntry[] {
	const entries: UcdEntry[] = [];

	for (const line of readFileSync(path, 'utf-8').split('\n')) {
		const [cp, name, gc] = line.split(';');
		const codepoint = parseInt(cp, 16);
		if (Number.isNaN(codepoint)) continue;
		entries.push({ codepoint, symbol: String.fromCodePoint(codepoint), name, gc });
	}

	return entries;
}
