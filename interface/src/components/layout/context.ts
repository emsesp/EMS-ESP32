import { createContext, useContext, useEffect } from 'react';

export interface LayoutContextValue {
  title: string;
  setTitle: (title: string) => void;
}

const LayoutContextDefaultValue = {} as LayoutContextValue;
export const LayoutContext = createContext(LayoutContextDefaultValue);

export const useLayoutTitle = (myTitle: string) => {
  const { setTitle } = useContext(LayoutContext);

  useEffect(() => {
    setTitle(myTitle);
  }, [setTitle, myTitle]);
};