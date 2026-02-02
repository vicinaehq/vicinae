import lib from 'emojilib';

//@ts-ignore - no typedefs for this package
import orderedEmojis from 'unicode-emoji-json/data-ordered-emoji';
//@ts-ignore - no typedefs for this package
import data from 'unicode-emoji-json';

import { CppSourceBuilder } from './cpp-source-builder';

const main = async () => {
	const builder = new CppSourceBuilder;

	for (const emoji of orderedEmojis) {
		const { name, group, skin_tone_support: skineToneSupport } = data[emoji];
		builder.addEmoji(emoji, name, group, lib[emoji], skineToneSupport);
	}

	await builder.build("dist");
}

main();
