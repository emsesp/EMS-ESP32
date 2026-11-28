import { type FC, type ReactNode, createContext, useContext } from 'react';
import { createPortal } from 'react-dom';

// The layout puts an element at the right end of the title bar and provides it here; null until it exists.
// Outside the layout (undefined) the content renders where it is used.
export const AppBarSlotContext = createContext<HTMLElement | null | undefined>(
  undefined
);

// Puts controls of a page (search, a switch) into the title bar instead of a row of their own
const AppBarActions: FC<{ children: ReactNode }> = ({ children }) => {
  const slot = useContext(AppBarSlotContext);
  if (slot === undefined) return <>{children}</>;
  return slot ? createPortal(children, slot) : null;
};

export default AppBarActions;
