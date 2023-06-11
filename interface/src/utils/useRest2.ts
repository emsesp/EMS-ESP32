import { useRequest, type Method } from 'alova';
import { useState } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';

import type { AlovaXHRRequestConfig, AlovaXHRResponse, AlovaXHRResponseHeaders } from '@alova/adapter-xhr';

import { useI18nContext } from 'i18n/i18n-react';

export interface RestRequestOptions<D> {
  read: () => Method<any, any, any, any, any, AlovaXHRResponse<any>, AlovaXHRResponseHeaders>;
  update: (
    value: D
  ) => Method<any, unknown, unknown, unknown, AlovaXHRRequestConfig, AlovaXHRResponse<any>, AlovaXHRResponseHeaders>;
}

export const useRest2 = <D>({ read, update }: RestRequestOptions<D>) => {
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

  const setData = (new_data: D) => {
    console.log('SET DATA'); // TODO remove console
    console.log('new_data:', new_data); // TODO remove console
    updateData({
      data: new_data
    });
  };

  onWriteSuccess(() => {
    toast.success(LL.UPDATED_OF(LL.SETTINGS()));
    setDirtyFlags([]);
  });

  onReadComplete((event) => {
    setOrigData(event.data); // make a copy
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
    console.log('SAVE DATA'); // TODO remove console
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
    setData,
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
