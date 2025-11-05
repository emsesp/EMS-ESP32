import { memo, useCallback, useContext, useMemo, useState } from 'react';
import { useBlocker } from 'react-router';

import CancelIcon from '@mui/icons-material/Cancel';
import CloseIcon from '@mui/icons-material/Close';
import DeleteIcon from '@mui/icons-material/Delete';
import CheckIcon from '@mui/icons-material/Done';
import EditIcon from '@mui/icons-material/Edit';
import PersonAddIcon from '@mui/icons-material/PersonAdd';
import VpnKeyIcon from '@mui/icons-material/VpnKey';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, IconButton } from '@mui/material';

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
import {
  BlockNavigation,
  ButtonRow,
  FormLoader,
  MessageBox,
  SectionContent
} from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SecuritySettingsType, UserType } from 'types';
import { useRest } from 'utils';
import { createUserValidator } from 'validators';

import GenerateToken from './GenerateToken';
import User from './User';

const ManageUsers = () => {
  const { loadData, saveData, saving, data, updateDataValue, errorMessage } =
    useRest<SecuritySettingsType>({
      read: SecurityApi.readSecuritySettings,
      update: SecurityApi.updateSecuritySettings
    });

  const [user, setUser] = useState<UserType>();
  const [creating, setCreating] = useState<boolean>(false);
  const [changed, setChanged] = useState<number>(0);
  const [generatingToken, setGeneratingToken] = useState<string>();
  const authenticatedContext = useContext(AuthenticatedContext);

  const blocker = useBlocker(changed !== 0);
  const { LL } = useI18nContext();

  const table_theme = useMemo(
    () =>
      useTheme({
        Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(0, 1fr)) minmax(120px, max-content) 120px;
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
      &:nth-of-type(2) {
        text-align: center;
      }
      &:last-of-type {
        text-align: right;
      }
    `
      }),
    []
  );

  const noAdminConfigured = useCallback(
    () => !data?.users.find((u) => u.admin),
    [data]
  );

  const removeUser = useCallback(
    (toRemove: UserType) => {
      if (!data) return;
      const users = data.users.filter((u) => u.username !== toRemove.username);
      updateDataValue({ ...data, users });
      setChanged(changed + 1);
    },
    [data, updateDataValue, changed]
  );

  const createUser = useCallback(() => {
    setCreating(true);
    setUser({
      username: '',
      password: '',
      admin: true
    });
  }, []);

  const editUser = useCallback((toEdit: UserType) => {
    setCreating(false);
    setUser({ ...toEdit });
  }, []);

  const cancelEditingUser = useCallback(() => {
    setUser(undefined);
  }, []);

  const doneEditingUser = useCallback(() => {
    if (user && data) {
      const users = [
        ...data.users.filter(
          (u: { username: string }) => u.username !== user.username
        ),
        user
      ];
      updateDataValue({ ...data, users });
      setUser(undefined);
      setChanged(changed + 1);
    }
  }, [user, data, updateDataValue, changed]);

  const closeGenerateToken = useCallback(() => {
    setGeneratingToken(undefined);
  }, []);

  const generateTokenForUser = useCallback((username: string) => {
    setGeneratingToken(username);
  }, []);

  const onSubmit = useCallback(async () => {
    await saveData();
    await authenticatedContext.refresh();
    setChanged(0);
  }, [saveData, authenticatedContext]);

  const onCancelSubmit = useCallback(async () => {
    await loadData();
    setChanged(0);
  }, [loadData]);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage || ''} />;
    }

    interface UserType2 {
      id: string;
      username: string;
      password: string;
      admin: boolean;
    }

    // add id to the type, needed for the table
    const user_table = useMemo(
      () =>
        data.users.map((u) => ({
          ...u,
          id: u.username
        })) as UserType2[],
      [data.users]
    );

    return (
      <>
        <Table
          data={{ nodes: user_table }}
          theme={table_theme}
          layout={{ custom: true }}
        >
          {(tableList: UserType2[]) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell resize>{LL.USERNAME(1)}</HeaderCell>
                  <HeaderCell stiff>{LL.IS_ADMIN(0)}</HeaderCell>
                  <HeaderCell stiff />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((u: UserType2) => (
                  <Row key={u.id} item={u}>
                    <Cell>{u.username}</Cell>
                    <Cell stiff>{u.admin ? <CheckIcon /> : <CloseIcon />}</Cell>
                    <Cell stiff>
                      <IconButton
                        size="small"
                        aria-label={LL.GENERATING_TOKEN()}
                        disabled={!authenticatedContext.me.admin}
                        onClick={() => generateTokenForUser(u.username)}
                      >
                        <VpnKeyIcon />
                      </IconButton>
                      <IconButton
                        size="small"
                        onClick={() => removeUser(u)}
                        aria-label={LL.REMOVE()}
                      >
                        <DeleteIcon />
                      </IconButton>
                      <IconButton
                        size="small"
                        onClick={() => editUser(u)}
                        aria-label={LL.EDIT()}
                      >
                        <EditIcon />
                      </IconButton>
                    </Cell>
                  </Row>
                ))}
              </Body>
            </>
          )}
        </Table>

        {noAdminConfigured() && (
          <MessageBox level="warning" message={LL.USER_WARNING()} my={2} />
        )}

        <Box display="flex" flexWrap="wrap">
          {changed !== 0 && (
            <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
              <ButtonRow>
                <Button
                  startIcon={<CancelIcon />}
                  disabled={saving}
                  variant="outlined"
                  color="secondary"
                  type="submit"
                  onClick={onCancelSubmit}
                >
                  {LL.CANCEL()}
                </Button>
                <Button
                  startIcon={<WarningIcon color="warning" />}
                  disabled={saving || noAdminConfigured()}
                  variant="contained"
                  color="info"
                  type="submit"
                  onClick={onSubmit}
                >
                  {LL.APPLY_CHANGES(changed)}
                </Button>
              </ButtonRow>
            </Box>
          )}
          <Box flexWrap="nowrap" whiteSpace="nowrap">
            <ButtonRow>
              <Button
                startIcon={<PersonAddIcon />}
                variant="outlined"
                color="secondary"
                onClick={createUser}
              >
                {LL.ADD(0)}
              </Button>
            </ButtonRow>
          </Box>
        </Box>

        <GenerateToken
          username={generatingToken || ''}
          onClose={closeGenerateToken}
        />
        {user && (
          <User
            user={user}
            setUser={setUser}
            creating={creating}
            onDoneEditing={doneEditingUser}
            onCancelEditing={cancelEditingUser}
            validator={createUserValidator(data.users, creating)}
          />
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content()}
    </SectionContent>
  );
};

export default memo(ManageUsers);
