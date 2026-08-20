import { readFileSync } from 'node:fs';

export interface Block {
	start: number;
	end: number;
	name: string;
}

// UAX #44: block names compare ignoring case, whitespace, hyphens and underscores.
export function blockKey(name: string): string {
	return name.toLowerCase().replace(/[\s_-]/g, '');
}

export function parseBlocks(path: string): Block[] {
	const blocks: Block[] = [];

	for (const raw of readFileSync(path, 'utf-8').split('\n')) {
		const match = raw.trim().match(/^([0-9A-Fa-f]+)\.\.([0-9A-Fa-f]+);\s*(.+)$/);
		if (!match) continue;
		blocks.push({ start: parseInt(match[1], 16), end: parseInt(match[2], 16), name: match[3].trim() });
	}

	return blocks.sort((a, b) => a.start - b.start);
}

export function makeBlockResolver(blocks: Block[]): (cp: number) => string | undefined {
	return (cp: number) => {
		let lo = 0;
		let hi = blocks.length - 1;

		while (lo <= hi) {
			const mid = (lo + hi) >> 1;
			const block = blocks[mid];

			if (cp < block.start) hi = mid - 1;
			else if (cp > block.end) lo = mid + 1;
			else return blockKey(block.name);
		}

		return undefined;
	};
}
