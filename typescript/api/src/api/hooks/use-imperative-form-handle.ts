import { randomUUID } from "crypto";
import { FormItemRef } from "../components/form";
import { useImperativeHandle, useRef, Ref } from "react";
import { bus } from "../bus";

export const useImperativeFormHandle = (ref?: Ref<FormItemRef>): [string] => {
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
