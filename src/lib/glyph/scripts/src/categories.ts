export type Kind = 'Emoji' | 'Symbol';

export type Category =
	| 'SmileysAndEmotion'
	| 'PeopleAndBody'
	| 'AnimalsAndNature'
	| 'FoodAndDrink'
	| 'TravelAndPlaces'
	| 'Activities'
	| 'Objects'
	| 'Symbols'
	| 'Flags'
	| 'Math'
	| 'Arrows'
	| 'Currency'
	| 'Punctuation'
	| 'Shapes'
	| 'MiscSymbols'
	| 'Greek'
	| 'NumberForms'
	| 'FancyLetters';

export interface CategoryInfo {
	category: Category;
	kind: Kind;
	label: string;
}

// Order defines the generated glyph::Category enum and the grid/dropdown order:
// emoji groups first (emoji-test order), then symbol categories.
export const CATEGORIES: CategoryInfo[] = [
	{ category: 'SmileysAndEmotion', kind: 'Emoji', label: 'Smileys & Emotion' },
	{ category: 'PeopleAndBody', kind: 'Emoji', label: 'People & Body' },
	{ category: 'AnimalsAndNature', kind: 'Emoji', label: 'Animals & Nature' },
	{ category: 'FoodAndDrink', kind: 'Emoji', label: 'Food & Drink' },
	{ category: 'TravelAndPlaces', kind: 'Emoji', label: 'Travel & Places' },
	{ category: 'Activities', kind: 'Emoji', label: 'Activities' },
	{ category: 'Objects', kind: 'Emoji', label: 'Objects' },
	{ category: 'Symbols', kind: 'Emoji', label: 'Symbols' },
	{ category: 'Flags', kind: 'Emoji', label: 'Flags' },
	{ category: 'Math', kind: 'Symbol', label: 'Math' },
	{ category: 'Arrows', kind: 'Symbol', label: 'Arrows' },
	{ category: 'Currency', kind: 'Symbol', label: 'Currency' },
	{ category: 'Punctuation', kind: 'Symbol', label: 'Punctuation' },
	{ category: 'Shapes', kind: 'Symbol', label: 'Shapes' },
	{ category: 'MiscSymbols', kind: 'Symbol', label: 'Misc Symbols' },
	{ category: 'Greek', kind: 'Symbol', label: 'Greek' },
	{ category: 'NumberForms', kind: 'Symbol', label: 'Number Forms' },
	{ category: 'FancyLetters', kind: 'Symbol', label: 'Fancy Letters' },
];

export const EMOJI_GROUP_CATEGORY: Record<string, Category> = {
	'Smileys & Emotion': 'SmileysAndEmotion',
	'People & Body': 'PeopleAndBody',
	'Animals & Nature': 'AnimalsAndNature',
	'Food & Drink': 'FoodAndDrink',
	'Travel & Places': 'TravelAndPlaces',
	Activities: 'Activities',
	Objects: 'Objects',
	Symbols: 'Symbols',
	Flags: 'Flags',
};

export type SymbolCategory = Extract<
	Category,
	'Math' | 'Arrows' | 'Currency' | 'Punctuation' | 'Shapes' | 'MiscSymbols' | 'Greek' | 'NumberForms' | 'FancyLetters'
>;

export interface Item {
	character: string;
	name: string;
	keywords: string[];
	kind: Kind;
	category: Category;
	skinnable: boolean;
}
