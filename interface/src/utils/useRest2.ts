import { useRequest, type Method } from 'alova';
import { useState } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';

import { useI18nContext } from 'i18n/i18n-react';

export interface RestRequestOptions2<D> {
  read: () => Method<any, any, any, any, any, any, any>;
  update: (value: D) => Method<any, any, any, any, any, any, any>;
}

// TODO rename back to useRest
export const useRest2 = <D>({ read, update }: RestRequestOptions2<D>) => {
  const { LL } = useI18nContext();

  const [errorMessage, setErrorMessage] = useState<string>();
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);
  const [origData, setOrigData] = useState<D>();

  const [dirtyFlags, setDirtyFlags] = useState<string[]>([]);
  const blocker = useBlocker(dirtyFlags.length !== 0);

  const { data: data, send: readData, update: updateData, onComplete: onReadComplete } = useRequest(read());

  const {
    loading: saving,
    send: writeData,
    onSuccess: onWriteSuccess
  } = useRequest((newData: D) => update(newData), { immediate: false });

  const updateDataValue = (new_data: D) => {
    updateData({ data: new_data });
  };

  onWriteSuccess(() => {
    toast.success(LL.UPDATED_OF(LL.SETTINGS()));
    setDirtyFlags([]);
  });

  onReadComplete((event) => {
    setOrigData(event.data);
  });

  const loadData = async () => {
    setDirtyFlags([]);
    setErrorMessage(undefined);
    await readData().catch((error) => {
      toast.error(error.message);
      setErrorMessage(error.message);
    });
  };

  const saveData = async () => {
    if (!data) {
      return;
    }
    setRestartNeeded(false);
    setErrorMessage(undefined);
    await writeData(data).catch((error) => {
      if (error.message === 'Reboot required') {
        setRestartNeeded(true);
      } else {
        toast.error(error.message);
        setErrorMessage(error.message);
      }
    });
  };

  return {
    loadData,
    saveData,
    saving,
    updateDataValue,
    data,
    origData,
    dirtyFlags,
    setDirtyFlags,
    setOrigData,
    blocker,
    errorMessage,
    restartNeeded
  } as const;
};
