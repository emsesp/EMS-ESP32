import { useEffect, useState } from 'react';

import {
  readCommands,
  readCustomEntities,
  readSchedule,
  readSensorData
} from '../../api/app';

// The names of the sensors, scheduler entries, custom entities and commands, for the search of the Dashboard.
// They are read once, when the first search starts; the administrator pages are only asked for administrators.

export type ModulePage = 'sensors' | 'scheduler' | 'customentities' | 'commands';

export interface ModuleEntry {
  page: ModulePage;
  name: string;
  hint: string;
}

export const useModuleIndex = (wanted: boolean, admin: boolean): ModuleEntry[] => {
  const [entries, setEntries] = useState<ModuleEntry[]>([]);
  const [loaded, setLoaded] = useState(false);

  useEffect(() => {
    if (!wanted || loaded) return;
    setLoaded(true);
    const found: ModuleEntry[] = [];
    const add = (page: ModulePage, name: string, hint = '') => {
      if (name) found.push({ page, name, hint });
    };
    void Promise.allSettled([
      readSensorData().then((d) => {
        d.ts.forEach((s) => add('sensors', s.n, s.id));
        d.as.forEach((s) => add('sensors', s.n, `GPIO ${s.g}`));
      }),
      admin
        ? readSchedule().then((l) =>
            l.forEach((i) => add('scheduler', i.name, i.cmd_name))
          )
        : undefined,
      admin
        ? readCustomEntities().then((l) =>
            l.forEach((i) => add('customentities', i.name))
          )
        : undefined,
      readCommands().then((l) => l.forEach((i) => add('commands', i.name, i.cmd)))
    ]).then(() => setEntries(found));
  }, [wanted, loaded, admin]);

  return entries;
};

export const matchEntries = (
  entries: readonly ModuleEntry[],
  needle: string
): ModuleEntry[] =>
  entries.filter((e) => `${e.name} ${e.hint}`.toLowerCase().includes(needle));
