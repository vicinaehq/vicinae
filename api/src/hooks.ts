import { useEffect, useId, useRef } from "react"
import { bus } from './bus';
import { randomUUID } from "crypto";


export const useEventListener = (fn: ((...args: any[]) => void) | undefined) => {
	const id = useRef(randomUUID());

	useEffect(() => {
		if (!fn) return ;

		bus?.subscribe(id.current, fn);

		return () => { bus?.unsubscribe(id.current); }
	}, []);

	return fn && id.current;
}
