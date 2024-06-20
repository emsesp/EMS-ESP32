import { useEffect, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
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
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
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
    if ((f & 131) === 128) {
      new_flags.push('128');
    }
    if ((f & 131) === 129) {
      new_flags.push('129');
    }
    if ((f & 131) === 130) {
      new_flags.push('130');
    }
    return new_flags;
  };

  const isTimer = editItem.flags === ScheduleFlag.SCHEDULE_TIMER;
  const isCondition = editItem.flags === ScheduleFlag.SCHEDULE_CONDITION;
  const isOnChange = editItem.flags === ScheduleFlag.SCHEDULE_ONCHANGE;

  const showFlag = (si: ScheduleItem, flag: number) => (
    <Typography
      variant="button"
      sx={{ fontSize: 10 }}
      color={
        (isOnChange && flag !== ScheduleFlag.SCHEDULE_ONCHANGE) ||
        (isCondition && flag !== ScheduleFlag.SCHEDULE_CONDITION)
          ? 'grey'
          : (si.flags & flag) === flag
            ? 'primary'
            : 'grey'
      }
    >
      {flag === ScheduleFlag.SCHEDULE_TIMER
        ? LL.TIMER(0)
        : flag === ScheduleFlag.SCHEDULE_ONCHANGE
          ? 'On Change'
          : flag === ScheduleFlag.SCHEDULE_CONDITION
            ? 'Condition'
            : dow[Math.log(flag) / Math.log(2)]}
    </Typography>
  );

  return (
    <Dialog sx={dialogStyle} open={open} onClose={close}>
      <DialogTitle>
        {creating ? LL.ADD(1) + ' ' + LL.NEW(0) : LL.EDIT()}&nbsp;
        {LL.SCHEDULE(1)}
      </DialogTitle>
      <DialogContent dividers>
        <Box display="flex" flexWrap="wrap" mb={1}>
          <Box flexGrow={1}>
            <ToggleButtonGroup
              size="small"
              color="secondary"
              value={getFlagString(editItem.flags)}
              onChange={(_event, flag: string[]) => {
                setEditItem({ ...editItem, flags: getFlagNumber(flag) & 127 });
              }}
            >
              <ToggleButton value="2">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_MON)}
              </ToggleButton>
              <ToggleButton value="4">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_TUE)}
              </ToggleButton>
              <ToggleButton value="8">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_WED)}
              </ToggleButton>
              <ToggleButton value="16">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_THU)}
              </ToggleButton>
              <ToggleButton value="32">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_FRI)}
              </ToggleButton>
              <ToggleButton value="64">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_SAT)}
              </ToggleButton>
              <ToggleButton value="1">
                {showFlag(editItem, ScheduleFlag.SCHEDULE_SUN)}
              </ToggleButton>
            </ToggleButtonGroup>
          </Box>
          <Box flexWrap="nowrap" whiteSpace="nowrap">
            {isTimer ? (
              <Button
                size="large"
                sx={{ bgcolor: '#334f65' }}
                variant="contained"
                onClick={() => {
                  setEditItem({ ...editItem, flags: ScheduleFlag.SCHEDULE_TIMER });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_TIMER)}
              </Button>
            ) : (
              <Button
                size="large"
                variant="outlined"
                onClick={() => {
                  setEditItem({
                    ...editItem,
                    flags: ScheduleFlag.SCHEDULE_TIMER
                  });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_TIMER)}
              </Button>
            )}
          </Box>
          <Box>
            {isOnChange ? (
              <Button
                size="large"
                sx={{ bgcolor: '#334f65' }}
                variant="contained"
                onClick={() => {
                  setEditItem({
                    ...editItem,
                    flags: ScheduleFlag.SCHEDULE_ONCHANGE
                  });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_ONCHANGE)}
              </Button>
            ) : (
              <Button
                size="large"
                variant="outlined"
                onClick={() => {
                  setEditItem({
                    ...editItem,
                    flags: ScheduleFlag.SCHEDULE_ONCHANGE
                  });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_ONCHANGE)}
              </Button>
            )}
          </Box>
          <Box>
            {isCondition ? (
              <Button
                size="large"
                sx={{ bgcolor: '#334f65' }}
                variant="contained"
                onClick={() => {
                  setEditItem({
                    ...editItem,
                    flags: ScheduleFlag.SCHEDULE_CONDITION
                  });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_CONDITION)}
              </Button>
            ) : (
              <Button
                size="large"
                variant="outlined"
                onClick={() => {
                  setEditItem({
                    ...editItem,
                    flags: ScheduleFlag.SCHEDULE_CONDITION
                  });
                }}
              >
                {showFlag(editItem, ScheduleFlag.SCHEDULE_CONDITION)}
              </Button>
            )}
          </Box>
        </Box>
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
          {isCondition || isOnChange ? (
            <TextField
              name="time"
              label={isCondition ? 'Condition' : 'On Change Value'}
              fullWidth
              value={editItem.time}
              margin="normal"
              onChange={updateFormValue}
            />
          ) : (
            <>
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
            </>
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
          onClick={close}
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
      </DialogActions>
    </Dialog>
  );
};

export default SchedulerDialog;
