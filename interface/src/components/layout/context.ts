import { createContext, useContext, useEffect, useRef } from 'react';

export interface LayoutContextValue {
  title: string;
  setTitle: (title: string) => void;
}

const LayoutContextDefaultValue = {} as LayoutContextValue;
export const LayoutContext = createContext(LayoutContextDefaultValue);

export const useLayoutTitle = (myTitle: string) => {
  const { title, setTitle } = useContext(LayoutContext);
  const previousTitle = useRef(title);

  useEffect(() => {
    setTitle(myTitle);
  }, [setTitle, myTitle]);

  useEffect(
    () => () => {
      setTitle(previousTitle.current);
    },
    [setTitle]
  );
};
