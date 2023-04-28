import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CheckCircleIcon from '@mui/icons-material/CheckCircle';
import DoneIcon from '@mui/icons-material/Done';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';

import {
  Box,
  Button,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Grid,
  TextField,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from '@mui/material';
import { useEffect, useState } from 'react';

import { ScheduleFlag } from './types';
import type { ScheduleItem } from './types';
import type Schema from 'async-validator';
import type { ValidateFieldsError } from 'async-validator';

import { BlockFormControlLabel, ValidatedTextField } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

import { updateValue } from 'utils';
import { validate } from 'validators';

type SettingsSchedulerDialogProps = {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ei: ScheduleItem) => void;
  selectedItem: ScheduleItem;
  validator: Schema;
  dow: string[];
};

const SettingsSchedulerDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator,
  dow
}: SettingsSchedulerDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ScheduleItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValue(setEditItem);

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const close = () => {
    onClose();
  };

  const save = async () => {
    try {
      setFieldErrors(undefined);
      await validate(validator, editItem);
      onSave(editItem);
    } catch (errors: any) {
      setFieldErrors(errors);
    }
  };

  const remove = () => {
    editItem.deleted = true;
    onSave(editItem);
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

  const showFlag = (si: ScheduleItem, flag: number) => (
    <Typography variant="button" sx={{ fontSize: 10 }} color={(si.flags & flag) === flag ? 'primary' : 'grey'}>
      {flag === ScheduleFlag.SCHEDULE_TIMER ? LL.TIMER(0) : dow[Math.log(flag) / Math.log(2)]}
    </Typography>
  );

  const isTimer = editItem.flags === ScheduleFlag.SCHEDULE_TIMER;

  return (
    <Dialog open={open} onClose={close}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.SCHEDULE(0)}
      </DialogTitle>
      <DialogContent dividers>
        <Box display="flex" flexWrap="wrap" mb={1}>
          <Box flexGrow={1}>
            <ToggleButtonGroup
              size="small"
              color="secondary"
              value={getFlagString(editItem.flags)}
              onChange={(event, flag) => {
                setEditItem({ ...editItem, flags: getFlagNumber(flag) & 127 });
              }}
            >
              <ToggleButton value="2">{showFlag(editItem, ScheduleFlag.SCHEDULE_MON)}</ToggleButton>
              <ToggleButton value="4">{showFlag(editItem, ScheduleFlag.SCHEDULE_TUE)}</ToggleButton>
              <ToggleButton value="8">{showFlag(editItem, ScheduleFlag.SCHEDULE_WED)}</ToggleButton>
              <ToggleButton value="16">{showFlag(editItem, ScheduleFlag.SCHEDULE_THU)}</ToggleButton>
              <ToggleButton value="32">{showFlag(editItem, ScheduleFlag.SCHEDULE_FRI)}</ToggleButton>
              <ToggleButton value="64">{showFlag(editItem, ScheduleFlag.SCHEDULE_SAT)}</ToggleButton>
              <ToggleButton value="1">{showFlag(editItem, ScheduleFlag.SCHEDULE_SUN)}</ToggleButton>
            </ToggleButtonGroup>
          </Box>
          <Box flexWrap="nowrap" whiteSpace="nowrap">
            {isTimer ? (
              <Button
                size="large"
                sx={{ bgcolor: '#334f65' }}
                variant="contained"
                onClick={() => {
                  setEditItem({ ...editItem, flags: 0 });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_TIMER)}
              </Button>
            ) : (
              <Button
                size="large"
                variant="outlined"
                onClick={() => {
                  setEditItem({ ...editItem, flags: ScheduleFlag.SCHEDULE_TIMER });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_TIMER)}
              </Button>
            )}
          </Box>
        </Box>
        <Grid container>
          <BlockFormControlLabel
            control={<Checkbox checked={editItem.active} onChange={updateFormValue} name="active" />}
            label={LL.ACTIVE()}
          />
          {editItem.active && (
            <Grid item sx={{ mt: 1 }}>
              <CheckCircleIcon sx={{ color: '#79D200', fontSize: 16, verticalAlign: 'middle' }} />
            </Grid>
          )}
        </Grid>
        <Grid container>
          <TextField
            name="time"
            type="time"
            label={isTimer ? LL.TIMER(1) : LL.TIME(1)}
            value={editItem.time}
            margin="normal"
            onChange={updateFormValue}
          />
          {isTimer && (
            <Box color="warning.main" ml={2} mt={4}>
              <Typography variant="body2">{LL.SCHEDULER_HELP_2()}</Typography>
            </Box>
          )}
        </Grid>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="cmd"
          label={LL.COMMAND(0)}
          fullWidth
          value={editItem.cmd}
          margin="normal"
          onChange={updateFormValue}
        />
        <TextField
          name="value"
          label={LL.VALUE(1)}
          multiline
          margin="normal"
          fullWidth
          value={editItem.value}
          onChange={updateFormValue}
        />
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="name"
          label={LL.NAME(0)}
          value={editItem.name}
          fullWidth
          margin="normal"
          onChange={updateFormValue}
        />
      </DialogContent>
      <DialogActions>
        {!creating && (
          <Box flexGrow={1}>
            <Button startIcon={<RemoveIcon />} variant="outlined" color="warning" onClick={remove}>
              {LL.REMOVE()}
            </Button>
          </Box>
        )}
        <Button startIcon={<CancelIcon />} variant="outlined" onClick={close} color="secondary">
          {LL.CANCEL()}
        </Button>
        <Button startIcon={creating ? <AddIcon /> : <DoneIcon />} variant="outlined" onClick={save} color="primary">
          {creating ? LL.ADD(0) : LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SettingsSchedulerDialog;
