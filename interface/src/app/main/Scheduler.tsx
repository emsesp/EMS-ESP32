import { useEffect, useState } from 'react';
import { useBlocker } from 'react-router';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CircleIcon from '@mui/icons-material/Circle';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Divider, Stack, Typography } from '@mui/material';

import {
  Body,
  Cell,
  Header,
  HeaderCell,
  HeaderRow,
  Row,
  Table
} from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { updateState, useRequest } from 'alova/client';
import {
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import { readCommands, readSchedule, writeSchedule } from '../../api/app';
import SettingsSchedulerDialog from './SchedulerDialog';
import { ScheduleFlag } from './types';
import type { Schedule, ScheduleItem } from './types';
import { schedulerItemValidation } from './validators';

// Constants
const INTERVAL_DELAY = 30000; // 30 seconds
const MIN_ID = -100;
const MAX_ID = 100;
const ICON_SIZE = 16;
const SCHEDULE_FLAG_THRESHOLD = 127;
const FLAG_ALL_DAYS = 127;
const REFERENCE_YEAR = 2017;
const REFERENCE_MONTH = '01';
const LOG_2 = Math.log(2);

// Days of week starting from Monday (1-7)
const WEEK_DAYS = [1, 2, 3, 4, 5, 6, 7] as const;

const DEFAULT_SCHEDULE_ITEM: Omit<ScheduleItem, 'id' | 'o_id'> = {
  active: false,
  deleted: false,
  flags: FLAG_ALL_DAYS,
  time: '',
  cmd_name: '',
  name: ''
};

const scheduleTheme = {
  Table: `
    --data-table-library_grid-template-columns: 36px 220px repeat(1, minmax(20px, 1fr)) 192px 160px;
  `,
  BaseRow: `
    font-size: 14px;
    .td {
      height: 32px;
    }
  `,
  BaseCell: `
    &:nth-of-type(1) {
      text-align: 8px;
    }
  `,
  HeaderRow: `
    text-transform: uppercase;
    background-color: black;
    color: #90CAF9;
    .th {
      border-bottom: 1px solid #565656;
      height: 36px;
    }
  `,
  Row: `
    background-color: #1e1e1e;
    position: relative;
    cursor: pointer;
    .td {
      border-bottom: 1px solid #565656;
    }
    &:hover .td {
      background-color: #177ac9;
    }
  `
};

const scheduleTypeLabels: Record<number, string> = {
  [ScheduleFlag.SCHEDULE_TIMER]: 'Timer',
  [ScheduleFlag.SCHEDULE_CONDITION]: 'Condition',
  [ScheduleFlag.SCHEDULE_ONCHANGE]: 'On Change'
};

const Scheduler = () => {
  const { LL, locale } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [selectedScheduleItem, setSelectedScheduleItem] = useState<ScheduleItem>();
  const [dow, setDow] = useState<string[]>([]);
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  useLayoutTitle(LL.SCHEDULER());

  const {
    data: schedule,
    send: fetchSchedule,
    error
  } = useRequest(readSchedule, {
    initialData: []
  });

  const { data: commandNames } = useRequest(readCommands, {
    initialData: [],
    initializing: true
  });

  const { send: updateSchedule } = useRequest(
    (data: Schedule) => writeSchedule(data),
    {
      immediate: false
    }
  );

  const hasScheduleChanged = (si: ScheduleItem) => {
    return (
      si.id !== si.o_id ||
      (si.name || '') !== (si.o_name || '') ||
      si.active !== si.o_active ||
      si.deleted !== si.o_deleted ||
      si.flags !== si.o_flags ||
      si.time !== si.o_time ||
      si.cmd_name !== si.o_cmd_name
    );
  };

  useInterval(() => {
    if (numChanges === 0) {
      void fetchSchedule();
    }
  }, INTERVAL_DELAY);

  useEffect(() => {
    const formatter = new Intl.DateTimeFormat(locale, {
      weekday: 'short',
      timeZone: 'UTC'
    });
    const days = WEEK_DAYS.map((day) => {
      const dayStr = String(day).padStart(2, '0');
      return new Date(
        `${REFERENCE_YEAR}-${REFERENCE_MONTH}-${dayStr}T00:00:00+00:00`
      );
    });
    setDow(days.map((date) => formatter.format(date)));
  }, [locale]);

  const schedule_theme = useTheme(scheduleTheme);

  const saveSchedule = async () => {
    try {
      await updateSchedule({
        schedule: schedule
          .filter((si: ScheduleItem) => !si.deleted)
          .map((condensed_si: ScheduleItem) => ({
            id: condensed_si.id,
            active: condensed_si.active,
            flags: condensed_si.flags,
            time: condensed_si.time,
            cmd_name: condensed_si.cmd_name,
            name: condensed_si.name
          }))
      });
      toast.success(LL.SCHEDULE_UPDATED());
    } catch (error: unknown) {
      const message = error instanceof Error ? error.message : String(error);
      toast.error(message);
    } finally {
      await fetchSchedule();
      setNumChanges(0);
    }
  };

  const editScheduleItem = (si: ScheduleItem) => {
    setCreating(false);
    setSelectedScheduleItem(si);
    setDialogOpen(true);
    if (si.o_name === undefined) {
      si.o_name = si.name;
    }
  };

  const onDialogClose = () => {
    setDialogOpen(false);
  };

  const onDialogCancel = async () => {
    await fetchSchedule().then(() => {
      setNumChanges(0);
    });
  };

  const onDialogSave = (updatedItem: ScheduleItem) => {
    setDialogOpen(false);
    void updateState(readSchedule(), (data: ScheduleItem[]) => {
      const new_data = creating
        ? [...data, updatedItem]
        : data.map((si) =>
            si.id === updatedItem.id ? { ...si, ...updatedItem } : si
          );

      setNumChanges(new_data.filter((si) => hasScheduleChanged(si)).length);

      return new_data;
    });
  };

  const addScheduleItem = () => {
    setCreating(true);
    const newItem: ScheduleItem = {
      id: Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID),
      ...DEFAULT_SCHEDULE_ITEM
    };
    setSelectedScheduleItem(newItem);
    setDialogOpen(true);
  };

  const filteredAndSortedSchedule = schedule
    .filter((si: ScheduleItem) => !si.deleted)
    .sort((a: ScheduleItem, b: ScheduleItem) => a.flags - b.flags);

  const dayBox = (si: ScheduleItem, flag: number, isLast = false) => {
    const dayIndex = Math.log(flag) / LOG_2;
    const isActive = (si.flags & flag) === flag;

    return (
      <>
        <Box>
          <Typography sx={{ fontSize: 11 }} color={isActive ? 'primary' : 'grey'}>
            {dow[dayIndex]}
          </Typography>
        </Box>
        {!isLast && <Divider orientation="vertical" flexItem />}
      </>
    );
  };

  const scheduleType = (si: ScheduleItem) => {
    const label = scheduleTypeLabels[si.flags];

    return (
      <Box>
        <Typography sx={{ fontSize: 11 }} color="primary">
          {label || ''}
        </Typography>
      </Box>
    );
  };

  const renderSchedule = () => {
    if (!schedule) {
      return (
        <FormLoader onRetry={fetchSchedule} errorMessage={error?.message || ''} />
      );
    }

    return (
      <Table
        data={{ nodes: filteredAndSortedSchedule }}
        theme={schedule_theme}
        layout={{ custom: true }}
      >
        {(tableList: ScheduleItem[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell />
                <HeaderCell stiff>{LL.SCHEDULE(0)}</HeaderCell>
                <HeaderCell stiff>
                  {LL.TIME(0)}/{LL.CONDITION()}
                </HeaderCell>
                <HeaderCell stiff>{LL.COMMAND(0)}</HeaderCell>
                <HeaderCell stiff>{LL.NAME(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((si: ScheduleItem) => (
                <Row key={si.id} item={si} onClick={() => editScheduleItem(si)}>
                  <Cell stiff>
                    <CircleIcon
                      color={si.active ? 'success' : 'error'}
                      sx={{
                        fontSize: ICON_SIZE,
                        ml: '4px',
                        position: 'relative',
                        top: '2px'
                      }}
                    />
                  </Cell>
                  <Cell stiff>
                    <Stack spacing={0.5} direction="row">
                      {si.flags > SCHEDULE_FLAG_THRESHOLD ? (
                        scheduleType(si)
                      ) : (
                        <>
                          {dayBox(si, ScheduleFlag.SCHEDULE_MON)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_TUE)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_WED)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_THU)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_FRI)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_SAT)}
                          {dayBox(si, ScheduleFlag.SCHEDULE_SUN, true)}
                        </>
                      )}
                    </Stack>
                  </Cell>
                  <Cell>{si.time === '' ? LL.SCHEDULER_HELP_2() : si.time}</Cell>
                  <Cell>{si.cmd_name}</Cell>
                  <Cell>{si.name}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Typography sx={{ mb: 2 }} color="warning" variant="body1">
        {LL.SCHEDULER_HELP_1()}.
      </Typography>
      {renderSchedule()}

      {selectedScheduleItem && (
        <SettingsSchedulerDialog
          open={dialogOpen}
          creating={creating}
          onClose={onDialogClose}
          onSave={onDialogSave}
          selectedItem={selectedScheduleItem}
          validator={schedulerItemValidation(schedule, selectedScheduleItem)}
          dow={dow}
          commandNames={commandNames.map((ci) => ci.name)}
        />
      )}

      <Box sx={{ display: 'flex', flexWrap: 'wrap' }}>
        <Box sx={{ flexGrow: 1 }}>
          {numChanges !== 0 && (
            <ButtonRow>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                onClick={onDialogCancel}
                color="secondary"
              >
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={saveSchedule}
              >
                {LL.APPLY_CHANGES(numChanges)}
              </Button>
            </ButtonRow>
          )}
        </Box>
        <Box sx={{ flexWrap: 'nowrap', whiteSpace: 'nowrap' }}>
          <ButtonRow>
            <Button
              startIcon={<AddIcon />}
              variant="outlined"
              color="primary"
              onClick={addScheduleItem}
            >
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default Scheduler;
