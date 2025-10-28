import { useCallback, useMemo, useState } from 'react';
import { useBlocker } from 'react-router';
import { toast } from 'react-toastify';

import type { AlovaGenerics, Method } from 'alova';
import { useRequest } from 'alova/client';
import { useI18nContext } from 'i18n/i18n-react';

export interface RestRequestOptions<D> {
  read: () => Method<AlovaGenerics>;
  update: (value: D) => Method<AlovaGenerics>;
}

const REBOOT_ERROR_MESSAGE = 'Reboot required';

export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
  const { LL } = useI18nContext();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [restartNeeded, setRestartNeeded] = useState(false);
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

  const updateDataValue = useCallback(
    (new_data: D) => updateData({ data: new_data }),
    [updateData]
  );

  const loadData = useCallback(async () => {
    setDirtyFlags([]);
    setErrorMessage(undefined);
    try {
      await readData();
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error);
      toast.error(message);
      setErrorMessage(message);
    }
  }, [readData]);

  const saveData = useCallback(async () => {
    if (!data) return;

    // Reset states before saving
    setRestartNeeded(false);
    setErrorMessage(undefined);
    setDirtyFlags([]);
    setOrigData(data as D);

    try {
      await writeData(data as D);
    } catch (error) {
      const message = error instanceof Error ? error.message : String(error);
      if (message === REBOOT_ERROR_MESSAGE) {
        setRestartNeeded(true);
      } else {
        toast.error(message);
        setErrorMessage(message);
      }
    }
  }, [data, writeData]);

  return useMemo(
    () => ({
      loadData,
      saveData,
      saving: !!saving,
      updateDataValue,
      data: data as D,
      origData: origData as D,
      dirtyFlags,
      setDirtyFlags,
      setOrigData,
      blocker,
      errorMessage,
      restartNeeded
    }),
    [
      loadData,
      saveData,
      saving,
      updateDataValue,
      data,
      origData,
      dirtyFlags,
      blocker,
      errorMessage,
      restartNeeded
    ]
  );
};
