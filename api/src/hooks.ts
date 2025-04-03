import { useEffect, useRef } from "react"
import { bus } from './bus';
import { randomUUID } from "crypto";

export const useEventListener = (fn: ((...args: any[]) => void) | undefined) => {
	const id = useRef(randomUUID());
	const callback = useRef<((...args: any[]) => void) | undefined>(fn);

	useEffect(() => {
		bus?.subscribe(id.current, (...args: any[]) => { callback.current?.(...args) });

		return () => { bus?.unsubscribe(id.current); }
	}, []);

	useEffect(() => {
		callback.current = fn;
	}, [fn]);

	return fn && id.current;
}
