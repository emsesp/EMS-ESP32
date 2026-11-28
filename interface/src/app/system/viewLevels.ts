import { useContext, useEffect, useMemo, useRef, useState } from 'react';

import { toast } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

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
  // the user moved this value away from its built-in level
  changed: (item: SysItem) => boolean;
}

// Which values are in Normal and which only in All values. The Web UI has a built-in level for every well known
// value; what the user changed is kept on the gateway, so it is the same in every browser.
export const useViewLevels = (items: readonly SysItem[]): LevelItems => {
  const { me } = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();
  const [levels, setLevels] = useState<ViewLevels>({});
  const [supported, setSupported] = useState(false);
  // the Undo button of the message outlives this render, so it reads the current levels
  const current = useRef(levels);
  current.current = levels;

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

  const changed = (i: SysItem): boolean => levels[i.key] !== undefined;

  const toggle = async (item: SysItem) => {
    const base = items.find((i) => i.uid === item.uid)?.level ?? item.level;
    const now = current.current;
    const level = (now[item.key] ?? base) === 1 ? 2 : 1;
    const next = { ...now };
    if (level === base) delete next[item.key];
    else next[item.key] = level;
    const previous = now;
    setLevels(next);
    try {
      await writeViewLevels(next);
      // say where the value went, as in Normal it may just vanish; the click can be undone
      toast.info(level === 2 ? LL.SYS_LEVEL_NOW_ALL() : LL.SYS_LEVEL_NOW_NORMAL(), {
        label: LL.UNDO(),
        onClick: () => void toggle(item)
      });
    } catch (error) {
      setLevels(previous);
      toast.error(error instanceof Error ? error.message : String(error));
    }
  };

  return {
    items: leveled,
    toggle: me.admin && supported ? (i) => void toggle(i) : undefined,
    canToggle,
    changed
  };
};
