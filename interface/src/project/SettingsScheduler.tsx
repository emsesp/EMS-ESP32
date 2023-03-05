import { FC, useState, useEffect, useCallback } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';

import {
  Button,
  Typography,
  Box,
  Stack,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  ToggleButton,
  ToggleButtonGroup,
  Checkbox,
  TextField,
  Divider
} from '@mui/material';

import { useTheme } from '@table-library/react-table-library/theme';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import { useSnackbar } from 'notistack';

import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import CheckIcon from '@mui/icons-material/Check';
import WarningIcon from '@mui/icons-material/Warning';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import AddIcon from '@mui/icons-material/Add';

import {
  ValidatedTextField,
  ButtonRow,
  FormLoader,
  BlockFormControlLabel,
  SectionContent,
  BlockNavigation
} from 'components';

import { extractErrorMessage, updateValue } from 'utils';

import { validate } from 'validators';
import { schedulerItemValidation } from './validators';
import { ValidateFieldsError } from 'async-validator';

import { ScheduleItem, ScheduleFlag } from './types';

import { useI18nContext } from 'i18n/i18n-react';

import * as EMSESP from './api';

function makeid() {
  // TODO finish this!
  let result = '';
  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  const charactersLength = characters.length;
  let counter = 0;
  while (counter < 4) {
    result += characters.charAt(Math.floor(Math.random() * charactersLength));
    counter += 1;
  }
  return result;
}

const SettingsScheduler: FC = () => {
  const { LL, locale } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();

  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);

  const emptySchedule = {
    id: '0',
    active: false,
    deleted: false,
    flags: 0,
    time: '12:00',
    cmd: '',
    value: '',
    name: '',
    o_name: ''
  };
  const [schedule, setSchedule] = useState<ScheduleItem[]>([emptySchedule]);
  const [scheduleItem, setScheduleItem] = useState<ScheduleItem>();
  const [dow, setDow] = useState<string[]>([]);
  const [errorMessage, setErrorMessage] = useState<string>();
  const [creating, setCreating] = useState<boolean>(false);

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  // eslint-disable-next-line
  const [flags, setFlags] = useState(() => ['']);

  function getDayNames() {
    const formatter = new Intl.DateTimeFormat(locale, { weekday: 'short', timeZone: 'UTC' });
    const days = [1, 2, 3, 4, 5, 6, 7].map((day) => {
      const dd = day < 10 ? `0${day}` : day;
      return new Date(`2017-01-${dd}T00:00:00+00:00`);
    });
    return days.map((date) => formatter.format(date));
  }

  useEffect(() => {
    setNumChanges(getNumChanges());
  });

  const schedule_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 36px 324px 72px 240px repeat(1, minmax(100px, 1fr)) 152px;
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
        font-weight: 500;
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
      &.tr.tr-body.row-select.row-select-single-selected {
        background-color: #3d4752;
        color: white;
        font-weight: normal;
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
      setOriginalSchedule(response.data.schedule);
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
    setDow(getDayNames());
  }, [LL]);

  useEffect(() => {
    fetchSchedule();
  }, [fetchSchedule]);

  const setOriginalSchedule = (data: ScheduleItem[]) => {
    setSchedule(
      data.map((si) => ({
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
  };

  const getFlagNumber = (newFlag: string[]) => {
    let new_flag = 0;
    for (const entry of newFlag) {
      new_flag |= Number(entry);
    }
    return new_flag;
  };

  const getFlagString = (f: number) => {
    const new_flags: string[] = [];
    if ((f & 1) === 1) {
      new_flags.push('1');
    }
    if ((f & 2) === 2) {
      new_flags.push('2');
    }
    if ((f & 4) === 4) {
      new_flags.push('4');
    }
    if ((f & 8) === 8) {
      new_flags.push('8');
    }
    if ((f & 16) === 16) {
      new_flags.push('16');
    }
    if ((f & 32) === 32) {
      new_flags.push('32');
    }
    if ((f & 64) === 64) {
      new_flags.push('64');
    }
    if ((f & 128) === 128) {
      new_flags.push('128');
    }
    return new_flags;
  };

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

  const getNumChanges = () => {
    if (!schedule) {
      return 0;
    }
    return schedule.filter((si) => hasScheduleChanged(si)).length;
  };

  const saveSchedule = async () => {
    if (schedule) {
      try {
        const response = await EMSESP.writeSchedule({
          schedule: schedule
            .filter((si) => !si.deleted)
            .map((condensed_si) => {
              return {
                id: condensed_si.id,
                active: condensed_si.active,
                flags: condensed_si.flags,
                time: condensed_si.time,
                cmd: condensed_si.cmd,
                value: condensed_si.value,
                name: condensed_si.name
              };
            })
        });
        if (response.status === 200) {
          enqueueSnackbar(LL.SCHEDULE_SAVED(), { variant: 'success' });
        } else {
          enqueueSnackbar(LL.PROBLEM_UPDATING(), { variant: 'error' });
        }
      } catch (error) {
        enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_UPDATING()), { variant: 'error' });
      }
      setOriginalSchedule(schedule);
    }
  };

  function getFlagName(flag: number) {
    if ((flag & ScheduleFlag.SCHEDULE_MON) === ScheduleFlag.SCHEDULE_MON) {
      return dow[1];
    }
    if ((flag & ScheduleFlag.SCHEDULE_TUE) === ScheduleFlag.SCHEDULE_TUE) {
      return dow[2];
    }
    if ((flag & ScheduleFlag.SCHEDULE_WED) === ScheduleFlag.SCHEDULE_WED) {
      return dow[3];
    }
    if ((flag & ScheduleFlag.SCHEDULE_THU) === ScheduleFlag.SCHEDULE_THU) {
      return dow[4];
    }
    if ((flag & ScheduleFlag.SCHEDULE_FRI) === ScheduleFlag.SCHEDULE_FRI) {
      return dow[5];
    }
    if ((flag & ScheduleFlag.SCHEDULE_SAT) === ScheduleFlag.SCHEDULE_SAT) {
      return dow[6];
    }
    if ((flag & ScheduleFlag.SCHEDULE_SUN) === ScheduleFlag.SCHEDULE_SUN) {
      return dow[0];
    }
    if ((flag & ScheduleFlag.SCHEDULE_TIMER) === ScheduleFlag.SCHEDULE_TIMER) {
      return LL.TIMER();
    }
    return '';
  }

  const dayBox = (si: ScheduleItem, flag: number) => (
    <>
      <Box>
        <Typography sx={{ fontSize: 11 }} color={(si.flags & flag) === flag ? 'primary' : 'grey'}>
          {getFlagName(flag)}
        </Typography>
      </Box>
      <Divider orientation="vertical" flexItem />
    </>
  );

  const showFlag = (si: ScheduleItem, flag: number) => (
    <Typography variant="button" sx={{ fontSize: 10 }} color={(si.flags & flag) === flag ? 'primary' : 'grey'}>
      {getFlagName(flag)}
    </Typography>
  );

  const editScheduleItem = (si: ScheduleItem) => {
    if (si.name === undefined) {
      si.name = '';
    }
    setCreating(false);
    setScheduleItem(si);
  };

  const addScheduleItem = () => {
    setCreating(true);
    setScheduleItem({
      id: '',
      active: false,
      deleted: false,
      flags: 0,
      time: '12:00',
      cmd: '',
      value: '',
      name: ''
    });
  };

  const updateScheduleItem = () => {
    setSchedule([...schedule.filter((si) => creating || si.o_id !== scheduleItem.o_id), scheduleItem]);
    setScheduleItem(undefined);
  };

  const renderSchedule = () => {
    if (!schedule) {
      return <FormLoader errorMessage={errorMessage} />;
    }

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
                <HeaderCell stiff>{LL.SCHEDULE()}</HeaderCell>
                <HeaderCell stiff>{LL.TIME()}</HeaderCell>
                <HeaderCell stiff>{LL.COMMAND()}</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.NAME()}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((si: ScheduleItem) => (
                <Row key={si.id} item={si} onClick={() => editScheduleItem(si)}>
                  <Cell stiff>{si.active && <CheckIcon sx={{ fontSize: 16, verticalAlign: 'middle' }} />}</Cell>
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

  const removeScheduleItem = (si: ScheduleItem) => {
    si.deleted = true;
    setScheduleItem(si);
    updateScheduleItem();
  };

  const validateScheduleItem = async () => {
    if (scheduleItem) {
      try {
        setFieldErrors(undefined);
        await validate(schedulerItemValidation(schedule, scheduleItem.o_id), scheduleItem);
        updateScheduleItem();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  const closeDialog = () => {
    setScheduleItem(undefined);
    setFieldErrors();
  };

  const renderEditSchedule = () => {
    if (scheduleItem) {
      const isTimer = scheduleItem.flags === ScheduleFlag.SCHEDULE_TIMER;
      return (
        <Dialog open={!!scheduleItem} onClose={() => closeDialog()}>
          <DialogTitle>
            {creating ? LL.ADD(0) + ' ' + LL.NEW() : LL.EDIT()}&nbsp;{LL.SCHEDULE()}
          </DialogTitle>
          <DialogContent dividers>
            <Box display="flex" flexWrap="wrap" mb={2}>
              <Box flexGrow={1}>
                <ToggleButtonGroup
                  size="small"
                  color="secondary"
                  value={getFlagString(scheduleItem.flags)}
                  onChange={(event, flag) => {
                    scheduleItem.flags = getFlagNumber(flag) & 127;
                    setFlags(['']); // forces refresh
                  }}
                >
                  <ToggleButton value="2">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_MON)}</ToggleButton>
                  <ToggleButton value="4">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_TUE)}</ToggleButton>
                  <ToggleButton value="8">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_WED)}</ToggleButton>
                  <ToggleButton value="16">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_THU)}</ToggleButton>
                  <ToggleButton value="32">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_FRI)}</ToggleButton>
                  <ToggleButton value="64">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_SAT)}</ToggleButton>
                  <ToggleButton value="1">{showFlag(scheduleItem, ScheduleFlag.SCHEDULE_SUN)}</ToggleButton>
                </ToggleButtonGroup>
              </Box>
              <Box flexWrap="nowrap" whiteSpace="nowrap">
                {isTimer ? (
                  <Button
                    size="large"
                    sx={{ bgcolor: '#334f65' }}
                    variant="contained"
                    onClick={() => {
                      scheduleItem.flags = 0;
                      setFlags(['']); // forces refresh
                    }}
                  >
                    {showFlag(scheduleItem, ScheduleFlag.SCHEDULE_TIMER)}
                  </Button>
                ) : (
                  <Button
                    size="large"
                    variant="outlined"
                    onClick={() => {
                      scheduleItem.flags = ScheduleFlag.SCHEDULE_TIMER;
                      setFlags(['']); // forces refresh
                    }}
                  >
                    {showFlag(scheduleItem, ScheduleFlag.SCHEDULE_TIMER)}
                  </Button>
                )}
              </Box>
            </Box>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="name"
              label={LL.NAME()}
              value={scheduleItem.name}
              fullWidth
              margin="normal"
              sx={{ width: '60ch' }}
              onChange={updateValue(setScheduleItem)}
            />
            <BlockFormControlLabel
              control={<Checkbox checked={scheduleItem.active} onChange={updateValue(setScheduleItem)} name="active" />}
              label={LL.ACTIVE()}
            />
            <TextField
              name="time"
              type="time"
              label={isTimer ? LL.TIMER() : LL.TIME()}
              value={scheduleItem.time}
              margin="normal"
              onChange={updateValue(setScheduleItem)}
            />
            {isTimer && (
              <Box color="warning.main">
                <Typography variant="body2">{LL.SCHEDULER_HELP_2()}</Typography>
              </Box>
            )}
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="cmd"
              label={LL.COMMAND()}
              fullWidth
              value={scheduleItem.cmd}
              margin="normal"
              onChange={updateValue(setScheduleItem)}
            />
            <TextField
              name="value"
              label={LL.VALUE(1)}
              multiline
              margin="normal"
              fullWidth
              value={scheduleItem.value}
              onChange={updateValue(setScheduleItem)}
            />
          </DialogContent>
          <DialogActions>
            {!creating && (
              <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
                <Button
                  startIcon={<RemoveIcon />}
                  variant="outlined"
                  color="error"
                  onClick={() => removeScheduleItem(scheduleItem)}
                >
                  {LL.REMOVE()}
                </Button>
              </Box>
            )}
            <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => closeDialog()} color="secondary">
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={creating ? <AddIcon /> : <DoneIcon />}
              variant="outlined"
              type="submit"
              onClick={() => validateScheduleItem()}
              color="primary"
            >
              {creating ? LL.ADD(0) : LL.UPDATE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  return (
    <SectionContent title={LL.SCHEDULER()} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.SCHEDULER_HELP_1()}</Typography>
      </Box>
      {renderSchedule()}
      {renderEditSchedule()}
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
