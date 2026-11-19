import { useMemo } from 'react';

import {
  Button,
  ButtonLink,
  Listbox,
  Message,
  Segmented,
  Switch,
  TextArea,
  TextField,
  cx
} from 'components/ui';
import 'components/ui/Chip.css';
import { useI18nContext } from 'i18n/i18n-react';

import EntityDialog from './EntityDialog';
import './Scheduler.css';
import {
  ALL_DAYS,
  DAY_FLAGS,
  SCHEDULE_TYPES,
  WORK_DAYS,
  scheduleType,
  weekdayNames
} from './schedule';
import { ScheduleFlag } from './types';
import type { CommandItem, ScheduleItem } from './types';
import { schedulerItemValidation } from './validators';

interface SchedulerDialogProps {
  creating: boolean;
  item: ScheduleItem;
  // the list without deleted rows, for the duplicate name check
  items: ScheduleItem[];
  commands: CommandItem[];
  onClose: () => void;
  onSave: (item: ScheduleItem) => void;
  onRemove?: (() => void) | undefined;
}

const DEFAULT_TIME = '00:00';

const SchedulerDialog = ({
  creating,
  item,
  items,
  commands,
  onClose,
  onSave,
  onRemove
}: SchedulerDialogProps) => {
  const { LL, locale } = useI18nContext();
  const days = useMemo(() => weekdayNames(locale, 'short'), [locale]);

  const commandOptions = commands.map((c) => ({
    value: c.name,
    label: `${c.name} (${c.cmd}${c.value ? ` = ${c.value}` : ''})`
  }));

  return (
    <EntityDialog
      title={creating ? LL.ADD_SCHEDULE() : LL.EDIT_SCHEDULE()}
      item={item}
      creating={creating}
      validator={(v) =>
        schedulerItemValidation(items, v, creating ? undefined : item.name)
      }
      onClose={onClose}
      onSave={onSave}
      onRemove={onRemove}
      size="md"
    >
      {(f) => {
        const { flags, time, cmd_name } = f.values;
        const type = scheduleType(flags);
        const daysError = f.errors?.flags?.[0]?.message;
        const toggleDay = (flag: number) => f.set({ flags: flags ^ flag });
        const options =
          cmd_name && !commands.some((c) => c.name === cmd_name)
            ? [...commandOptions, { value: cmd_name, label: cmd_name }]
            : commandOptions;

        return (
          <>
            <Segmented
              label={LL.TYPE(0)}
              fullWidth
              options={SCHEDULE_TYPES.map((t) => ({
                value: String(t.flag),
                label: t.label(LL)
              }))}
              value={String(type)}
              onChange={(s) => {
                const t = Number(s) as ScheduleFlag;
                if (t === type) return;
                f.set(
                  t === ScheduleFlag.SCHEDULE_DAY
                    ? { flags: ALL_DAYS, time: DEFAULT_TIME }
                    : { flags: t, time: '' }
                );
              }}
            />

            {type === ScheduleFlag.SCHEDULE_DAY && (
              <div className={cx('field', 'full', daysError && 'invalid')}>
                <span className="seg-label">{LL.SCHEDULE(0)}</span>
                <div className="row wrap sch-days-edit">
                  <span
                    className="row sch-pills"
                    role="group"
                    aria-label={LL.SCHEDULE(0)}
                  >
                    {DAY_FLAGS.map((flag, i) => (
                      <button
                        key={flag}
                        type="button"
                        className={cx(
                          'chip',
                          'outline',
                          'sch-pill',
                          (flags & flag) === flag && 'on'
                        )}
                        aria-pressed={(flags & flag) === flag}
                        onClick={() => toggleDay(flag)}
                      >
                        {days[i]}
                      </button>
                    ))}
                  </span>
                  <span className="grow" />
                  <Button
                    size="sm"
                    color="secondary"
                    onClick={() => f.set({ flags: WORK_DAYS })}
                  >
                    {`${days[0]}–${days[4]}`}
                  </Button>
                  <Button
                    size="sm"
                    color="secondary"
                    onClick={() => f.set({ flags: ALL_DAYS })}
                  >
                    {LL.EVERY_DAY()}
                  </Button>
                </div>
                {daysError && <span className="cap err">{daysError}</span>}
              </div>
            )}

            {type === ScheduleFlag.SCHEDULE_DAY && (
              <TextField
                label={LL.TIME(1)}
                type="time"
                {...f.text('time')}
                value={time || DEFAULT_TIME}
              />
            )}
            {type === ScheduleFlag.SCHEDULE_TIMER && (
              <TextField
                label={LL.TIMER(1)}
                type="time"
                helperText={`${DEFAULT_TIME} = ${LL.SCHEDULER_HELP_2()}`}
                {...f.text('time')}
                value={time || DEFAULT_TIME}
              />
            )}
            {type === ScheduleFlag.SCHEDULE_ONCHANGE && (
              <TextField
                label={LL.ONCHANGE()}
                mono
                fullWidth
                placeholder="boiler/outdoortemp"
                {...f.text('time')}
              />
            )}
            {type === ScheduleFlag.SCHEDULE_CONDITION && (
              <TextArea
                label={LL.CONDITION()}
                rows={2}
                placeholder="boiler/outdoortemp < 0"
                {...f.text('time')}
              />
            )}

            {commands.length === 0 && (
              <Message
                level="warning"
                className="full"
                actions={
                  <ButtonLink to="/commands" size="sm" icon="playlist_play">
                    {LL.COMMANDS()}
                  </ButtonLink>
                }
              >
                {LL.SCHEDULER_NO_COMMANDS()}
              </Message>
            )}
            <Listbox
              label={LL.COMMAND(0)}
              fullWidth
              options={options}
              {...f.select('cmd_name')}
            />
            <TextField label={LL.NAME(0)} mono {...f.text('name')} />
            <Switch label={LL.ACTIVE()} {...f.check('active')} />
          </>
        );
      }}
    </EntityDialog>
  );
};

export default SchedulerDialog;
