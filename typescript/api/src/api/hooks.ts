import { useEffect, useRef } from "react";
import { bus } from "./bus";

const handlerIdFactory = {
	nextHandlerId: 0,
	next() {
		return this.nextHandlerId++;
	},
};

/*
export const useEventListener = (
  fn: ((...args: any[]) => void) | undefined,
) => {
  const id = useRef(`handler:${handlerIdFactory.next()}`);
  const callback = useRef<((...args: any[]) => void) | undefined>();

  useEffect(() => {
    const { unsubscribe } = bus.subscribe(id.current, (...args: any[]) => {
      callback.current?.(...args);
    });

    return unsubscribe;
  }, []);

  useEffect(() => {
    callback.current = fn;
  }, [fn]);

  return fn && id.current;
};
*/
