import { type FC, type ReactNode, useEffect, useId, useRef } from 'react';

import './Dialog.css';
import { cx } from './cx';

export interface DialogProps {
  open: boolean;
  title: string;
  // called on Escape and on a click outside; leave out for a dialog that must be answered
  onClose?: (() => void) | undefined;
  // makes the dialog a form: Enter in a field submits it, so the primary action needs type="submit"
  onSubmit?: (() => void) | undefined;
  actions?: ReactNode;
  size?: 'sm' | 'md';
  children?: ReactNode;
}

// after a failed validation, once the errors have rendered
export const focusInvalid = () =>
  setTimeout(() =>
    document
      .querySelector<HTMLElement>('.dialog[open] [aria-invalid="true"]')
      ?.focus()
  );

// Native modal <dialog>: top layer, focus trap and Escape come from the browser
const Dialog: FC<DialogProps> = ({
  open,
  title,
  onClose,
  onSubmit,
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

  const body = (
    <>
      <h2 id={titleId}>{title}</h2>
      <div className="dialog-content">{children}</div>
      {actions && <div className="dialog-actions">{actions}</div>}
    </>
  );

  return (
    <dialog
      ref={ref}
      className={cx('dialog', size)}
      aria-labelledby={titleId}
      onCancel={(e) => {
        e.preventDefault();
        onClose?.();
      }}
      // a click outside a form would throw away what was typed; Escape and Cancel still close it
      onClick={(e) => {
        if (e.target === ref.current && !onSubmit) onClose?.();
      }}
      // closed by the browser rather than by `open`, e.g. a form with method="dialog"
      onClose={() => {
        if (open) onClose?.();
      }}
    >
      {open &&
        (onSubmit ? (
          <form
            className="dialog-box"
            noValidate
            onSubmit={(e) => {
              e.preventDefault();
              onSubmit();
            }}
          >
            {body}
          </form>
        ) : (
          <div className="dialog-box">{body}</div>
        ))}
    </dialog>
  );
};

export default Dialog;
