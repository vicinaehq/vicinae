import React, { ReactNode, useEffect, useState } from "react";
import context from "./navigation-context";
import { bus } from "../bus";

const View: React.FC<{ children: ReactNode }> = ({ children }) => {
  return <>{children}</>;
};

export const NavigationProvider: React.FC<{ root: ReactNode }> = ({ root }) => {
  const [navStack, setNavStack] = useState<ReactNode[]>([root]);

  const pop = () => {
    bus.turboRequest("ui.popView", {}).then(() => {
      setNavStack((cur) => cur.slice(0, -1));
    });
  };

  const push = (node: ReactNode) => {
    bus.turboRequest("ui.pushView", {}).then(() => {
      setNavStack((cur) => [...cur, node]);
    });
  };

  useEffect(() => {
    //console.log('changed nav stack size', navStack.length);
  }, [navStack]);

  useEffect(() => {
    const listener = bus!.subscribe("pop-view", () => {
      setNavStack((cur) => cur.slice(0, -1));
    });

    return () => listener.unsubscribe();
  }, []);

  return (
    <context.Provider
      value={{
        push,
        pop,
      }}
    >
      {navStack.map((el, idx) => (
        <View key={idx}>{el}</View>
      ))}
    </context.Provider>
  );
};
