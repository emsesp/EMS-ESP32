import { useCallback, useEffect, useState } from 'react';
import { useSnackbar } from 'notistack';

export interface RestRequestOptions {
  endpoint: string;
  fetchFunction?: typeof fetch;
}

const useRest = <D>({
  endpoint,
  fetchFunction = fetch
}: RestRequestOptions) => {
  const { enqueueSnackbar } = useSnackbar();

  const [saving, setSaving] = useState<boolean>(false);
  const [data, setData] = useState<D>();
  const [errorMessage, setErrorMessage] = useState<string>();

  const handleError = useCallback(
    (error: any) => {
      const errorMessage = error.message || 'Unknown error';
      enqueueSnackbar('Problem fetching: ' + errorMessage, {
        variant: 'error'
      });
      setErrorMessage(errorMessage);
    },
    [enqueueSnackbar]
  );

  const loadData = useCallback(async () => {
    setData(undefined);
    setErrorMessage(undefined);
    try {
      const response = await fetchFunction(endpoint);
      if (response.status !== 200) {
        throw Error('Invalid status code: ' + response.status);
      }
      setData(await response.json());
    } catch (error) {
      handleError(error);
    }
  }, [handleError, fetchFunction, endpoint]);

  const save = useCallback(
    async (data: D) => {
      setSaving(true);
      setErrorMessage(undefined);
      try {
        const response = await fetchFunction(endpoint, {
          method: 'POST',
          body: JSON.stringify(data),
          headers: {
            'Content-Type': 'application/json'
          }
        });
        if (response.status !== 200) {
          throw Error('Invalid status code: ' + response.status);
        }
        enqueueSnackbar('Update successful.', { variant: 'success' });
        setData(await response.json());
      } catch (error) {
        handleError(error);
      } finally {
        setSaving(false);
      }
    },
    [enqueueSnackbar, handleError, fetchFunction, endpoint]
  );

  const saveData = () => data && save(data);

  useEffect(() => {
    loadData();
  }, [loadData]);

  return { loadData, saveData, saving, setData, data, errorMessage } as const;
};

export default useRest;
