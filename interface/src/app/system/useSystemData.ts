import { useCallback, useEffect, useMemo, useRef, useState } from 'react';

import { toast } from 'components/ui';
import { useInterval } from 'utils';

import {
  readCoreData,
  readDeviceData,
  readDeviceEntities,
  writeDeviceValue
} from '../../api/app';
import type { Device, DeviceEntity } from '../main/types';
import { TYPE_NAME, buildItems } from './model';
import type { SysItem } from './model';

export interface SystemData {
  loading: boolean;
  error: string | null;
  items: SysItem[];
  reload: () => Promise<void>;
}

const POLL_MS = 15000;

// Loads all devices one after the other (gentle on the ESP32) and refreshes regularly.
export const useSystemData = (): SystemData => {
  const [state, setState] = useState<{
    loading: boolean;
    error: string | null;
    items: SysItem[];
  }>({ loading: true, error: null, items: [] });
  const entities = useRef(new Map<number, readonly DeviceEntity[] | null>());
  const busy = useRef(false);
  // a reload that was asked for while another one ran, e.g. after a write
  const again = useRef(false);
  // the items of the last successful read of each device
  const last = useRef(new Map<number, SysItem[]>());
  const mounted = useRef(true);

  const reload = useCallback(async () => {
    if (busy.current) {
      again.current = true;
      return;
    }
    busy.current = true;
    try {
      const core = await readCoreData();
      const devices: Device[] = core.devices.filter((d) => TYPE_NAME[d.t]);
      const all: SysItem[] = [];
      for (const d of devices) {
        if (!entities.current.has(d.id)) {
          try {
            entities.current.set(d.id, await readDeviceEntities(d.id));
          } catch {
            // not an administrator or not available: show the values without short names
            entities.current.set(d.id, null);
          }
        }
        try {
          const data = await readDeviceData(d.id);
          const items = buildItems(
            d,
            data.nodes,
            entities.current.get(d.id) ?? null
          );
          last.current.set(d.id, items);
          all.push(...items);
        } catch {
          // a single device without data (e.g. a gateway) must not block the page, and one that
          // doesn't answer for a moment (the gateway is busy after a write) keeps its last values
          all.push(...(last.current.get(d.id) ?? []));
        }
      }
      if (mounted.current) setState({ loading: false, error: null, items: all });
    } catch (error) {
      if (mounted.current) {
        setState((s) => ({
          ...s,
          loading: false,
          error: error instanceof Error ? error.message : String(error)
        }));
      }
    } finally {
      busy.current = false;
      if (again.current && mounted.current) {
        again.current = false;
        void reload();
      }
    }
  }, []);

  useEffect(() => {
    mounted.current = true;
    void reload();
    return () => {
      mounted.current = false;
    };
  }, [reload]);

  useInterval(() => {
    if (!document.hidden) void reload();
  }, POLL_MS);

  return useMemo(() => ({ ...state, reload }), [state, reload]);
};

export interface Edits {
  pending: ReadonlyMap<string, unknown>;
  value: (item: SysItem) => unknown;
  set: (item: SysItem, v: unknown) => void;
  discard: () => void;
  saving: boolean;
  apply: (sent: string) => Promise<void>;
  run: (item: SysItem, sent: string) => Promise<void>;
}

// Collects the changes before they go to the device after confirmation
export const useEdits = (data: SystemData): Edits => {
  const [pending, setPending] = useState<ReadonlyMap<string, unknown>>(new Map());
  const [saving, setSaving] = useState(false);
  const itemsRef = useRef(data.items);
  itemsRef.current = data.items;

  const value = useCallback(
    (item: SysItem) => (pending.has(item.uid) ? pending.get(item.uid) : item.dv.v),
    [pending]
  );

  const set = useCallback((item: SysItem, v: unknown) => {
    setPending((prev) => {
      const next = new Map(prev);
      if (v === item.dv.v) next.delete(item.uid);
      else next.set(item.uid, v);
      return next;
    });
  }, []);

  const send = async (item: SysItem, v: unknown) => {
    await writeDeviceValue({ id: item.devId, c: item.dv.c ?? '', v });
  };

  const apply = useCallback(
    async (sent: string) => {
      setSaving(true);
      try {
        for (const [uid, v] of pending) {
          const item = itemsRef.current.find((i) => i.uid === uid);
          if (item) await send(item, v);
        }
        toast.success(sent);
        setPending(new Map());
        await data.reload();
      } catch (error) {
        toast.error(error instanceof Error ? error.message : String(error));
      } finally {
        setSaving(false);
      }
    },
    [pending, data.reload]
  );

  const run = useCallback(
    async (item: SysItem, sent: string) => {
      try {
        await send(item, item.dv.v ?? '');
        toast.success(sent);
        await data.reload();
      } catch (error) {
        toast.error(error instanceof Error ? error.message : String(error));
      }
    },
    [data]
  );

  const discard = useCallback(() => setPending(new Map()), []);

  return { pending, value, set, discard, saving, apply, run };
};
