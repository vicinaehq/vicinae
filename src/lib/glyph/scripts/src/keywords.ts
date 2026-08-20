const STOPWORDS = new Set(['a', 'an', 'and', 'the', 'of', 'to', 'for', 'in', 'on', 'with', 'as', 'or', 'at', 'by', 'from']);

function tokenize(text: string): string[] {
	return text
		.toLowerCase()
		.split(/[^\p{L}\p{N}]+/u)
		.filter((token) => token.length >= 2 && !STOPWORDS.has(token));
}

export interface KeywordSources {
	displayName: string;
	unicodeName: string;
	cldr: string[];
	aliases: string[];
}

// CLDR keywords are kept verbatim (short ones like "x" or "eur" matter); names and aliases are tokenized.
export function buildKeywords(sources: KeywordSources): string[] {
	const keywords: string[] = [];
	const seen = new Set<string>();

	const add = (value: string) => {
		const normalized = value.trim().toLowerCase();
		if (normalized && !seen.has(normalized)) {
			seen.add(normalized);
			keywords.push(normalized);
		}
	};

	for (const keyword of sources.cldr) add(keyword);

	const tokens = new Set<string>();
	for (const source of [sources.displayName, sources.unicodeName, ...sources.aliases]) {
		for (const token of tokenize(source)) tokens.add(token);
	}
	for (const token of [...tokens].sort()) add(token);

	return keywords;
}
