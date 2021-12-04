import { useCallback, useEffect, useState } from 'react';
import { useSnackbar } from 'notistack';
import { AxiosPromise } from 'axios';

import { extractErrorMessage } from '.';

export interface RestRequestOptions<D> {
  read: () => AxiosPromise<D>;
  update?: (value: D) => AxiosPromise<D>;
}

export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
  const { enqueueSnackbar } = useSnackbar();

  const [saving, setSaving] = useState<boolean>(false);
  const [data, setData] = useState<D>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);

  const loadData = useCallback(async () => {
    setData(undefined);
    setErrorMessage(undefined);
    try {
      setData((await read()).data);
    } catch (error: any) {
      const message = extractErrorMessage(error, 'Problem loading data');
      enqueueSnackbar(message, { variant: 'error' });
      setErrorMessage(message);
    }
  }, [read, enqueueSnackbar]);

  const save = useCallback(
    async (toSave: D) => {
      if (!update) {
        return;
      }
      setSaving(true);
      setRestartNeeded(false);
      setErrorMessage(undefined);
      try {
        const response = await update(toSave);
        setData(response.data);
        if (response.status === 202) {
          setRestartNeeded(true);
        }
        enqueueSnackbar('Settings saved', { variant: 'success' });
      } catch (error: any) {
        const message = extractErrorMessage(error, 'Problem saving data');
        enqueueSnackbar(message, { variant: 'error' });
        setErrorMessage(message);
      } finally {
        setSaving(false);
      }
    },
    [update, enqueueSnackbar]
  );

  const saveData = () => data && save(data);

  useEffect(() => {
    loadData();
  }, [loadData]);

  return { loadData, saveData, saving, setData, data, errorMessage, restartNeeded } as const;
};
