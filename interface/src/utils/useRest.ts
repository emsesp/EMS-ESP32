import { useCallback, useEffect, useState } from 'react';
import { useSnackbar } from 'notistack';
import { AxiosPromise } from 'axios';

import { extractErrorMessage } from '.';

import { useI18nContext } from '../i18n/i18n-react';

export interface RestRequestOptions<D> {
  read: () => AxiosPromise<D>;
  update?: (value: D) => AxiosPromise<D>;
}

export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
  const { LL } = useI18nContext();

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
        setData(response.data);
        if (response.status === 202) {
          setRestartNeeded(true);
        } else {
          enqueueSnackbar(LL.SETTINGS_OF('') + ' ' + LL.SAVED(), { variant: 'success' });
        }
      } catch (error) {
        const message = extractErrorMessage(error, LL.PROBLEM_UPDATING());
        enqueueSnackbar(message, { variant: 'error' });
        setErrorMessage(message);
      } finally {
        setSaving(false);
      }
    },
    [update, enqueueSnackbar, LL]
  );

  const saveData = () => data && save(data);

  useEffect(() => {
    loadData();
  }, [loadData]);

  return { loadData, saveData, saving, setData, data, errorMessage, restartNeeded } as const;
};
