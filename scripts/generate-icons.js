// Codegen for icons
// Generates cpp source and header to access builtin icons through an enum/string identifier and the typescript enum consumed by extensions.

const path = require('path');
const fs = require('fs');
const OMNI_ICON_DIR = path.join(__dirname, "..", "vicinae", "icons");

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

const toEnumType = (str) => {
	return str
		.toLowerCase()
		.replace(/[^a-zA-Z0-9]+(.)/g, (match, char) => char.toUpperCase())
		.replace(/^./, char => char.toUpperCase());
}

const generateSources = (files) => {
	const serializedFileNames = files.map(file => `"${file.split('.')[0]}"`);
	const enumNames = files.map(file => toEnumType(`${file.split('.')[0]}`));
	const header = `#pragma once
#include <string>
#include <format>
#include <unordered_map>

enum class BuiltinIcon: std::uint16_t {
	${enumNames.join(',\n\t')}
};

class BuiltinIconService {
public:
  using Mapping = std::unordered_map<BuiltinIcon, const char*>;

  static BuiltinIcon unknownIcon() { return BuiltinIcon::QuestionMarkCircle; }
  static std::string pathForName(std::string_view name) { return std::format(":icons/{}", name); }
  static const char* nameForIcon(BuiltinIcon icon);
  static const Mapping& mapping();
};
`;

	const src = `#include "builtin_icon.hpp"

static const std::unordered_map<BuiltinIcon, const char*> iconMap = {
	${enumNames.map((name, i) => `{BuiltinIcon::${name}, ${serializedFileNames[i]}}`)
			.join(',\n\t')}
};

const BuiltinIconService::Mapping& BuiltinIconService::mapping() {
	return iconMap;
}

const char* BuiltinIconService::nameForIcon(BuiltinIcon icon) {
	if (auto it = iconMap.find(icon); it != iconMap.end()) {
		return it->second;
	}
	return nullptr;
}
`;


	const tsEnum = `
export enum Icon {
${enumNames.map((name, i) => `${name} = ${serializedFileNames[i]}`)
			.join(',\n\t')}
}
`;

	return { cpp: { src, header }, ts: { iconEnum: tsEnum } };
};


const writeFile = (path, data) => {
	fs.writeFileSync(path, data);
	console.log(`Wrote file at ${path}`);
}

const icons = fs.readdirSync(OMNI_ICON_DIR).filter((file) => file.endsWith('.svg'));
const qrc = generateQrc(icons);
const { cpp, ts } = generateSources(icons);
const cppSrcDir = path.join(__dirname, "..", "vicinae", "src");
const apiIconSource = path.join(__dirname, "..", "typescript", "api", "src", "api", "icon.ts");

writeFile(path.join(__dirname, "..", "vicinae", "icons", "icons.qrc"), qrc);
writeFile(path.join(cppSrcDir, "builtin_icon.cpp"), cpp.src);
writeFile(path.join(cppSrcDir, "builtin_icon.hpp"), cpp.header);
writeFile(apiIconSource, ts.iconEnum);
