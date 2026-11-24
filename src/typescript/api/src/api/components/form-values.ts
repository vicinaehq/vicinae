import type { Form } from "./form";

type WireDate = { $date: string | null };

/** The app sends dates as ISO 8601 UTC strings, or null when the field is empty or invalid. */
export const toDate = (value: string | null): Date | null =>
	value ? new Date(value) : null;

const isWireDate = (value: unknown): value is WireDate =>
	typeof value === "object" && value !== null && "$date" in value;

/** The app submits date picker values as `{ $date }` so they can be told apart from strings. */
export const reviveFormValues = (
	values: Record<string, unknown>,
): Form.Values =>
	Object.fromEntries(
		Object.entries(values).map(([id, value]) => [
			id,
			isWireDate(value) ? toDate(value.$date) : value,
		]),
	) as Form.Values;
