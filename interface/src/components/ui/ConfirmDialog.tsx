import { type FC, type ReactNode, useState } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import Button, { type ButtonProps } from './Button';
import Dialog from './Dialog';
import type { IconName } from './icons';

export interface ConfirmDialogProps {
  open: boolean;
  title: string;
  // the action button
  label: string;
  icon: IconName;
  color?: ButtonProps['color'];
  disabled?: boolean | undefined;
  // the button stays disabled until a returned promise settles
  onConfirm: () => unknown;
  onClose: () => void;
  cancelLabel?: string | undefined;
  cancelIcon?: IconName | undefined;
  size?: 'sm' | 'md';
  // plain text is put in a paragraph
  children?: ReactNode;
}

// asks before an action: Cancel and one coloured action button
const ConfirmDialog: FC<ConfirmDialogProps> = ({
  open,
  title,
  label,
  icon,
  color = 'primary',
  disabled,
  onConfirm,
  onClose,
  cancelLabel,
  cancelIcon = 'cancel',
  size = 'sm',
  children
}) => {
  const { LL } = useI18nContext();
  const [busy, setBusy] = useState(false);

  const confirm = async () => {
    setBusy(true);
    try {
      await onConfirm();
    } finally {
      setBusy(false);
    }
  };

  return (
    <Dialog
      open={open}
      title={title}
      size={size}
      onClose={onClose}
      actions={
        <>
          <Button color="neutral" icon={cancelIcon} onClick={onClose}>
            {cancelLabel ?? LL.CANCEL()}
          </Button>
          <Button
            color={color}
            icon={icon}
            disabled={busy || disabled}
            onClick={() => void confirm()}
          >
            {label}
          </Button>
        </>
      }
    >
      {typeof children === 'string' ? <p>{children}</p> : children}
    </Dialog>
  );
};

export default ConfirmDialog;
