import { useEffect, useRef } from "react"
import { bus } from './bus';
import { randomUUID } from "crypto";

export const useEventListener = (fn: ((...args: any[]) => void) | undefined) => {
	const id = useRef(randomUUID());
	const callback = useRef<((...args: any[]) => void) | undefined>(fn);

	useEffect(() => {
		const { unsubscribe } = bus?.subscribe(id.current, (...args: any[]) => { callback.current?.(...args) });

		return unsubscribe;
	}, []);

	useEffect(() => {
		console.log('update handler');
		callback.current = fn;
	}, [fn]);

	return fn && id.current;
}
