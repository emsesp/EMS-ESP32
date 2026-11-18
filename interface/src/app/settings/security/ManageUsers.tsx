import { memo, useContext, useState } from 'react';

import * as SecurityApi from 'api/security';

import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  Icon,
  IconButton,
  Loader,
  Message,
  SaveBar,
  Section,
  SortHeader,
  Table,
  Tag,
  byNumber,
  byText,
  useTableSort
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SecuritySettingsType, UserType } from 'types';
import { useRest } from 'utils';
import { createUserValidator } from 'validators';

import User from './User';

// usernames that were added, removed or changed compared to the loaded list
const changedUsers = (orig: UserType[], users: UserType[]) => {
  const names = new Set([...orig, ...users].map((u) => u.username));
  return [...names].filter((name) => {
    const o = orig.find((u) => u.username === name);
    const u = users.find((x) => x.username === name);
    return !o || !u || o.admin !== u.admin || o.password !== u.password;
  });
};

const ManageUsers = () => {
  const { LL } = useI18nContext();
  const { me, refresh } = useContext(AuthenticatedContext);
  const rest = useRest<SecuritySettingsType>({
    read: SecurityApi.readSecuritySettings,
    update: SecurityApi.updateSecuritySettings
  });
  const { data, origData, dirtyFlags, setDirtyFlags, updateDataValue } = rest;

  const [user, setUser] = useState<UserType>();
  const [creating, setCreating] = useState(false);

  const sort = useTableSort(data?.users ?? [], {
    name: byText((u) => u.username),
    role: byNumber((u) => u.admin)
  });

  if (!data) {
    return <Loader error={rest.errorMessage} onRetry={() => void rest.loadData()} />;
  }

  const orig = origData?.users ?? [];
  const noAdmin = !data.users.some((u) => u.admin);
  const removed = orig.filter(
    (o) => !data.users.some((u) => u.username === o.username)
  );

  const setUsers = (users: UserType[]) => {
    updateDataValue({ ...data, users });
    setDirtyFlags(changedUsers(orig, users));
  };

  const addUser = () => {
    setCreating(true);
    setUser({ username: '', password: '', admin: false });
  };

  const editUser = (u: UserType) => {
    setCreating(false);
    setUser({ ...u });
  };

  const doneEditing = (edited: UserType) => {
    setUsers(
      creating
        ? [...data.users, edited]
        : data.users.map((u) => (u.username === edited.username ? edited : u))
    );
    setUser(undefined);
  };

  const cancel = () => {
    updateDataValue({ ...data, users: orig });
    setDirtyFlags([]);
  };

  const save = async () => {
    await rest.saveData();
    await refresh();
    await rest.loadData();
  };

  const rowState = (u: UserType) => {
    if (!dirtyFlags.includes(u.username)) return undefined;
    return orig.some((o) => o.username === u.username) ? 'st-mod' : 'st-new';
  };

  const roleTag = (u: UserType) =>
    u.admin ? (
      <Tag kind="mod">{LL.ROLE_ADMIN()}</Tag>
    ) : (
      <Tag kind="sys">{LL.ROLE_GUEST()}</Tag>
    );

  const nameCell = (u: UserType) => (
    <td>
      <span className="row sc-user">
        <Icon name="person" size="sm" className="muted" />
        {u.username}
        {u.username === me.username && <span className="cap">· {LL.YOU()}</span>}
      </span>
    </td>
  );

  return (
    <>
      {rest.blocker && <BlockNavigation blocker={rest.blocker} />}
      <Section
        title={LL.USERS()}
        icon="group"
        actions={
          <Button variant="contained" icon="person_add" onClick={addUser}>
            {LL.ADD_USER()}
          </Button>
        }
      >
        <p className="cap sc-desc">{LL.USERS_TEXT()}</p>
        {noAdmin && (
          <Message level="warning" className="sc-msg">
            {LL.USER_WARNING()}
          </Message>
        )}
        <Table stack>
          <thead>
            <tr>
              <SortHeader {...sort.header('name')}>{LL.USERNAME(1)}</SortHeader>
              <SortHeader {...sort.header('role')}>{LL.ROLE()}</SortHeader>
              <th />
            </tr>
          </thead>
          <tbody>
            {sort.rows.map((u) => (
              <tr key={u.username} className={rowState(u)}>
                {nameCell(u)}
                <td data-label={LL.ROLE()}>{roleTag(u)}</td>
                <td className="act">
                  <IconButton
                    icon="edit"
                    label={LL.EDIT()}
                    onClick={() => editUser(u)}
                  />
                  <IconButton
                    icon="delete"
                    label={LL.REMOVE()}
                    onClick={() =>
                      setUsers(data.users.filter((x) => x.username !== u.username))
                    }
                  />
                </td>
              </tr>
            ))}
            {removed.map((u) => (
              <tr key={u.username} className="st-del">
                {nameCell(u)}
                <td data-label={LL.ROLE()}>{roleTag(u)}</td>
                <td className="act">
                  <IconButton
                    icon="undo"
                    label={LL.UNDO()}
                    onClick={() => setUsers([...data.users, u])}
                  />
                </td>
              </tr>
            ))}
          </tbody>
        </Table>
      </Section>

      {dirtyFlags.length > 0 && (
        <SaveBar
          message={
            <>
              {LL.UNSAVED_CHANGES({ num: dirtyFlags.length })}
              {noAdmin && <span className="warnc"> · {LL.USER_WARNING()}</span>}
            </>
          }
        >
          <Button
            color="secondary"
            icon="undo"
            disabled={rest.saving}
            onClick={cancel}
          >
            {LL.CANCEL()}
          </Button>
          <Button
            variant="contained"
            icon="check"
            disabled={rest.saving || noAdmin}
            onClick={() => void save()}
          >
            {LL.SAVE()}
          </Button>
        </SaveBar>
      )}

      {user && (
        <User
          user={user}
          creating={creating}
          validator={createUserValidator(data.users, creating)}
          onDone={doneEditing}
          onClose={() => setUser(undefined)}
        />
      )}
    </>
  );
};

export default memo(ManageUsers);
