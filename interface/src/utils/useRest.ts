import { useCallback, useEffect, useState } from 'react';
import { useSnackbar } from 'notistack';
import { AxiosPromise } from 'axios';

import { extractErrorMessage } from '.';

import { useI18nContext } from 'i18n/i18n-react';

import { unstable_useBlocker as useBlocker } from 'react-router-dom';

export interface RestRequestOptions<D> {
  read: () => AxiosPromise<D>;
  update?: (value: D) => AxiosPromise<D>;
}

export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
  const { LL } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();

  const [data, setData] = useState<D>();
  const [saving, setSaving] = useState<boolean>(false);
  const [errorMessage, setErrorMessage] = useState<string>();
  const [restartNeeded, setRestartNeeded] = useState<boolean>(false);

  const [origData, setOrigData] = useState<D>();
  const [dirtyFlags, setDirtyFlags] = useState<string[]>();

  const blocker = useBlocker(dirtyFlags?.length !== 0);

  const loadData = useCallback(async () => {
    setData(undefined);
    setDirtyFlags([]);
    setErrorMessage(undefined);
    try {
      const fetch_data = (await read()).data;
      setData(fetch_data);
      setOrigData(fetch_data);
    } catch (error) {
      const message = extractErrorMessage(error, LL.PROBLEM_LOADING());
      enqueueSnackbar(message, { variant: 'error' });
      setErrorMessage(message);
    }
  }, [read, enqueueSnackbar, LL]);

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
        setOrigData(response.data);
        setData(response.data);
        if (response.status === 202) {
          setRestartNeeded(true);
        } else {
          enqueueSnackbar(LL.UPDATED_OF(LL.SETTINGS_OF('')), { variant: 'success' });
        }
      } catch (error) {
        const message = extractErrorMessage(error, LL.PROBLEM_UPDATING());
        enqueueSnackbar(message, { variant: 'error' });
        setErrorMessage(message);
      } finally {
        setSaving(false);
        setDirtyFlags([]);
      }
    },
    [update, enqueueSnackbar, LL]
  );

  const saveData = () => data && save(data);

  useEffect(() => {
    loadData();
  }, [loadData]);

  return {
    loadData,
    saveData,
    saving,
    setData,
    data,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    errorMessage,
    restartNeeded
  } as const;
};
