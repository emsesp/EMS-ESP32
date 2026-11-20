import { useCallback, useMemo, useRef, useState } from 'react';
import { useBlocker } from 'react-router';

import type { AlovaGenerics, Method } from 'alova';
import { useRequest } from 'alova/client';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';

import { useInterval } from './useInterval';

export type PendingState = 'new' | 'mod' | 'del' | undefined;

export interface PendingRow<T> {
  // stable for the life of the row, also when the item's own id changes
  key: string;
  item: T;
  state: PendingState;
}

export interface PendingListOptions<T> {
  read: () => Method<AlovaGenerics>;
  // gets the list as it will be after saving (deleted rows left out)
  write: (items: T[]) => Method<AlovaGenerics>;
  // reload this often while nothing is pending
  pollMs?: number;
  // toast text after a successful save
  savedText?: string;
}

const same = (a: unknown, b: unknown) => JSON.stringify(a) === JSON.stringify(b);

// a list loaded from the device, edited locally and saved in one go
export const usePendingList = <T>({
  read,
  write,
  pollMs,
  savedText
}: PendingListOptions<T>) => {
  const { LL } = useI18nContext();
  // edits per row key: the new value, or null when the row is deleted
  const [edits, setEdits] = useState<Map<string, T | null>>(new Map());
  const [added, setAdded] = useState<{ key: string; item: T }[]>([]);
  const nextKey = useRef(0);

  const {
    data,
    error,
    send: reload
  } = useRequest(read).onSuccess(() => {
    setEdits(new Map());
    setAdded([]);
  });
  const loaded = data as T[] | undefined;

  const { send: sendWrite, loading: saving } = useRequest(
    (items: T[]) => write(items),
    { immediate: false }
  );

  const rows = useMemo<PendingRow<T>[]>(() => {
    const base = (loaded ?? []).map((item, i) => {
      const key = `o${i}`;
      const edit = edits.get(key);
      if (edit === null) return { key, item, state: 'del' as const };
      if (edit !== undefined) return { key, item: edit, state: 'mod' as const };
      return { key, item, state: undefined };
    });
    return [...base, ...added.map((a) => ({ ...a, state: 'new' as const }))];
  }, [loaded, edits, added]);

  const changes = edits.size + added.length;
  const blocker = useBlocker(changes !== 0);

  useInterval(() => {
    if (pollMs && changes === 0) void reload();
  }, pollMs ?? 3_600_000);

  const add = useCallback((item: T) => {
    const key = `n${nextKey.current++}`;
    setAdded((a) => [...a, { key, item }]);
    return key;
  }, []);

  const update = useCallback(
    (key: string, item: T) => {
      if (key.startsWith('n')) {
        setAdded((a) => a.map((x) => (x.key === key ? { key, item } : x)));
        return;
      }
      const orig = loaded?.[Number(key.slice(1))];
      setEdits((e) => {
        const next = new Map(e);
        if (same(orig, item)) next.delete(key);
        else next.set(key, item);
        return next;
      });
    },
    [loaded]
  );

  // a new row is dropped straight away, a loaded one is marked until Save
  const remove = useCallback((key: string) => {
    if (key.startsWith('n')) {
      setAdded((a) => a.filter((x) => x.key !== key));
      return;
    }
    setEdits((e) => new Map(e).set(key, null));
  }, []);

  const undo = useCallback((key: string) => {
    if (key.startsWith('n')) {
      setAdded((a) => a.filter((x) => x.key !== key));
      return;
    }
    setEdits((e) => {
      const next = new Map(e);
      next.delete(key);
      return next;
    });
  }, []);

  const cancel = useCallback(() => {
    setEdits(new Map());
    setAdded([]);
  }, []);

  const save = async () => {
    try {
      await sendWrite(rows.filter((r) => r.state !== 'del').map((r) => r.item));
      toast.success(savedText ?? LL.SETTINGS_UPDATED());
      await reload();
    } catch (err) {
      toast.error(err instanceof Error ? err.message : String(err));
    }
  };

  return {
    data: loaded,
    error,
    reload,
    rows,
    // the list without deleted rows, e.g. for duplicate checks
    items: rows.filter((r) => r.state !== 'del').map((r) => r.item),
    changes,
    saving: !!saving,
    blocker,
    add,
    update,
    remove,
    undo,
    cancel,
    save
  };
};

export type PendingList<T> = ReturnType<typeof usePendingList<T>>;
