import { createContext } from "react";
import type { Form } from "./form";

/** Ids of the date pickers in the current form. The app sends their values back as strings. */
export const DateFieldsContext = createContext<Set<string> | null>(null);

const DATE_ONLY = /^\d{4}-\d{2}-\d{2}$/;

export const parseDate = (value: unknown): Date | null => {
	if (typeof value !== "string" || !value) return null;

	// "YYYY-MM-DD" is local midnight, but new Date() would read it as UTC
	const date = new Date(DATE_ONLY.test(value) ? `${value}T00:00` : value);

	return Number.isNaN(date.getTime()) ? null : date;
};

export const reviveDateValues = (
	values: Form.Values,
	dateFields: Set<string> | null,
): Form.Values => {
	const revived = { ...values };
	for (const id of dateFields ?? []) {
		if (id in revived) revived[id] = parseDate(revived[id]);
	}
	return revived;
};
