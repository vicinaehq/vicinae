const dbUrl = 'https://raw.githubusercontent.com/github/gemoji/refs/heads/master/db/emoji.json';

const makeEmojiInfo = ({ emoji, description, tags, category, aliases }) => `{ .emoji = "${emoji}", .description = "${description}", .aliases = {${aliases.map(a => `"${a}"`).join(',')}}, .tags = {${tags.map(a => `"${a}"`).join(',')}}, .category = categories[${category}] }`;

const main = async () => {
	const res = await fetch(dbUrl);
	const json = await res.json();
	const serializedEmojis = [];
	const categories = [];
	
	for (const emoji of json) {
		let categoryIndex = categories.indexOf(emoji.category);

		if (categoryIndex == -1) {
			categoryIndex = categories.length;
			categories.push(emoji.category);
		}

		serializedEmojis.push(makeEmojiInfo({
			emoji: emoji.emoji,
			description: emoji.description,
			category: categoryIndex,
			aliases: emoji.aliases,
			tags: emoji.tags
		}));
	}

	const emojis = serializedEmojis.join(',\n');

	const baseCode = `
#include <vector>
#include "emoji-database.hpp"

const char* categories[] = {
	${categories.map(cat => `"${cat}"`).join(',\n')}
};

const std::vector<EmojiInfo> EMOJI_LIST = {
	${emojis}
};
`;

	process.stdout.write(baseCode);
}

main();
