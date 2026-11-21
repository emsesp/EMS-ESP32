import type { ReactNode } from 'react';

import { API } from 'api/app';

import { useRequest } from 'alova/client';
import { Button, SaveBar, toast } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import type { APIcall } from './types';

interface ChangesBarProps {
  message: ReactNode;
  saving: boolean;
  // Save only, e.g. while the change would leave no admin
  saveDisabled?: boolean;
  onDiscard: () => void;
  onSave: () => void;
}

// save bar with Discard and Save for a page with unsaved changes
export const ChangesBar = ({
  message,
  saving,
  saveDisabled,
  onDiscard,
  onSave
}: ChangesBarProps) => {
  const { LL } = useI18nContext();
  return (
    <SaveBar message={message}>
      <Button color="neutral" icon="undo" disabled={saving} onClick={onDiscard}>
        {LL.DISCARD()}
      </Button>
      <Button icon="check" disabled={saving || !!saveDisabled} onClick={onSave}>
        {LL.SAVE()}
      </Button>
    </SaveBar>
  );
};

// save bar asking for a restart after saving; onRestarted runs once the device accepted it
export const RestartBar = ({ onRestarted }: { onRestarted: () => void }) => {
  const { LL } = useI18nContext();
  const { send, loading } = useRequest((call: APIcall) => API(call), {
    immediate: false
  });
  return (
    <SaveBar warning icon="restart_alt" message={LL.SAVED_RESTART()}>
      <Button
        color="warning"
        icon="restart_alt"
        disabled={loading}
        onClick={() =>
          void send({ device: 'system', cmd: 'restart', id: 0 })
            .then(onRestarted)
            .catch((error: Error) => toast.error(error.message))
        }
      >
        {LL.RESTART_NOW()}
      </Button>
    </SaveBar>
  );
};
