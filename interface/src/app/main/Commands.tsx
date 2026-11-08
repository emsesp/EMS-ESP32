import { useState } from 'react';
import { useBlocker } from 'react-router';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Typography } from '@mui/material';

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

import { readCommands, writeCommands } from '../../api/app';
import CommandsDialog from './CommandsDialog';
import type { CommandItem, Commands as CommandsType } from './types';
import { commandItemValidation } from './validators';

const INTERVAL_DELAY = 30000;
const MIN_ID = -100;
const MAX_ID = 100;

const DEFAULT_COMMAND_ITEM: Omit<CommandItem, 'id'> = {
  cmd: '',
  value: '',
  name: '',
  deleted: false
};

const commandsTheme = {
  Table: `
    --data-table-library_grid-template-columns: repeat(1, minmax(100px, 1fr)) repeat(1, minmax(100px, 1fr)) 160px;
  `,
  BaseRow: `
    font-size: 14px;
    .td {
      height: 32px;
    }
  `,
  BaseCell: `
  &:nth-of-type(1) {
    padding: 8px;
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

const Commands = () => {
  const { LL } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [selectedItem, setSelectedItem] = useState<CommandItem>();
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  useLayoutTitle(LL.COMMANDS());

  const {
    data: commands,
    send: fetchCommands,
    error
  } = useRequest(readCommands, {
    initialData: []
  });

  const { send: updateCommands } = useRequest(
    (data: CommandsType) => writeCommands(data),
    { immediate: false }
  );

  const hasChanged = (ci: CommandItem) =>
    ci.id !== ci.o_id ||
    (ci.name || '') !== (ci.o_name || '') ||
    ci.cmd !== ci.o_cmd ||
    ci.value !== ci.o_value ||
    ci.deleted !== ci.o_deleted;

  useInterval(() => {
    if (numChanges === 0) {
      void fetchCommands();
    }
  }, INTERVAL_DELAY);

  const theme = useTheme(commandsTheme);

  const saveCommands = async () => {
    try {
      await updateCommands({
        commands: commands
          .filter((ci: CommandItem) => !ci.deleted)
          .map((ci: CommandItem) => ({
            id: ci.id,
            cmd: ci.cmd,
            value: ci.value,
            name: ci.name
          }))
      });
      toast.success(LL.UPDATED_OF(LL.COMMANDS()));
    } catch (error: unknown) {
      const message = error instanceof Error ? error.message : String(error);
      toast.error(message);
    } finally {
      await fetchCommands();
      setNumChanges(0);
    }
  };

  const editItem = (ci: CommandItem) => {
    setCreating(false);
    setSelectedItem(ci);
    setDialogOpen(true);
    if (ci.o_name === undefined) {
      ci.o_name = ci.name;
    }
  };

  const onDialogClose = () => {
    setDialogOpen(false);
  };

  const onDialogCancel = async () => {
    await fetchCommands().then(() => {
      setNumChanges(0);
    });
  };

  const onDialogSave = (updatedItem: CommandItem) => {
    setDialogOpen(false);
    void updateState(readCommands(), (data: CommandItem[]) => {
      const new_data = creating
        ? [...data, updatedItem]
        : data.map((ci) =>
            ci.id === updatedItem.id ? { ...ci, ...updatedItem } : ci
          );
      setNumChanges(new_data.filter((ci) => hasChanged(ci)).length);
      return new_data;
    });
  };

  const addItem = () => {
    setCreating(true);
    const newItem: CommandItem = {
      id: Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID),
      ...DEFAULT_COMMAND_ITEM
    };
    setSelectedItem(newItem);
    setDialogOpen(true);
  };

  const filteredCommands = commands.filter((ci: CommandItem) => !ci.deleted);

  const renderCommands = () => {
    if (!commands) {
      return (
        <FormLoader onRetry={fetchCommands} errorMessage={error?.message || ''} />
      );
    }

    return (
      <Table
        data={{ nodes: filteredCommands }}
        theme={theme}
        layout={{ custom: true }}
      >
        {(tableList: CommandItem[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell stiff>{LL.COMMAND(0)}</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.NAME(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((ci: CommandItem) => (
                <Row key={ci.id} item={ci} onClick={() => editItem(ci)}>
                  <Cell>{ci.cmd}</Cell>
                  <Cell>{ci.value}</Cell>
                  <Cell>{ci.name}</Cell>
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
        {LL.COMMANDS_HELP_1()}.
      </Typography>
      {renderCommands()}

      {selectedItem && (
        <CommandsDialog
          open={dialogOpen}
          creating={creating}
          onClose={onDialogClose}
          onSave={onDialogSave}
          selectedItem={selectedItem}
          validator={commandItemValidation(commands, selectedItem)}
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
                onClick={saveCommands}
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
              onClick={addItem}
            >
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default Commands;
