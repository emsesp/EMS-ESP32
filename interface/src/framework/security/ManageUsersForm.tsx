import { FC, useContext, useState } from 'react';

import { Button, IconButton, Box } from '@mui/material';
import SaveIcon from '@mui/icons-material/Save';
import DeleteIcon from '@mui/icons-material/Delete';
import PersonAddIcon from '@mui/icons-material/PersonAdd';
import EditIcon from '@mui/icons-material/Edit';
import CheckIcon from '@mui/icons-material/Check';
import CloseIcon from '@mui/icons-material/Close';
import VpnKeyIcon from '@mui/icons-material/VpnKey';

import { Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import * as SecurityApi from '../../api/security';
import { SecuritySettings, User } from '../../types';
import { ButtonRow, FormLoader, MessageBox, SectionContent } from '../../components';
import { createUserValidator } from '../../validators';
import { useRest } from '../../utils';
import { AuthenticatedContext } from '../../contexts/authentication';

import GenerateToken from './GenerateToken';
import UserForm from './UserForm';

const ManageUsersForm: FC = () => {
  const { loadData, saving, data, setData, saveData, errorMessage } = useRest<SecuritySettings>({
    read: SecurityApi.readSecuritySettings,
    update: SecurityApi.updateSecuritySettings
  });

  const [user, setUser] = useState<User>();
  const [creating, setCreating] = useState<boolean>(false);
  const [generatingToken, setGeneratingToken] = useState<string>();
  const authenticatedContext = useContext(AuthenticatedContext);

  const table_theme = useTheme({
    BaseRow: `
      font-size: 14px;
      color: white;
  `,
    HeaderRow: `
      text-transform: uppercase;
      background-color: black;
      color: #90CAF9;
      border-bottom: 1px solid #e0e0e0;
    `,
    Row: `
      &:nth-of-type(odd) {
        background-color: #303030;
      }
      &:nth-of-type(even) {
        background-color: #1e1e1e;
      }
      border-top: 1px solid #565656;
      border-bottom: 1px solid #565656;
      position: relative;
      z-index: 1;
      &:not(:last-of-type) {
        margin-bottom: -1px;
      }
      &:not(:first-of-type) {
        margin-top: -1px;
      }
      &:hover {
        color: white;
      }
      `,
    BaseCell: `
      border-top: 1px solid transparent;
      border-right: 1px solid transparent;
      border-bottom: 1px solid transparent;
    `
  });

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const noAdminConfigured = () => !data.users.find((u) => u.admin);

    const removeUser = (toRemove: User) => {
      const users = data.users.filter((u) => u.id !== toRemove.id);
      setData({ ...data, users });
    };

    const createUser = () => {
      setCreating(true);
      setUser({
        id: '',
        username: '',
        password: '',
        admin: true
      });
    };

    const editUser = (toEdit: User) => {
      setCreating(false);
      setUser({ ...toEdit });
    };

    const cancelEditingUser = () => {
      setUser(undefined);
    };

    const doneEditingUser = () => {
      if (user) {
        const users = [...data.users.filter((u) => u.id !== user.id), user];
        setData({ ...data, users });
        setUser(undefined);
      }
    };

    const closeGenerateToken = () => {
      setGeneratingToken(undefined);
    };

    const generateToken = (id: string) => {
      setGeneratingToken(id);
    };

    const onSubmit = async () => {
      await saveData();
      authenticatedContext.refresh();
    };

    return (
      <>
        <Table data={{ nodes: data.users }} theme={table_theme}>
          {(tableList: any) => (
            <>
              <Header>
                <HeaderRow>
                  <HeaderCell>USERNAME</HeaderCell>
                  <HeaderCell>IS ADMIN</HeaderCell>
                  <HeaderCell />
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((u: User, index: number) => (
                  <Row key={u.id} item={u}>
                    <Cell>{u.id}</Cell>
                    <Cell>{u.admin ? <CheckIcon /> : <CloseIcon />}</Cell>
                    <Cell>
                      <IconButton
                        size="small"
                        disabled={!authenticatedContext.me.admin}
                        aria-label="Generate Token"
                        onClick={() => generateToken(u.id)}
                      >
                        <VpnKeyIcon />
                      </IconButton>
                      <IconButton size="small" aria-label="Delete" onClick={() => removeUser(u)}>
                        <DeleteIcon />
                      </IconButton>
                      <IconButton size="small" aria-label="Edit" onClick={() => editUser(u)}>
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
          <MessageBox level="warning" message="You must have at least one admin user configured" my={2} />
        )}

        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
            <Button
              startIcon={<SaveIcon />}
              disabled={saving || noAdminConfigured()}
              variant="outlined"
              color="primary"
              type="submit"
              onClick={onSubmit}
            >
              Save
            </Button>
          </Box>

          <Box flexWrap="nowrap" whiteSpace="nowrap">
            <ButtonRow>
              <Button startIcon={<PersonAddIcon />} variant="outlined" color="secondary" onClick={createUser}>
                Add
              </Button>
            </ButtonRow>
          </Box>
        </Box>

        <GenerateToken username={generatingToken} onClose={closeGenerateToken} />
        <UserForm
          user={user}
          setUser={setUser}
          creating={creating}
          onDoneEditing={doneEditingUser}
          onCancelEditing={cancelEditingUser}
          validator={createUserValidator(data.users, creating)}
        />
      </>
    );
  };

  return (
    <SectionContent title="Manage Users" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default ManageUsersForm;
