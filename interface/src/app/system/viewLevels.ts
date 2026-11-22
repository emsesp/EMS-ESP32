import { useContext, useEffect, useMemo, useState } from 'react';

import { toast } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';

import { readViewLevels, writeViewLevels } from '../../api/app';
import type { ViewLevels } from '../../api/app';
import type { SysItem } from './model';

// The gateway only stores keys of printable ASCII characters, without quotes and backslashes
const storable = (key: string): boolean =>
  key.length > 0 &&
  key.length <= 80 &&
  [...key].every((c) => c >= ' ' && c <= '~' && c !== '"' && c !== '\\');

const clean = (data: unknown): ViewLevels => {
  const out: ViewLevels = {};
  if (typeof data === 'object' && data !== null) {
    for (const [k, v] of Object.entries(data)) {
      if (v === 1 || v === 2) out[k] = v;
    }
  }
  return out;
};

export interface LevelItems {
  // the items with the level the user chose; the built-in level where nothing was chosen
  items: SysItem[];
  // undefined when the level can't be changed (not an administrator, firmware without the feature)
  toggle: ((item: SysItem) => void) | undefined;
  canToggle: (item: SysItem) => boolean;
}

// Which values are in Normal and which only in All values. The Web UI has a built-in level for every well known
// value; what the user changed is kept on the gateway, so it is the same in every browser.
export const useViewLevels = (items: readonly SysItem[]): LevelItems => {
  const { me } = useContext(AuthenticatedContext);
  const [levels, setLevels] = useState<ViewLevels>({});
  const [supported, setSupported] = useState(false);

  useEffect(() => {
    let active = true;
    readViewLevels()
      .then((data) => {
        if (active) {
          setLevels(clean(data));
          setSupported(true);
        }
      })
      .catch(() => {
        // an older firmware: the built-in levels apply
      });
    return () => {
      active = false;
    };
  }, []);

  const leveled = useMemo(
    () =>
      items.map((i) => {
        const l = levels[i.key];
        return l && l !== i.level ? { ...i, level: l } : i;
      }),
    [items, levels]
  );

  const canToggle = (i: SysItem): boolean =>
    me.admin && supported && storable(i.key);

  const toggle = async (item: SysItem) => {
    const base = items.find((i) => i.uid === item.uid)?.level ?? item.level;
    const level = (levels[item.key] ?? base) === 1 ? 2 : 1;
    const next = { ...levels };
    if (level === base) delete next[item.key];
    else next[item.key] = level;
    const previous = levels;
    setLevels(next);
    try {
      await writeViewLevels(next);
    } catch (error) {
      setLevels(previous);
      toast.error(error instanceof Error ? error.message : String(error));
    }
  };

  return {
    items: leveled,
    toggle: me.admin && supported ? (i) => void toggle(i) : undefined,
    canToggle
  };
};
