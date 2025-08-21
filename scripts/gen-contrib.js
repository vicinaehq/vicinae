const { writeFileSync, mkdirSync } = require('fs');
const { join } = require('path');
const { QrcBuilder } = require('./qrc-builder');

const REPO = 'vicinaehq/vicinae';

const fetchContributors = async () => {
	const res = await fetch(`https://api.github.com/repos/${REPO}/contributors`);

	return res.json();
}

const fetchAvatar = async (url) => {
	const res = await fetch(url);

	return res.arrayBuffer();
}

const main = async () => {
	const contribs = await fetchContributors();
	const qrc = new QrcBuilder;

	console.log(contribs);

	const contributorImageFolder = join(__dirname, '..', 'vicinae', 'contribs');

	mkdirSync(contributorImageFolder, { recursive: true });

	const contribProgLines = [];

	for (const { login, avatar_url, contributions } of contribs) {
		const image = await fetchAvatar(avatar_url);
		const imagePath = join(contributorImageFolder, `${login}.png`);

		writeFileSync(imagePath, Buffer.from(image));
		qrc.addFile(join('contribs', `${login}.png`), { prefix: 'contribs', alias: login });

		contribProgLines.push(`Contributor::Contributor{.login = "${login}", .resource = ":contribs/${login}", .contribs = ${contributions} }`);
	}

	const prog = `#include "contribs.hpp"

static std::vector<Contributor::Contributor> CONTRIB_LIST = {
	${contribProgLines.join(',\n')}
};

std::vector<Contributor::Contributor> Contributor::getList() { return CONTRIB_LIST; };`

	const target = join(__dirname, '..', 'vicinae', 'contribs.qrc');
	const sourceTarget = join(__dirname, '..', 'vicinae', 'src', 'contribs', 'contribs.cpp');

	writeFileSync(sourceTarget, prog);
	writeFileSync(target, qrc.toXML());
	console.log('wrote contrib qrc to ' + target);
	console.log('wrote contribs.cpp to ' + sourceTarget);
}

main();
