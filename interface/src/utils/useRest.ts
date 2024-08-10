// TODO Alova 3 code....
// import { useState } from 'react';
// import { useBlocker } from 'react-router-dom';
// import { toast } from 'react-toastify';
// import type { AlovaGenerics, Method } from 'alova';
// import { useRequest } from 'alova/client';
// import { useI18nContext } from 'i18n/i18n-react';
// export interface RestRequestOptions<D> {
//   read: () => Method<AlovaGenerics>;
//   update: (value: D) => Method<AlovaGenerics>;
// }
// export const useRest = <D>({ read, update }: RestRequestOptions<D>) => {
//   const { LL } = useI18nContext();
//   const [errorMessage, setErrorMessage] = useState<string>();
//   const [restartNeeded, setRestartNeeded] = useState<boolean>(false);
//   const [origData, setOrigData] = useState<D>();
//   const [dirtyFlags, setDirtyFlags] = useState<string[]>([]);
//   const blocker = useBlocker(dirtyFlags.length !== 0);
//   const {
//     data,
//     send: readData,
//     update: updateData
//   } = useRequest(read()).onComplete((event) => {
//     setOrigData(event.data as D);
//   });
//   const { loading: saving, send: writeData } = useRequest(
//     (newData: D) => update(newData),
//     { immediate: false }
//   ).onSuccess(() => {
//     toast.success(LL.UPDATED_OF(LL.SETTINGS(1)));
//     setDirtyFlags([]);
//   });
//   const updateDataValue = (new_data: D) => {
//     updateData({ data: new_data });
//   };
//   const loadData = async () => {
//     setDirtyFlags([]);
//     setErrorMessage(undefined);
//     await readData().catch((error: Error) => {
//       toast.error(error.message);
//       setErrorMessage(error.message);
//     });
//   };
//   const saveData = async () => {
//     if (!data) {
//       return;
//     }
//     setRestartNeeded(false);
//     setErrorMessage(undefined);
//     setDirtyFlags([]);
//     setOrigData(data as D);
//     await writeData(data as D).catch((error: Error) => {
//       if (error.message === 'Reboot required') {
//         setRestartNeeded(true);
//       } else {
//         toast.error(error.message);
//         setErrorMessage(error.message);
//       }
//     });
//   };
//   return {
//     loadData,
//     saveData,
//     saving: saving as boolean,
//     updateDataValue,
//     data: data as D, // Explicitly define the type of 'data'
//     origData: origData as D, // Explicitly define the type of 'origData' to 'D'
//     dirtyFlags,
//     setDirtyFlags,
//     setOrigData,
//     blocker,
//     errorMessage,
//     restartNeeded
//   } as const;
// };
import { useState } from 'react';
import { useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';

import { type Method, useRequest } from 'alova';
import { useI18nContext } from 'i18n/i18n-react';

export interface RestRequestOptions<D> {
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  read: () => Method<any, any, any, any, any, any, any>;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  update: (value: D) => Method<any, any, any, any, any, any, any>;
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
    update: updateData,
    onComplete: onReadComplete
  } = useRequest(read());

  const {
    loading: saving,
    send: writeData,
    onSuccess: onWriteSuccess
  } = useRequest((newData: D) => update(newData), { immediate: false });

  const updateDataValue = (new_data: D) => {
    updateData({ data: new_data });
  };

  onWriteSuccess(() => {
    toast.success(LL.UPDATED_OF(LL.SETTINGS(1)));
    setDirtyFlags([]);
  });

  onReadComplete((event) => {
    setOrigData(event.data as D);
  });

  const loadData = async () => {
    setDirtyFlags([]);
    setErrorMessage(undefined);
    await readData().catch((error: Error) => {
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
  };

  return {
    loadData,
    saveData,
    saving: saving as boolean,
    updateDataValue,
    data: data as D, // Explicitly define the type of 'data'
    origData: origData as D, // Explicitly define the type of 'origData' to 'D'
    dirtyFlags,
    setDirtyFlags,
    setOrigData,
    blocker,
    errorMessage,
    restartNeeded
  } as const;
};
