import { writeFileSync } from "node:fs";
import { join } from "node:path";
import type { z } from "zod";
import type ManifestSchema from "../schemas/manifest.js";

type Manifest = z.infer<typeof ManifestSchema>;
type Preference = NonNullable<Manifest["preferences"]>[number];
type Argument = NonNullable<Manifest["commands"][number]["arguments"]>[number];

/**
 * Generates and updates the extension types file in the specified directory
 */
export function updateExtensionTypes(
	manifest: Manifest,
	outDir: string,
): { envPath: string } {
	const content = createExtensionTypes(manifest);
	const envPath = join(outDir, "vicinae-env.d.ts");
	writeFileSync(envPath, content);
	return { envPath };
}

/**
 * Generates TypeScript type definitions from a manifest (package.json)
 */
export function createExtensionTypes(manifest: Manifest): string {
	const extPrefs = generateExtensionPreferences(manifest);
	const cmdPrefs = generateCommandPreferences(manifest);
	const argTypes = generateCommandArguments(manifest);

	return `
/// <reference types="@vicinae/api">

/*
 * This file is auto-generated from the extension's manifest.
 * Do not modify manually. Instead, update the \`package.json\` file.
 */

type ExtensionPreferences = {
  ${extPrefs}
}

declare type Preferences = ExtensionPreferences

declare namespace Preferences {
  ${cmdPrefs}
}

declare namespace Arguments {
  ${argTypes}
}
	`.trim();
}

/**
 * Generates TypeScript type definitions for extension preferences
 */
function generateExtensionPreferences(manifest: Manifest): string {
	if (!manifest.preferences) {
		return "";
	}

	const extPrefs: string[] = [];
	for (const pref of manifest.preferences) {
		let prefText = generateDocComment(pref.title, pref.description);
		if (prefText) {
			prefText += "\n\t";
		}
		prefText += `"${pref.name}"${pref.required ? "?" : ""}: ${getPreferenceType(pref)};`;
		extPrefs.push(prefText);
	}

	return extPrefs.join("\n").trim();
}

/**
 * Generates a JSDoc comment string for a preference or argument
 */
function generateDocComment(title?: string, description?: string): string {
	if (!title && !description) {
		return "";
	}

	const content = [title, description].filter(Boolean).join(" - ");
	return `\n\t/** ${content.trim()} */`;
}

/**
 * Generates a TypeScript type string for a preference value
 */
function getPreferenceType(pref: Preference): string {
	if (pref.type === "checkbox") {
		return "boolean";
	}

	if (
		pref.type === "dropdown" &&
		Array.isArray(pref.data) &&
		pref.data.length > 0
	) {
		// Build a union type from the 'value' fields for dropdowns
		const unionValues = pref.data
			.map((option) =>
				typeof option.value === "string" ? `"${option.value}"` : undefined,
			)
			.filter((v): v is string => !!v)
			.join(" | ");
		return unionValues || "string";
	}

	return "string";
}

/**
 * Generates TypeScript type definitions for command preferences
 */
function generateCommandPreferences(manifest: Manifest): string {
	const cmdPrefs: string[] = [];

	for (const cmd of manifest.commands) {
		const prefix =
			generateDocComment(`Command: ${cmd.title}`) +
			`\n\texport type ${toPascalCase(cmd.name)} = ExtensionPreferences & {`;

		const prefs: string[] = [];
		for (const pref of cmd.preferences ?? []) {
			let prefText = "";
			const docComment = generateDocComment(pref.title, pref.description);
			if (docComment) {
				prefText = `\n\t\t${docComment.trim()}\n\t\t`;
			}
			prefText += `"${pref.name}"${pref.required ? "?" : ""}: ${getPreferenceType(pref)};`;
			prefs.push(prefText);
		}

		cmdPrefs.push(`${prefix}\n\t\t${prefs.join("\n").trim()}\n\t}`);
	}

	return cmdPrefs.join("\n").trim();
}

/**
 * Generates TypeScript type definitions for command arguments
 */
function generateCommandArguments(manifest: Manifest): string {
	const cmdArgs: string[] = [];

	for (const cmd of manifest.commands) {
		const prefix =
			generateDocComment(`Command: ${cmd.title}`) +
			`\n\texport type ${toPascalCase(cmd.name)} = {`;

		const args: string[] = [];
		for (const arg of cmd.arguments ?? []) {
			const docComment = generateDocComment(arg.placeholder);
			let argText = `\n\t\t${docComment.trim()}\n\t\t`;
			argText += `"${arg.name}"${arg.required ? "?" : ""}: ${getArgumentType(arg)}`;
			args.push(argText);
		}

		cmdArgs.push(`${prefix}\n\t\t${args.join("\n").trim()}\n\t}`);
	}

	return cmdArgs.join("\n").trim();
}

/**
 * Generates a TypeScript type string for an argument value
 */
function getArgumentType(arg: Argument): string {
	if (
		arg.type === "dropdown" &&
		Array.isArray(arg.data) &&
		arg.data.length > 0
	) {
		// Build a union type from the 'value' fields
		const unionValues = arg.data
			.map((option) =>
				typeof option.value === "string" ? `"${option.value}"` : undefined,
			)
			.filter((v): v is string => !!v)
			.join(" | ");
		if (unionValues) {
			return unionValues;
		}
	}
	return "string";
}

/**
 * Converts a string to PascalCase
 */
function toPascalCase(str: string): string {
	return str
		.replace(/([a-z])([A-Z])/g, "$1 $2") // Splits camelCase words into separate words
		.replace(/[-_]+|[^\p{L}\p{N}]/gu, " ") // Replaces dashes, underscores, and special characters with spaces
		.toLowerCase() // Converts the entire string to lowercase
		.replace(/(?:^|\s)(\p{L})/gu, (_, letter) => letter.toUpperCase()) // Capitalizes the first letter of each word
		.replace(/\s+/g, ""); // Removes all spaces
}
