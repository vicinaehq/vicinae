import { PathLike, readFileSync } from "fs";
import { ExtensionManifest } from './protocols/extension';
import { basename } from "path";
import { Preference, Data as PreferenceData } from "./protocols/preference";

const parsePreferenceData = (data: any): PreferenceData | null => {
	const type = data["type"];

	// TODO: implement them all

	if (type == "textfield") { return { text: {} } }
	if (type == "password") { return { password: {} }}
	if (type == "checkbox") { return { checkbox: { label: data["label"]  } }};
	if (type == "appPicker") { return { appPicker: {} }};
	if (type == "dropdown") {
		return { dropdown: { options: data["data"] }};
	}
	
	return null;
}

export const parsePreference = (data: any): Preference | null => {
	const type = data["type"];
	const title = data["title"];
	const description = data["description"];
	const name = data["name"];
	const placeholder = data["placeholder"];
	const isRequired = data["required"];
	const defaultValue = data["default"];
	const specialData = parsePreferenceData(data);

	if (!specialData) {
		console.error(`Failed to parse preference with type ${type}`);
		return null;
	}

	return {
		id: name,
		title,
		description,
		placeholder,
		isRequired,
		defaultValue,
		data: specialData 
	}
}

export const parseManifest = (path: PathLike): ExtensionManifest => {
	const metadata = JSON.parse(readFileSync(path, 'utf-8'));
	const { name, title, icon, author, version, description, preferences = [] as any } = metadata;
	// TODO: we probably want to handle multiple authors
	
	const parsedPreferences: Preference[] = preferences.map(parsePreference).filter(Boolean);

	/*
	for (const cmd of metadata.commands) {
		const bundle = join(path, `${cmd.name}.js`);
		
		extension.commands.push({ 
			name: cmd.name, 
			title: cmd.title,
			subtitle: cmd.subtitle,
			description: cmd.description,
			arguments: cmd.arguments ?? [],
			preferences: cmd.preferences ?? [],
			mode: cmd.mode,
			componentPath: bundle 
		});
	}
	*/

	return {
		id: basename(path.toString()),
		name,
		icon,
		description,
		preferences: parsedPreferences,
		categories: [],
		commands: [],
		author: "",
	};
}
