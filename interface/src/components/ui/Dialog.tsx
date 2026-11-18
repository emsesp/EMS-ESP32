import { type FC, type ReactNode, useEffect, useId, useRef } from 'react';

import './Dialog.css';
import { cx } from './cx';

export interface DialogProps {
  open: boolean;
  title: string;
  // called on Escape and on a click outside; leave out for a dialog that must be answered
  onClose?: (() => void) | undefined;
  actions?: ReactNode;
  size?: 'sm' | 'md';
  children?: ReactNode;
}

// Native modal <dialog>: top layer, focus trap and Escape come from the browser
const Dialog: FC<DialogProps> = ({
  open,
  title,
  onClose,
  actions,
  size = 'sm',
  children
}) => {
  const ref = useRef<HTMLDialogElement>(null);
  const titleId = useId();

  useEffect(() => {
    const dialog = ref.current;
    if (!dialog) return;
    if (open && !dialog.open) dialog.showModal();
    if (!open && dialog.open) dialog.close();
  }, [open]);

  return (
    <dialog
      ref={ref}
      className={cx('dialog', size)}
      aria-labelledby={titleId}
      onCancel={(e) => {
        e.preventDefault();
        onClose?.();
      }}
      onClick={(e) => {
        if (e.target === ref.current) onClose?.();
      }}
      // closed by the browser rather than by `open`, e.g. a form with method="dialog"
      onClose={() => {
        if (open) onClose?.();
      }}
    >
      {open && (
        <div className="dialog-box">
          <h2 id={titleId}>{title}</h2>
          <div className="dialog-content">{children}</div>
          {actions && <div className="dialog-actions">{actions}</div>}
        </div>
      )}
    </dialog>
  );
};

export default Dialog;
