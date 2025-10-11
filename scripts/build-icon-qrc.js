const { join } = require('path');
const { readdirSync, writeFileSync } = require('fs');
const OMNI_ICON_DIR = join("..", "vicinae", "icons");

const generateQrc = (files) => {
	const serializedFiles = files.map(file => `<file>${file}</file>`);

	return `<!DOCTYPE RCC>
<RCC version="1.0">
	<qresource prefix="/icons">
		${serializedFiles.join('\n\t\t')}
	</qresource>
</RCC>
`;
}

const generateSource = (files) => {
	const serializedFileNames = files.map(file => `"${file.split('.')[0]}"`);

	return `#include "builtin_icon.hpp"
#include <QString>
#include <QList>

static const QList<QString> builtinIcons = {
	${serializedFileNames.join(',\n\t')}
};

const QList<QString> &BuiltinIconService::icons() { return builtinIcons; }
`;

};

const icons = readdirSync(OMNI_ICON_DIR).filter((file) => file.endsWith('.svg'));
const qrc = generateQrc(icons);
const source = generateSource(icons);

writeFileSync(join("..", "vicinae", "icons", "icons.qrc"), qrc);
writeFileSync(join("..", "vicinae", "src", "builtin_icon.cpp"), source);
