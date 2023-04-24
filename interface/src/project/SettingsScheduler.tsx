import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import WarningIcon from '@mui/icons-material/Warning';

import { Box, Typography, Divider, Stack, Button } from '@mui/material';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useState, useEffect, useCallback } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';
import SettingsSchedulerDialog from './SettingsSchedulerDialog';
import * as EMSESP from './api';
import { ScheduleFlag } from './types';
import { schedulerItemValidation } from './validators';
import type { ScheduleItem } from './types';
import type { FC } from 'react';

import { ButtonRow, FormLoader, SectionContent, BlockNavigation } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import { extractErrorMessage } from 'utils';

const SettingsScheduler: FC = () => {
  const { LL, locale } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [schedule, setSchedule] = useState<ScheduleItem[]>([]);
  const [selectedScheduleItem, setSelectedScheduleItem] = useState<ScheduleItem>();
  const [dow, setDow] = useState<string[]>([]);
  const [errorMessage, setErrorMessage] = useState<string>();
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  function hasScheduleChanged(si: ScheduleItem) {
    return (
      si.id !== si.o_id ||
      (si?.name || '') !== (si?.o_name || '') ||
      si.active !== si.o_active ||
      si.deleted !== si.o_deleted ||
      si.flags !== si.o_flags ||
      si.time !== si.o_time ||
      si.cmd !== si.o_cmd ||
      si.value !== si.o_value
    );
  }

  useEffect(() => {
    if (schedule) {
      setNumChanges(schedule ? schedule.filter((si) => hasScheduleChanged(si)).length : 0);
    }
  }, [schedule]);

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

  const fetchSchedule = useCallback(async () => {
    try {
      const response = await EMSESP.readSchedule();
      setSchedule(
        response.data.schedule.map((si) => ({
          ...si,
          o_id: si.id,
          o_active: si.active,
          o_deleted: si.deleted,
          o_flags: si.flags,
          o_time: si.time,
          o_cmd: si.cmd,
          o_value: si.value,
          o_name: si.name
        }))
      );
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  // on mount
  useEffect(() => {
    const formatter = new Intl.DateTimeFormat(locale, { weekday: 'short', timeZone: 'UTC' });
    const days = [1, 2, 3, 4, 5, 6, 7].map((day) => {
      const dd = day < 10 ? `0${day}` : day;
      return new Date(`2017-01-${dd}T00:00:00+00:00`);
    });
    setDow(days.map((date) => formatter.format(date)));
    void fetchSchedule();
  }, [locale, fetchSchedule]);

  const saveSchedule = async () => {
    if (schedule) {
      try {
        const response = await EMSESP.writeSchedule({
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
        });
        if (response.status === 200) {
          toast.success(LL.SCHEDULE_UPDATED());
        } else {
          toast.error(LL.PROBLEM_UPDATING());
        }
        await fetchSchedule();
      } catch (error) {
        toast.error(extractErrorMessage(error, LL.PROBLEM_UPDATING()));
      }
    }
  };

  const editScheduleItem = useCallback((si: ScheduleItem) => {
    setCreating(false);
    setSelectedScheduleItem(si);
    setDialogOpen(true);
  }, []);

  const onDialogClose = () => {
    setDialogOpen(false);
  };

  const onDialogSave = (updatedItem: ScheduleItem) => {
    setDialogOpen(false);
    if (schedule && creating) {
      setSchedule([...schedule.filter((si) => creating || si.o_id !== updatedItem.o_id), updatedItem]);
    } else {
      setSchedule(schedule?.map((si) => (si.id === updatedItem.id ? { ...si, ...updatedItem } : si)));
    }
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
      return <FormLoader errorMessage={errorMessage} />;
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

    return (
      <Table
        data={{ nodes: schedule.filter((si) => !si.deleted).sort((a, b) => a.time.localeCompare(b.time)) }}
        theme={schedule_theme}
        layout={{ custom: true }}
      >
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell />
                <HeaderCell stiff>{LL.SCHEDULE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.TIME(0)}</HeaderCell>
                <HeaderCell stiff>{LL.COMMAND(0)}</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.NAME(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((si: ScheduleItem) => (
                <Row key={si.id} item={si} onClick={() => editScheduleItem(si)}>
                  <Cell stiff>
                    {si.active && <CheckCircleIcon sx={{ color: '#79D200', fontSize: 16, verticalAlign: 'middle' }} />}
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
    <SectionContent title={LL.SCHEDULER()} titleGutter>
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
          selectedSchedulerItem={selectedScheduleItem}
          validator={schedulerItemValidation()}
          dow={dow}
        />
      )}

      <Box display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges !== 0 && (
            <ButtonRow>
              <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => fetchSchedule()} color="secondary">
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={() => saveSchedule()}
              >
                {LL.APPLY_CHANGES(numChanges)}
              </Button>
            </ButtonRow>
          )}
        </Box>
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button startIcon={<AddIcon />} variant="outlined" color="secondary" onClick={() => addScheduleItem()}>
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default SettingsScheduler;
