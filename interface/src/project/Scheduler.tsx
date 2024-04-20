import { useCallback, useEffect, useState } from 'react';
import type { FC } from 'react';
import { useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CircleIcon from '@mui/icons-material/Circle';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Divider, Stack, Typography } from '@mui/material';

import { Body, Cell, Header, HeaderCell, HeaderRow, Row, Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { updateState, useRequest } from 'alova';
import { BlockNavigation, ButtonRow, FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import * as EMSESP from './api';
import SettingsSchedulerDialog from './SchedulerDialog';
import { ScheduleFlag } from './types';
import type { Schedule, ScheduleItem } from './types';
import { schedulerItemValidation } from './validators';

const Scheduler: FC = () => {
  const { LL, locale } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [selectedScheduleItem, setSelectedScheduleItem] = useState<ScheduleItem>();
  const [dow, setDow] = useState<string[]>([]);
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  const {
    data: schedule,
    send: fetchSchedule,
    error
  } = useRequest(EMSESP.readSchedule, {
    initialData: [],
    force: true
  });

  const { send: writeSchedule } = useRequest((data: Schedule) => EMSESP.writeSchedule(data), {
    immediate: false
  });

  function hasScheduleChanged(si: ScheduleItem) {
    return (
      si.id !== si.o_id ||
      (si.name || '') !== (si.o_name || '') ||
      si.active !== si.o_active ||
      si.deleted !== si.o_deleted ||
      si.flags !== si.o_flags ||
      si.time !== si.o_time ||
      si.cmd !== si.o_cmd ||
      si.value !== si.o_value
    );
  }

  useEffect(() => {
    const formatter = new Intl.DateTimeFormat(locale, { weekday: 'short', timeZone: 'UTC' });
    const days = [1, 2, 3, 4, 5, 6, 7].map((day) => {
      const dd = day < 10 ? `0${day}` : day;
      return new Date(`2017-01-${dd}T00:00:00+00:00`);
    });
    setDow(days.map((date) => formatter.format(date)));
  }, [locale]);

  const schedule_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 36px 324px 50px 192px repeat(1, minmax(100px, 1fr)) 160px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      &:nth-of-type(2) {
        text-align: center;
      }
      &:nth-of-type(1) {
        text-align: center;
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
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }
      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
    `
  });

  const saveSchedule = async () => {
    await writeSchedule({
      schedule: schedule
        .filter((si) => !si.deleted)
        .map((condensed_si) => ({
          id: condensed_si.id,
          active: condensed_si.active,
          flags: condensed_si.flags,
          time: condensed_si.time,
          cmd: condensed_si.cmd,
          value: condensed_si.value,
          name: condensed_si.name
        }))
    })
      .then(() => {
        toast.success(LL.SCHEDULE_UPDATED());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        await fetchSchedule();
        setNumChanges(0);
      });
  };

  const editScheduleItem = useCallback((si: ScheduleItem) => {
    setCreating(false);
    setSelectedScheduleItem(si);
    setDialogOpen(true);
  }, []);

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

    updateState('schedule', (data: ScheduleItem[]) => {
      const new_data = creating
        ? [...data.filter((si) => creating || si.o_id !== updatedItem.o_id), updatedItem]
        : data.map((si) => (si.id === updatedItem.id ? { ...si, ...updatedItem } : si));

      setNumChanges(new_data.filter((si) => hasScheduleChanged(si)).length);

      return new_data;
    });
  };

  const addScheduleItem = () => {
    setCreating(true);
    setSelectedScheduleItem({
      id: Math.floor(Math.random() * (Math.floor(200) - 100) + 100),
      active: false,
      deleted: false,
      flags: 0,
      time: '12:00',
      cmd: '',
      value: '',
      name: ''
    });
    setDialogOpen(true);
  };

  const renderSchedule = () => {
    if (!schedule) {
      return <FormLoader onRetry={fetchSchedule} errorMessage={error?.message} />;
    }

    const dayBox = (si: ScheduleItem, flag: number) => (
      <>
        <Box>
          <Typography sx={{ fontSize: 11 }} color={(si.flags & flag) === flag ? 'primary' : 'grey'}>
            {flag === ScheduleFlag.SCHEDULE_TIMER ? LL.TIMER(0) : dow[Math.log(flag) / Math.log(2)]}
          </Typography>
        </Box>
        <Divider orientation="vertical" flexItem />
      </>
    );

    useLayoutTitle(LL.SCHEDULER());

    return (
      <Table
        data={{ nodes: schedule.filter((si) => !si.deleted).sort((a, b) => a.time.localeCompare(b.time)) }}
        theme={schedule_theme}
        layout={{ custom: true }}
      >
        {(tableList: ScheduleItem[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell />
                <HeaderCell stiff>{LL.SCHEDULE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.TIME(0)}</HeaderCell>
                <HeaderCell stiff>{LL.COMMAND(0)}</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(1)}</HeaderCell>
                <HeaderCell stiff>{LL.NAME(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((si: ScheduleItem) => (
                <Row key={si.id} item={si} onClick={() => editScheduleItem(si)}>
                  <Cell stiff>
                    {si.active ? (
                      <CircleIcon color="success" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    ) : (
                      <CircleIcon color="error" sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                    )}
                  </Cell>
                  <Cell stiff>
                    <Stack spacing={1} direction="row">
                      <Divider orientation="vertical" flexItem />
                      {dayBox(si, ScheduleFlag.SCHEDULE_MON)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_TUE)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_WED)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_THU)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_FRI)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_SAT)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_SUN)}
                      {dayBox(si, ScheduleFlag.SCHEDULE_TIMER)}
                    </Stack>
                  </Cell>
                  <Cell>{si.time}</Cell>
                  <Cell>{si.cmd}</Cell>
                  <Cell>{si.value}</Cell>
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
      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.SCHEDULER_HELP_1()}</Typography>
      </Box>
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
        />
      )}

      <Box mt={1} display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges !== 0 && (
            <ButtonRow>
              <Button startIcon={<CancelIcon />} variant="outlined" onClick={onDialogCancel} color="secondary">
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
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button startIcon={<AddIcon />} variant="outlined" color="secondary" onClick={addScheduleItem}>
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default Scheduler;
