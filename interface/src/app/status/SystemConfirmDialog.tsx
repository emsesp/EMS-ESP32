import { ConfirmDialog } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

export type SystemCommand = 'restart' | 'format';

interface SystemConfirmDialogProps {
  // the command to confirm; the dialog is closed while undefined
  confirm: SystemCommand | undefined;
  disabled?: boolean;
  onClose: () => void;
  onConfirm: (cmd: SystemCommand) => unknown;
}

// asks before a restart or a factory reset (Status overview and Firmware page)
const SystemConfirmDialog = ({
  confirm,
  disabled,
  onClose,
  onConfirm
}: SystemConfirmDialogProps) => {
  const { LL } = useI18nContext();
  const format = confirm === 'format';

  return (
    <ConfirmDialog
      open={confirm !== undefined}
      title={format ? LL.FACTORY_RESET() : LL.RESTART()}
      label={format ? LL.FACTORY_RESET() : LL.RESTART()}
      icon={format ? 'delete_forever' : 'restart_alt'}
      color={format ? 'error' : 'warning'}
      disabled={disabled}
      onClose={onClose}
      onConfirm={() => confirm && onConfirm(confirm)}
    >
      {format ? LL.SYSTEM_FACTORY_TEXT_DIALOG() : LL.RESTART_CONFIRM()}
    </ConfirmDialog>
  );
};

export default SystemConfirmDialog;
