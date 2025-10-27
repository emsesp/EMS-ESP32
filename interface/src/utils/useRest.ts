import { useCallback, useState } from 'react';
import { useBlocker } from 'react-router';
import { toast } from 'react-toastify';

import type { AlovaGenerics, Method } from 'alova';
import { useRequest } from 'alova/client';
import { useI18nContext } from 'i18n/i18n-react';

export interface RestRequestOptions<D> {
  read: () => Method<AlovaGenerics>;
  update: (value: D) => Method<AlovaGenerics>;
}

export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
  const { LL } = useI18nContext();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);
  const [origData, setOrigData] = useState<D>();
  const [dirtyFlags, setDirtyFlags] = useState<string[]>([]);
  const blocker = useBlocker(dirtyFlags.length !== 0);

  const {
    data,
    send: readData,
    update: updateData
  } = useRequest(read()).onComplete((event) => {
    setOrigData(event.data as D);
  });

  const { loading: saving, send: writeData } = useRequest(
    (newData: D) => update(newData),
    { immediate: false }
  ).onSuccess(() => {
    toast.success(LL.UPDATED_OF(LL.SETTINGS(1)));
    setDirtyFlags([]);
  });

  // Memoize updateDataValue to prevent unnecessary re-renders
  const updateDataValue = useCallback(
    (new_data: D) => {
      updateData({ data: new_data });
    },
    [updateData]
  );

  // Memoize loadData to prevent unnecessary re-renders
  const loadData = useCallback(async () => {
    setDirtyFlags([]);
    setErrorMessage(undefined);
    await readData().catch((error: Error) => {
      toast.error(error.message);
      setErrorMessage(error.message);
    });
  }, [readData]);

  // Memoize saveData to prevent unnecessary re-renders
  const saveData = useCallback(async () => {
    if (!data) {
      return;
    }
    setRestartNeeded(false);
    setErrorMessage(undefined);
    setDirtyFlags([]);
    setOrigData(data as D);
    await writeData(data as D).catch((error: Error) => {
      if (error.message === 'Reboot required') {
        setRestartNeeded(true);
      } else {
        toast.error(error.message);
        setErrorMessage(error.message);
      }
    });
  }, [data, writeData]);

  return {
    loadData,
    saveData,
    saving: saving as boolean,
    updateDataValue,
    data: data as D,
    origData: origData as D,
    dirtyFlags,
    setDirtyFlags,
    setOrigData,
    blocker,
    errorMessage,
    restartNeeded
  } as const;
};
