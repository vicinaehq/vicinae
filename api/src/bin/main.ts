#! /usr/bin/env node

import { developExtension } from "./develop";

export const main = async () => {
	const command = process.argv[2];

	if (command === 'develop') {
		await developExtension(process.argv[3] ?? process.cwd());
	}
}

main();
