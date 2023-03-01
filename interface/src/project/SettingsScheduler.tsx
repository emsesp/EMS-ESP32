import { FC, useState, useEffect, useCallback } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';

import {
  Button,
  Typography,
  Box,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  ToggleButton,
  MenuItem,
  ToggleButtonGroup,
  Checkbox,
  TextField,
  Radio,
  RadioGroup,
  FormControlLabel
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
export const APIURL = window.location.origin + '/api/';

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
    description: ''
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
      --data-table-library_grid-template-columns: 140px 48px 324px 72px 240px repeat(1, minmax(100px, 1fr));
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
      },
      &:nth-of-type(3) {
        text-align: center;
      },
      &:nth-of-type(4) {
        text-align: center;
      },
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;
      .th {
        padding: 8px;
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
        padding: 8px;
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
        o_description: si.description
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
    let new_flags: string[] = [];
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
      (si?.description || '') !== (si?.o_description || '') ||
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
                description: condensed_si.description
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

  function showFlag(si: ScheduleItem, flag: number) {
    let text = '';
    if ((flag & ScheduleFlag.SCHEDULE_MON) === ScheduleFlag.SCHEDULE_MON) {
      text = dow[1];
    }
    if ((flag & ScheduleFlag.SCHEDULE_TUE) === ScheduleFlag.SCHEDULE_TUE) {
      text = dow[2];
    }
    if ((flag & ScheduleFlag.SCHEDULE_WED) === ScheduleFlag.SCHEDULE_WED) {
      text = dow[3];
    }
    if ((flag & ScheduleFlag.SCHEDULE_THU) === ScheduleFlag.SCHEDULE_THU) {
      text = dow[4];
    }
    if ((flag & ScheduleFlag.SCHEDULE_FRI) === ScheduleFlag.SCHEDULE_FRI) {
      text = dow[5];
    }
    if ((flag & ScheduleFlag.SCHEDULE_SAT) === ScheduleFlag.SCHEDULE_SAT) {
      text = dow[6];
    }
    if ((flag & ScheduleFlag.SCHEDULE_SUN) === ScheduleFlag.SCHEDULE_SUN) {
      text = dow[0];
    }
    if ((flag & ScheduleFlag.SCHEDULE_TIMER) === ScheduleFlag.SCHEDULE_TIMER) {
      text = LL.TIMER();
    }

    return (
      <Typography variant="button" sx={{ fontSize: 10 }} color={(si.flags & flag) === flag ? 'primary' : 'grey'}>
        {text}
      </Typography>
    );
  }

  const editScheduleItem = (si: ScheduleItem) => {
    if (si.description === undefined) {
      si.description = '';
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
      description: ''
    });
  };

  const updateScheduleItem = () => {
    if (scheduleItem) {
      const new_schedule = [...schedule.filter((si) => si.id !== scheduleItem.id), scheduleItem].sort((a, b) =>
        a.time.localeCompare(b.time)
      );
      setSchedule(new_schedule);
      setScheduleItem(undefined);
    }
  };

  const renderSchedule = () => {
    if (!schedule) {
      return <FormLoader errorMessage={errorMessage} />;
    }

    return (
      <Table data={{ nodes: schedule.filter((si) => !si.deleted) }} theme={schedule_theme} layout={{ custom: true }}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell stiff>{LL.NAME()}</HeaderCell>
                <HeaderCell stiff>
                  <CheckIcon sx={{ fontSize: 16, verticalAlign: 'middle' }} />
                </HeaderCell>
                <HeaderCell stiff>{LL.SCHEDULE()}</HeaderCell>
                <HeaderCell stiff>{LL.TIME()}</HeaderCell>
                <HeaderCell stiff>{LL.COMMAND()}</HeaderCell>
                <HeaderCell>{LL.VALUE(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((si: ScheduleItem) => (
                <Row key={si.id} item={si} onClick={() => editScheduleItem(si)}>
                  <Cell>{si.id}</Cell>
                  <Cell stiff>
                    <Checkbox
                      size="small"
                      checked={si.active}
                      onChange={() => {
                        si.active = !si.active;
                        setFlags(['']); // forces refresh
                      }}
                    />
                  </Cell>
                  <Cell stiff>
                    <ToggleButtonGroup
                      size="small"
                      color="secondary"
                      value={getFlagString(si.flags)}
                      onChange={(event, flag) => {
                        si.flags = getFlagNumber(flag);
                        if (si.flags & ScheduleFlag.SCHEDULE_TIMER) {
                          si.flags = ScheduleFlag.SCHEDULE_TIMER;
                        }
                        setFlags(['']); // forces refresh
                      }}
                    >
                      <ToggleButton value="2">{showFlag(si, ScheduleFlag.SCHEDULE_MON)}</ToggleButton>
                      <ToggleButton value="4">{showFlag(si, ScheduleFlag.SCHEDULE_TUE)}</ToggleButton>
                      <ToggleButton value="8">{showFlag(si, ScheduleFlag.SCHEDULE_WED)}</ToggleButton>
                      <ToggleButton value="16">{showFlag(si, ScheduleFlag.SCHEDULE_THU)}</ToggleButton>
                      <ToggleButton value="32">{showFlag(si, ScheduleFlag.SCHEDULE_FRI)}</ToggleButton>
                      <ToggleButton value="64">{showFlag(si, ScheduleFlag.SCHEDULE_SAT)}</ToggleButton>
                      <ToggleButton value="1">{showFlag(si, ScheduleFlag.SCHEDULE_SUN)}</ToggleButton>
                      <ToggleButton value="128">{showFlag(si, ScheduleFlag.SCHEDULE_TIMER)}</ToggleButton>
                    </ToggleButtonGroup>
                  </Cell>
                  <Cell>{si.time}</Cell>
                  <Cell>{si.cmd}</Cell>
                  <Cell>{si.value}</Cell>
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
        await validate(schedulerItemValidation(schedule, creating), scheduleItem);
        updateScheduleItem();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  const renderEditSchedule = () => {
    if (scheduleItem) {
      return (
        <Dialog open={!!scheduleItem} onClose={() => setScheduleItem(undefined)}>
          <DialogTitle>
            {creating ? LL.ADD(0) + ' ' + LL.NEW() + ' ' + LL.SCHEDULE() : LL.EDIT() + " '" + scheduleItem.id + "'"}
          </DialogTitle>
          <DialogContent dividers>
            {creating ? (
              <>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="id"
                  label={LL.NAME()}
                  value={scheduleItem.id}
                  fullWidth
                  margin="normal"
                  sx={{ width: '60ch' }}
                  onChange={updateValue(setScheduleItem)}
                />
                <RadioGroup
                  row
                  name="schedule-type"
                  onChange={(event) => {
                    if ((event.target as HTMLInputElement).value === 't') {
                      scheduleItem.flags = ScheduleFlag.SCHEDULE_TIMER;
                      scheduleItem.time = '01:00';
                    } else {
                      scheduleItem.flags = 0;
                    }
                    updateValue(setScheduleItem);
                    setFlags(['']); // refresh screen
                  }}
                >
                  <FormControlLabel value="w" control={<Radio />} label={LL.WEEKLY()} />
                  <FormControlLabel value="t" control={<Radio />} label={LL.TIMER()} />
                </RadioGroup>
              </>
            ) : (
              <Typography variant="h6" color="primary" sx={{ pb: 1 }}>
                {LL.TYPE()}:&nbsp;{scheduleItem.flags & ScheduleFlag.SCHEDULE_TIMER ? LL.TIMER() : LL.WEEKLY()}
              </Typography>
            )}
            <TextField
              name="description"
              label={LL.DESCRIPTION()}
              value={scheduleItem.description}
              fullWidth
              margin="normal"
              sx={{ width: '60ch' }}
              onChange={updateValue(setScheduleItem)}
            />
            <BlockFormControlLabel
              control={<Checkbox checked={scheduleItem.active} onChange={updateValue(setScheduleItem)} name="active" />}
              label={LL.ACTIVE()}
            />

            {(scheduleItem.flags & ScheduleFlag.SCHEDULE_TIMER) === ScheduleFlag.SCHEDULE_TIMER ? (
              <TextField
                name="time"
                label={LL.TIMER()}
                value={scheduleItem.time}
                fullWidth
                variant="outlined"
                onChange={updateValue(setScheduleItem)}
                margin="normal"
                select
              >
                <MenuItem value={'00:00'}>{LL.SCHEDULE_TIMER_1()}</MenuItem>
                <MenuItem value={'00:01'}>{LL.SCHEDULE_TIMER_2()}</MenuItem>
                <MenuItem value={'01:00'}>{LL.SCHEDULE_TIMER_3()}</MenuItem>
              </TextField>
            ) : (
              <TextField
                name="time"
                type="time"
                label={LL.TIME()}
                value={scheduleItem.time}
                margin="normal"
                onChange={updateValue(setScheduleItem)}
              />
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
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setScheduleItem(undefined)}
              color="secondary"
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<DoneIcon />}
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
