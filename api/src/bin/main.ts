#! /usr/bin/env node

import { developExtension } from "./develop";

export const main = () => {
	const command = process.argv[2];

	if (command === 'develop') {
		developExtension(process.argv[3] ?? process.cwd());
	}
}

main();
