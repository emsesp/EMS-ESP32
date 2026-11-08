import { useEffect, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutlined';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  TextField
} from '@mui/material';

import { callAction } from '@/api/app';
import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { ValidatedTextField } from 'components';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import { updateValue } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

import type { CommandItem } from './types';

interface CommandsDialogProps {
  open: boolean;
  creating: boolean;
  onClose: () => void;
  onSave: (ci: CommandItem) => void;
  selectedItem: CommandItem;
  validator: Schema;
}

const CommandsDialog = ({
  open,
  creating,
  onClose,
  onSave,
  selectedItem,
  validator
}: CommandsDialogProps) => {
  const { LL } = useI18nContext();
  const [hasChanges, setHasChanges] = useState<boolean>(false);
  const [editItem, setEditItem] = useState<CommandItem>(selectedItem);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateFormValue = updateValue(
    setEditItem as unknown as React.Dispatch<
      React.SetStateAction<Record<string, unknown>>
    >
  );

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
      setEditItem(selectedItem);
    }
  }, [open, selectedItem]);

  const hasChanged = (ci: CommandItem) =>
    ci.id !== ci.o_id ||
    (ci.name || '') !== (ci.o_name || '') ||
    ci.cmd !== ci.o_cmd ||
    ci.value !== ci.o_value ||
    ci.deleted !== ci.o_deleted;

  useEffect(() => {
    setHasChanges(hasChanged(editItem));
  }, [editItem]);

  const handleSave = async (itemToSave: CommandItem) => {
    try {
      setFieldErrors(undefined);
      await validate(validator, itemToSave);
      onSave(itemToSave);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
    } finally {
      // setHasChanges(false);
    }
  };

  const save = async () => {
    await handleSave(editItem);
  };

  const { send: executeCommand } = useRequest(
    (id: string) => callAction({ action: 'executeCommand', param: id }),
    { immediate: false }
  )
    .onSuccess(() => {
      toast.success(LL.COMMAND_EXECUTED());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const execute = async () => {
    await executeCommand(editItem.name);
  };

  const remove = () => {
    onSave({ ...editItem, deleted: true });
  };

  const handleClose = (
    _event: React.SyntheticEvent,
    reason: 'backdropClick' | 'escapeKeyDown'
  ) => {
    if (reason !== 'backdropClick') {
      onClose();
    }
  };

  return (
    <Dialog sx={dialogStyle} open={open} onClose={handleClose}>
      <DialogTitle>
        {creating ? `${LL.ADD(1)} ` : LL.UPDATE() + '/' + LL.RUN()}&nbsp;
        {LL.COMMAND(0)}
      </DialogTitle>
      <DialogContent dividers>
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
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
          fieldErrors={fieldErrors || {}}
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
          <Box sx={{ flexGrow: 1 }}>
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

        {hasChanges && (
          <Button
            startIcon={creating ? <AddIcon /> : <DoneIcon />}
            variant="outlined"
            onClick={save}
            color="primary"
          >
            {creating ? LL.ADD(0) : LL.UPDATE()}
          </Button>
        )}

        {!creating && !hasChanges && editItem.cmd !== '' && (
          <Button
            startIcon={<PlayArrowIcon />}
            variant="outlined"
            onClick={execute}
            color="success"
          >
            {LL.RUN()}
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );
};

export default CommandsDialog;
