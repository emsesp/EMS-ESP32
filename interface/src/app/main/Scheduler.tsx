import { useMemo, useState } from 'react';

import { useRequest } from 'alova/client';
import {
  Button,
  Icon,
  SearchField,
  Section,
  SortHeader,
  Switch,
  Table,
  Tag,
  byNumber,
  byText,
  cx,
  rowAction,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { type PendingRow, usePendingList } from 'utils';

import { readCommands, readSchedule, writeSchedule } from '../../api/app';
import ListPage, { PendingTag, pendingClass } from './ListPage';
import './Scheduler.css';
import SchedulerDialog from './SchedulerDialog';
import {
  ALL_DAYS,
  DAY_FLAGS,
  SCHEDULE_TYPES,
  scheduleType,
  weekdayNames
} from './schedule';
import { ScheduleFlag } from './types';
import type { CommandItem, ScheduleItem } from './types';

const POLL_MS = 30000;

const DEFAULT_ITEM: Omit<ScheduleItem, 'id' | 'cmd_name'> = {
  active: false,
  flags: ALL_DAYS,
  time: '00:00',
  name: ''
};

interface Editing {
  // missing when adding
  key?: string;
  item: ScheduleItem;
}

const Scheduler = () => {
  const { LL, locale } = useI18nContext();
  const [query, setQuery] = useState('');
  const [editing, setEditing] = useState<Editing>();
  const days = useMemo(() => weekdayNames(locale, 'narrow'), [locale]);

  const list = usePendingList<ScheduleItem>({
    read: readSchedule,
    write: (items) =>
      writeSchedule({
        schedule: items.map(({ id, active, flags, time, cmd_name, name }) => ({
          id,
          active,
          flags,
          time,
          cmd_name,
          name
        }))
      }),
    pollMs: POLL_MS,
    savedText: LL.SCHEDULE_UPDATED()
  });

  const { data: commands } = useRequest(readCommands, {
    initialData: [] as CommandItem[]
  });

  const q = query.trim().toLowerCase();
  const matches = ({ item }: PendingRow<ScheduleItem>) => {
    if (!q) return true;
    const c = commands.find((x) => x.name === item.cmd_name);
    return `${item.name} ${item.time} ${item.cmd_name} ${c?.cmd ?? ''} ${c?.value ?? ''}`
      .toLowerCase()
      .includes(q);
  };
  const rows = list.rows.filter(matches).sort((a, b) => a.item.flags - b.item.flags);
  const byFlags = byNumber<PendingRow<ScheduleItem>>((r) => r.item.flags);
  const byTime = byText<PendingRow<ScheduleItem>>((r) => r.item.time);
  const sort = useTableSort(rows, {
    active: byNumber((r) => r.item.active),
    name: byText((r) => r.item.name),
    schedule: (a, b) => byFlags(a, b) || byTime(a, b),
    command: byText((r) => r.item.cmd_name)
  });

  const add = () =>
    setEditing({
      item: {
        ...DEFAULT_ITEM,
        id: list.items.reduce((max, si) => Math.max(max, si.id), 0) + 1,
        cmd_name: commands[0]?.name ?? ''
      }
    });

  const save = (item: ScheduleItem) => {
    if (editing?.key) list.update(editing.key, item);
    else list.add(item);
    setEditing(undefined);
  };

  const when = (si: ScheduleItem) => {
    const type = scheduleType(si.flags);
    const t = SCHEDULE_TYPES.find((x) => x.flag === type)!;
    if (type === ScheduleFlag.SCHEDULE_DAY) {
      return (
        <span className="sch-when">
          <Icon name={t.icon} size="xs" className="muted" label={t.label(LL)} />
          <span className="sch-days">
            {DAY_FLAGS.map((flag, i) => (
              <span key={flag} className={cx((si.flags & flag) === flag && 'on')}>
                {days[i]}
              </span>
            ))}
          </span>
          <b>{si.time}</b>
        </span>
      );
    }
    const atStart =
      type === ScheduleFlag.SCHEDULE_TIMER &&
      (si.time === '' || si.time === '00:00');
    return (
      <span
        className={cx(
          'sch-when',
          type !== ScheduleFlag.SCHEDULE_TIMER && 'sch-expr'
        )}
      >
        <Icon name={t.icon} size="xs" className="muted" />
        <span className="muted">{t.label(LL)}</span>
        {atStart ? (
          LL.SCHEDULER_HELP_2()
        ) : (
          <span className={cx(type === ScheduleFlag.SCHEDULE_TIMER ? 'lb' : 'mono')}>
            {si.time}
          </span>
        )}
      </span>
    );
  };

  const command = (si: ScheduleItem) => {
    const c = commands.find((x) => x.name === si.cmd_name);
    if (!c) {
      return (
        <Tag kind="err" icon="link_off">
          {si.cmd_name || '-'}
        </Tag>
      );
    }
    return (
      <div>
        <div>{c.name}</div>
        <div className="mono muted sch-cmd">
          {c.cmd}
          {c.value ? ` = ${c.value}` : ''}
        </div>
      </div>
    );
  };

  const renderRow = ({ key, item: si, state }: PendingRow<ScheduleItem>) => {
    const deleted = state === 'del';
    const open = deleted
      ? undefined
      : rowAction(() => setEditing({ key, item: si }), si.name);
    return (
      <tr
        key={key}
        {...open}
        className={cx(
          open?.className,
          pendingClass(state),
          !si.active && !deleted && 'dim'
        )}
      >
        <td className="sch-act" onClick={(e) => e.stopPropagation()}>
          <Switch
            label={<span className="sr-only">{`${LL.ACTIVE()} ${si.name}`}</span>}
            checked={si.active}
            disabled={deleted}
            onChange={(active) => list.update(key, { ...si, active })}
          />
        </td>
        <td className="sch-name">
          {si.name}
          <PendingTag state={state} />
        </td>
        <td data-label={LL.SCHEDULE(0)}>{when(si)}</td>
        <td data-label={LL.COMMAND(0)}>{command(si)}</td>
        <td className="act">
          {deleted && (
            <Button
              size="sm"
              color="secondary"
              icon="undo"
              onClick={() => list.undo(key)}
            >
              {LL.UNDO()}
            </Button>
          )}
        </td>
      </tr>
    );
  };

  return (
    <ListPage list={list}>
      <Section
        desc={LL.SCHEDULER_HELP_1()}
        title={LL.SCHEDULER()}
        icon="more_time"
        actions={
          <>
            <SearchField
              label={LL.SEARCH()}
              value={query}
              onInput={(e) => setQuery(e.currentTarget.value)}
            />
            <Button icon="add" onClick={add}>
              {LL.ADD(0)}
            </Button>
          </>
        }
      >
        <div className="tbl-wrap">
          <Table stack className="sch-tbl">
            <thead>
              <tr>
                <SortHeader className="sch-act" {...sort.header('active')}>
                  {LL.ACTIVE()}
                </SortHeader>
                <SortHeader {...sort.header('name')}>{LL.NAME(0)}</SortHeader>
                <SortHeader {...sort.header('schedule')}>
                  {LL.SCHEDULE(0)}
                </SortHeader>
                <SortHeader {...sort.header('command')}>{LL.COMMAND(0)}</SortHeader>
                <th />
              </tr>
            </thead>
            <tbody>
              {sort.rows.map(renderRow)}
              {rows.length === 0 && (
                <tr>
                  <td colSpan={5} className="tbl-empty">
                    {LL.SCHEDULE_NONE()}
                  </td>
                </tr>
              )}
            </tbody>
          </Table>
        </div>
      </Section>

      {editing && (
        <SchedulerDialog
          creating={!editing.key}
          item={editing.item}
          items={list.items}
          commands={commands}
          onClose={() => setEditing(undefined)}
          onSave={save}
          onRemove={
            editing.key
              ? () => {
                  list.remove(editing.key!);
                  setEditing(undefined);
                }
              : undefined
          }
        />
      )}
    </ListPage>
  );
};

export default Scheduler;
