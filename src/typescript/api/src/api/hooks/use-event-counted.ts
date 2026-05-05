import { useCallback, useRef } from "react";

export type EventCounted<T> = { value: T; eventCount: number };

export function useEventCounted<T>(
	value: T | undefined,
	onChange?: (value: T) => void,
): [EventCounted<T> | undefined, ((...args: any[]) => void) | undefined] {
	const ref = useRef(0);

	const handler = useCallback(
		(...args: any[]) => {
			if (typeof args[1] === "number") ref.current = args[1];
			onChange?.(args[0]);
		},
		[onChange],
	);

	const counted: EventCounted<T> | undefined =
		value != null ? { value, eventCount: ref.current } : undefined;

	return [counted, onChange ? handler : undefined];
}
