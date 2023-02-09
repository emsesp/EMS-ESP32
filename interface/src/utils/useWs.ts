import { useCallback, useEffect, useRef, useState } from 'react';
import Sockette from 'sockette';
import { debounce } from 'lodash-es';

import { addAccessTokenParameter } from 'api/authentication';

interface WebSocketIdMessage {
  type: 'id';
  id: string;
}

interface WebSocketPayloadMessage<D> {
  type: 'payload';
  origin_id: string;
  payload: D;
}

export type WebSocketMessage<D> = WebSocketIdMessage | WebSocketPayloadMessage<D>;

export const useWs = <D>(wsUrl: string, wsThrottle = 100) => {
  const ws = useRef<Sockette>();
  const clientId = useRef<string>();

  const [connected, setConnected] = useState<boolean>(false);
  const [data, setData] = useState<D>();
  const [transmit, setTransmit] = useState<boolean>();
  const [clear, setClear] = useState<boolean>();

  const onMessage = useCallback((event: MessageEvent) => {
    const rawData = event.data;
    if (typeof rawData === 'string' || rawData instanceof String) {
      const message = JSON.parse(rawData as string) as WebSocketMessage<D>;
      switch (message.type) {
        case 'id':
          clientId.current = message.id;
          break;
        case 'payload':
          if (clientId.current) {
            setData((existingData) => (clientId.current === message.origin_id && existingData) || message.payload);
          }
          break;
      }
    }
  }, []);

  const doSaveData = useCallback((newData: D, clearData = false) => {
    if (!ws.current) {
      return;
    }
    if (clearData) {
      setData(undefined);
    }
    ws.current.json(newData);
  }, []);

  const saveData = useRef(debounce(doSaveData, wsThrottle));

  const updateData = (newData: React.SetStateAction<D | undefined>, transmitData: true, clearData: false) => {
    setData(newData);
    setTransmit(transmitData);
    setClear(clearData);
  };

  useEffect(() => {
    if (!transmit) {
      return;
    }
    data && saveData.current(data, clear);
    setTransmit(false);
    setClear(false);
  }, [doSaveData, data, transmit, clear]);

  useEffect(() => {
    const instance = new Sockette(addAccessTokenParameter(wsUrl), {
      onmessage: onMessage,
      onopen: () => {
        setConnected(true);
      },
      onclose: () => {
        clientId.current = undefined;
        setConnected(false);
        setData(undefined);
      }
    });
    ws.current = instance;
    return instance.close;
  }, [wsUrl, onMessage]);

  return { connected, data, updateData } as const;
};
