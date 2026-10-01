import { readFileSync } from 'node:fs';

export interface CldrAnnotation {
	tts?: string[];
	default?: string[];
}

export type Annotations = Record<string, CldrAnnotation>;

const VARIATION_SELECTOR_16 = /️/g;

// The base file covers single codepoints, the derived one adds ZWJ sequences and skin-tone variants.
export function loadAnnotations(basePath: string, derivedPath: string): Annotations {
	const base = JSON.parse(readFileSync(basePath, 'utf-8')).annotations.annotations as Annotations;
	const derived = JSON.parse(readFileSync(derivedPath, 'utf-8')).annotationsDerived.annotations as Annotations;
	return { ...base, ...derived };
}

// CLDR keys omit U+FE0F.
export function lookupAnnotation(annotations: Annotations, glyph: string): CldrAnnotation | undefined {
	return annotations[glyph] ?? annotations[glyph.replace(VARIATION_SELECTOR_16, '')];
}
