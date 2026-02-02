import type { Form } from "../components/form";
import { useImperativeHandle, useRef } from "react";
import type { Ref } from "react";
import { bus } from "../bus";
import { randomUUID } from "node:crypto";

export const useImperativeFormHandle = (
	ref?: Ref<Form.ItemReference>,
): [string] => {
	const handleId = useRef<string>(randomUUID());

	useImperativeHandle(ref, () => {
		return {
			focus: () => {
				bus.emit(handleId.current, {
					type: "focus",
				});
			},
			reset: () => {
				bus.emit(handleId.current, {
					type: "reset",
				});
			},
		};
	}, []);

	return [handleId.current];
};
