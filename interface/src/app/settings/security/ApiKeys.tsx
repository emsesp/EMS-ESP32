import { memo, useState } from 'react';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import ContentCopyIcon from '@mui/icons-material/ContentCopy';
import DeleteIcon from '@mui/icons-material/Delete';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  IconButton,
  MenuItem,
  TextField
} from '@mui/material';

import * as SecurityApi from 'api/security';

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
import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { ButtonRow, FormLoader, MessageBox, SectionContent } from 'components';
import { toast } from 'components/toast';
import { useI18nContext } from 'i18n/i18n-react';
import type { ApiKeyType } from 'types';

const MAX_KEYS = 10;
const MAX_LABEL = 32;

const ApiKeys = () => {
  const { LL } = useI18nContext();

  const [creating, setCreating] = useState<boolean>(false);
  const [label, setLabel] = useState<string>('');
  const [write, setWrite] = useState<boolean>(false);
  const [newKey, setNewKey] = useState<string>();

  const {
    data,
    send: loadKeys,
    error
  } = useRequest(SecurityApi.readApiKeys, { initialData: { keys: [] } });

  const { send: sendAction } = useRequest(
    (action: { action: string; label?: string; write?: boolean; id?: string }) =>
      SecurityApi.apiKeyAction(action),
    { immediate: false }
  );

  const table_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(0, 1fr)) minmax(120px, max-content) minmax(120px, max-content) 60px;
    `,
    BaseRow: `
      font-size: 14px;
    `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;
      .th {
        padding: 8px;
        height: 36px;
        border-bottom: 1px solid #565656;
      }
    `,
    Row: `
      .td {
        padding: 8px;
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }
      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
      &:nth-of-type(even) .td {
        background-color: #1e1e1e;
      }
    `,
    BaseCell: `
      &:last-of-type {
        text-align: right;
      }
    `
  });

  const openCreate = () => {
    setLabel('');
    setWrite(false);
    setCreating(true);
  };

  const create = async () => {
    await sendAction({ action: 'create', label, write })
      .then((response) => {
        setNewKey((response as { key: string }).key);
        setCreating(false);
      })
      .catch((err: Error) => {
        toast.error(err.message);
      });
    await loadKeys();
  };

  const revoke = async (key: ApiKeyType) => {
    await sendAction({ action: 'revoke', id: key.id }).catch((err: Error) => {
      toast.error(err.message);
    });
    await loadKeys();
  };

  const copyKey = () => {
    if (newKey) {
      void navigator.clipboard.writeText(newKey).then(() => {
        toast.success(LL.COPIED());
      });
    }
  };

  const formatCreated = (created: number) =>
    created ? new Date(created * 1000).toLocaleString() : LL.UNKNOWN();

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadKeys} errorMessage={error?.message || ''} />;
    }

    const keys = data.keys.map((k) => ({ ...k }));

    return (
      <>
        <MessageBox level="info" message={LL.API_KEYS_TEXT()} sx={{ mb: 2 }} />
        <Table data={{ nodes: keys }} theme={table_theme} layout={{ custom: true }}>
          {(tableList: ApiKeyType[]) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell resize>{LL.NAME(0)}</HeaderCell>
                  <HeaderCell stiff>{LL.API_KEY_ACCESS()}</HeaderCell>
                  <HeaderCell stiff>{LL.CREATED()}</HeaderCell>
                  <HeaderCell stiff />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((k: ApiKeyType) => (
                  <Row key={k.id} item={k}>
                    <Cell>{k.label}</Cell>
                    <Cell stiff>
                      {k.write ? LL.API_KEY_READ_WRITE() : LL.API_KEY_READ()}
                    </Cell>
                    <Cell stiff>{formatCreated(k.created)}</Cell>
                    <Cell stiff>
                      <IconButton
                        size="small"
                        onClick={() => void revoke(k)}
                        aria-label={LL.REMOVE()}
                      >
                        <DeleteIcon />
                      </IconButton>
                    </Cell>
                  </Row>
                ))}
              </Body>
            </>
          )}
        </Table>

        <Box sx={{ display: 'flex', justifyContent: 'flex-end' }}>
          <ButtonRow>
            <Button
              startIcon={<AddIcon />}
              variant="outlined"
              color="secondary"
              disabled={data.keys.length >= MAX_KEYS}
              onClick={openCreate}
            >
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>

        <Dialog
          sx={dialogStyle}
          open={creating}
          onClose={() => setCreating(false)}
          fullWidth
          maxWidth="sm"
        >
          <DialogTitle>
            {LL.ADD(1)}&nbsp;{LL.API_KEYS()}
          </DialogTitle>
          <DialogContent dividers>
            <TextField
              label={LL.NAME(0)}
              value={label}
              onChange={(event) => setLabel(event.target.value.slice(0, MAX_LABEL))}
              fullWidth
              margin="normal"
            />
            <TextField
              select
              label={LL.API_KEY_ACCESS()}
              value={write ? 'write' : 'read'}
              onChange={(event) => setWrite(event.target.value === 'write')}
              fullWidth
              margin="normal"
            >
              <MenuItem value="read">{LL.API_KEY_READ()}</MenuItem>
              <MenuItem value="write">{LL.API_KEY_READ_WRITE()}</MenuItem>
            </TextField>
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              color="secondary"
              onClick={() => setCreating(false)}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={<AddIcon />}
              variant="outlined"
              color="primary"
              disabled={!label.trim()}
              onClick={() => void create()}
            >
              {LL.ADD(0)}
            </Button>
          </DialogActions>
        </Dialog>

        <Dialog
          sx={dialogStyle}
          open={!!newKey}
          onClose={() => setNewKey(undefined)}
          fullWidth
          maxWidth="sm"
        >
          <DialogTitle>{LL.API_KEYS()}</DialogTitle>
          <DialogContent dividers>
            <MessageBox
              level="warning"
              message={LL.API_KEY_SHOW_ONCE()}
              sx={{ mb: 2 }}
            />
            <TextField
              value={newKey || ''}
              fullWidth
              slotProps={{ input: { readOnly: true } }}
            />
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<ContentCopyIcon />}
              variant="outlined"
              color="primary"
              onClick={copyKey}
            >
              {LL.COPY()}
            </Button>
            <Button
              startIcon={<CloseIcon />}
              variant="outlined"
              color="secondary"
              onClick={() => setNewKey(undefined)}
            >
              {LL.CLOSE()}
            </Button>
          </DialogActions>
        </Dialog>
      </>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default memo(ApiKeys);
