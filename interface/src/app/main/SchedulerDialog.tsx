import { useEffect, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid2 as Grid,
  TextField,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from '@mui/material';

import { dialogStyle } from 'CustomTheme';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';
import { BlockFormControlLabel, ValidatedTextField } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';
import { validate } from 'validators';

import { ScheduleFlag } from './types';
import type { ScheduleItem } from './types';

interface SchedulerDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: ScheduleItem) => void;
  selectedItem: ScheduleItem;
  validator: Schema;
  dow: string[];
}

const SchedulerDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator,
  dow
}: SchedulerDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ScheduleItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const [scheduleType, setScheduleType] = useState<ScheduleFlag>();

  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
      // set the flags based on type when page is loaded...
      // 0-127 is day schedule
      // 128 is timer
      // 129 is on change
      // 130 is on condition
      // 132 is immediate
      setScheduleType(
        selectedItem.flags < 128 ? ScheduleFlag.SCHEDULE_DAY : selectedItem.flags
      );
    }
  }, [open, selectedItem]);

  const save = async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  };

  const saveandactivate = async () => {
    editItem.active = true;
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      onSave(editItem);
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
    }
  };

  const remove = () => {
    editItem.deleted = true;
    onSave(editItem);
  };

  const getFlagDOWnumber = (newFlag: string[]) => {
    let new_flag = 0;
    for (const entry of newFlag) {
      new_flag |= Number(entry);
    }
    return new_flag & 127;
  };

  const getFlagDOWstring = (f: number) => {
    const new_flags: string[] = [];
    if ((f & 129) === 1) {
      new_flags.push('1');
    }
    if ((f & 130) === 2) {
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

    return new_flags;
  };

  const showDOW = (si: ScheduleItem, flag: number) => (
    <Typography
      sx={{ fontSize: 10 }}
      color={(si.flags & flag) === flag ? 'primary' : 'grey'}
    >
      {dow[Math.log(flag) / Math.log(2)]}
    </Typography>
  );

  const handleClose = (_event, reason: 'backdropClick' | 'escapeKeyDown') => {
    if (reason !== 'backdropClick') {
      onClose();
    }
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(0) : LL.EDIT()}&nbsp;
        {LL.SCHEDULE(1)}
      </DialogTitle>
      <DialogContent dividers>
        <ToggleButtonGroup
          size="small"
          color="secondary"
          value={scheduleType}
          exclusive
          disabled={!creating}
          onChange={(_event, flag: ScheduleFlag) => {
            if (flag !== null) {
              setFieldErrors(undefined); // clear any validation errors
              setScheduleType(flag);
              // wipe the time field when changing the schedule type
              setEditItem({ ...editItem, time: '' });
              // set the flags based on type
              // 0-127 is day schedule
              // 128 is timer
              // 129 is on change
              // 130 is on condition
              // 132 is immediate
              setEditItem(
                flag === ScheduleFlag.SCHEDULE_DAY
                  ? { ...editItem, flags: 0 }
                  : { ...editItem, flags: flag }
              );
            }
          }}
        >
          <ToggleButton value={ScheduleFlag.SCHEDULE_DAY}>
            <Typography
              sx={{ fontSize: 10 }}
              color={scheduleType === ScheduleFlag.SCHEDULE_DAY ? 'primary' : 'grey'}
            >
              {LL.SCHEDULE(0)}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_TIMER}>
            <Typography
              sx={{ fontSize: 10 }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_TIMER ? 'primary' : 'grey'
              }
            >
              {LL.TIMER(0)}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_ONCHANGE}>
            <Typography
              sx={{ fontSize: 10 }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_ONCHANGE ? 'primary' : 'grey'
              }
            >
              {LL.ONCHANGE()}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_CONDITION}>
            <Typography
              sx={{ fontSize: 10 }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_CONDITION ? 'primary' : 'grey'
              }
            >
              {LL.CONDITION()}
            </Typography>
          </ToggleButton>
          <ToggleButton value={ScheduleFlag.SCHEDULE_IMMEDIATE}>
            <Typography
              sx={{ fontSize: 10 }}
              color={
                scheduleType === ScheduleFlag.SCHEDULE_IMMEDIATE ? 'primary' : 'grey'
              }
            >
              {LL.IMMEDIATE()}
            </Typography>
          </ToggleButton>
        </ToggleButtonGroup>

        {scheduleType === ScheduleFlag.SCHEDULE_DAY && (
          <ToggleButtonGroup
            size="small"
            color="secondary"
            value={getFlagDOWstring(editItem.flags)}
            onChange={(_event, flag: string[]) => {
              setEditItem({ ...editItem, flags: getFlagDOWnumber(flag) });
            }}
          >
            <ToggleButton value="2">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_MON)}
            </ToggleButton>
            <ToggleButton value="4">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_TUE)}
            </ToggleButton>
            <ToggleButton value="8">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_WED)}
            </ToggleButton>
            <ToggleButton value="16">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_THU)}
            </ToggleButton>
            <ToggleButton value="32">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_FRI)}
            </ToggleButton>
            <ToggleButton value="64">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_SAT)}
            </ToggleButton>
            <ToggleButton value="1">
              {showDOW(editItem, ScheduleFlag.SCHEDULE_SUN)}
            </ToggleButton>
          </ToggleButtonGroup>
        )}

        {scheduleType !== ScheduleFlag.SCHEDULE_IMMEDIATE && (
          <>
            <Grid container>
              <BlockFormControlLabel
                control={
                  <Checkbox
                    checked={editItem.active}
                    onChange={updateFormValue}
                    name="active"
                  />
                }
                label={LL.ACTIVE()}
              />
            </Grid>
            <Grid container>
              {scheduleType === ScheduleFlag.SCHEDULE_DAY ||
              scheduleType === ScheduleFlag.SCHEDULE_TIMER ? (
                <>
                  <TextField
                    name="time"
                    type="time"
                    label={
                      scheduleType === ScheduleFlag.SCHEDULE_TIMER
                        ? LL.TIMER(1)
                        : LL.TIME(1)
                    }
                    value={editItem.time === '' ? '00:00' : editItem.time}
                    margin="normal"
                    onChange={updateFormValue}
                  />
                  {scheduleType === ScheduleFlag.SCHEDULE_TIMER && (
                    <Box color="warning.main" ml={2} mt={4}>
                      <Typography variant="body2">
                        {LL.SCHEDULER_HELP_2()}
                      </Typography>
                    </Box>
                  )}
                </>
              ) : (
                <TextField
                  name="time"
                  label={
                    scheduleType === ScheduleFlag.SCHEDULE_CONDITION
                      ? LL.CONDITION()
                      : scheduleType === ScheduleFlag.SCHEDULE_ONCHANGE
                        ? LL.ONCHANGE()
                        : LL.IMMEDIATE()
                  }
                  multiline
                  fullWidth
                  value={editItem.time === '00:00' ? '' : editItem.time}
                  margin="normal"
                  onChange={updateFormValue}
                />
              )}
            </Grid>
          </>
        )}
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="cmd"
          label={LL.COMMAND(0)}
          multiline
          fullWidth
          value={editItem.cmd}
          margin="normal"
          onChange={updateFormValue}
        />
        <TextField
          name="value"
          label={LL.VALUE(0)}
          multiline
          margin="normal"
          fullWidth
          value={editItem.value}
          onChange={updateFormValue}
        />
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="name"
          label={LL.NAME(0) + ' (' + LL.OPTIONAL() + ')'}
          value={editItem.name}
          fullWidth
          margin="normal"
          onChange={updateFormValue}
        />
      </DialogContent>

      <DialogActions>
        {!creating && (
          <Box flexGrow={1}>
            <Button
              startIcon={<RemoveIcon />}
              variant="outlined"
              color="warning"
              onClick={remove}
            >
              {LL.REMOVE()}
            </Button>
          </Box>
        )}
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={onClose}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={creating ? <AddIcon /> : <DoneIcon />}
          variant="outlined"
          onClick={save}
          color="primary"
        >
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
        {scheduleType === ScheduleFlag.SCHEDULE_IMMEDIATE && editItem.cmd !== '' && (
          <Button
            startIcon={<PlayArrowIcon />}
            variant="outlined"
            onClick={saveandactivate}
            color="success"
          >
            {LL.EXECUTE()}
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

export default SchedulerDialog;
