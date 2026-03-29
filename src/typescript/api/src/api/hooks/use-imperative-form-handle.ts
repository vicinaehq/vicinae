import { useImperativeHandle, useRef, type Ref } from "react";
import { randomUUID } from "node:crypto";
import type { Form } from "../components/form";

export const useImperativeFormHandle = (
	ref?: Ref<Form.ItemReference>,
): [string] => {
	const handleId = useRef<string>(randomUUID());

	useImperativeHandle(ref, () => {
		return {
			focus: () => {
				/*
				bus.emit(handleId.current, {
					type: "focus",
				});
				*/
			},
			reset: () => {
				/*
				bus.emit(handleId.current, {
					type: "reset",
				});
				*/
			},
		};
	}, []);

	return [handleId.current];
};
