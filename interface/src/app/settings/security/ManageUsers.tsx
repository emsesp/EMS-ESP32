import { memo, useContext, useState } from 'react';

import * as SecurityApi from 'api/security';

import { ChangesBar } from 'app/main/ChangesBar';
import { PendingTag, pendingClass } from 'app/main/ListPage';
import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  Icon,
  Loader,
  Message,
  Section,
  SortHeader,
  Table,
  Tag,
  byNumber,
  byText,
  cx,
  rowAction,
  useTableSort
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SecuritySettingsType, UserType } from 'types';
import { useRest } from 'utils';
import type { PendingState } from 'utils';
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

  const removeUser = (u: UserType) => {
    setUsers(data.users.filter((x) => x.username !== u.username));
    setUser(undefined);
  };

  const rowState = (u: UserType): PendingState => {
    if (!dirtyFlags.includes(u.username)) return undefined;
    return orig.some((o) => o.username === u.username) ? 'mod' : 'new';
  };

  const roleTag = (u: UserType) =>
    u.admin ? (
      <Tag kind="warn" icon="admin_panel_settings">
        {LL.ROLE_ADMIN()}
      </Tag>
    ) : (
      <Tag kind="ok" icon="group">
        {LL.ROLE_GUEST()}
      </Tag>
    );

  const nameCell = (u: UserType, state: PendingState) => (
    <td>
      <span className="row sc-user">
        <Icon name="person" size="sm" className="muted" />
        {u.username}
        {u.username === me.username && <span className="cap">· {LL.YOU()}</span>}
        <PendingTag state={state} />
      </span>
    </td>
  );

  return (
    <>
      {rest.blocker && <BlockNavigation blocker={rest.blocker} />}
      <Section
        desc={LL.USERS_TEXT()}
        title={LL.USERS()}
        icon="group"
        actions={
          <Button icon="add" onClick={addUser}>
            {LL.ADD(0)}
          </Button>
        }
      >
        {noAdmin && <Message level="warning">{LL.USER_WARNING()}</Message>}
        <Table stack>
          <thead>
            <tr>
              <SortHeader {...sort.header('name')}>{LL.USERNAME(1)}</SortHeader>
              <SortHeader {...sort.header('role')}>{LL.ROLE()}</SortHeader>
              <th />
            </tr>
          </thead>
          <tbody>
            {sort.rows.map((u) => {
              const state = rowState(u);
              return (
                <tr
                  key={u.username}
                  {...rowAction(() => editUser(u), `${LL.EDIT()} ${u.username}`)}
                  className={cx('click', pendingClass(state))}
                >
                  {nameCell(u, state)}
                  <td data-label={LL.ROLE()}>{roleTag(u)}</td>
                  <td className="act" />
                </tr>
              );
            })}
            {removed.map((u) => (
              <tr key={u.username} className={pendingClass('del')}>
                {nameCell(u, 'del')}
                <td data-label={LL.ROLE()}>{roleTag(u)}</td>
                <td className="act">
                  <Button
                    size="sm"
                    color="secondary"
                    icon="undo"
                    onClick={() => setUsers([...data.users, u])}
                  >
                    {LL.UNDO()}
                  </Button>
                </td>
              </tr>
            ))}
          </tbody>
        </Table>
      </Section>

      {dirtyFlags.length > 0 && (
        <ChangesBar
          message={
            <>
              {LL.UNSAVED_CHANGES({ num: dirtyFlags.length })}
              {noAdmin && <span className="warnc"> · {LL.USER_WARNING()}</span>}
            </>
          }
          saving={rest.saving}
          saveDisabled={noAdmin}
          onDiscard={cancel}
          onSave={() => void save()}
        />
      )}

      {user && (
        <User
          user={user}
          creating={creating}
          validator={createUserValidator(data.users, creating)}
          onDone={doneEditing}
          onRemove={creating ? undefined : () => removeUser(user)}
          onClose={() => setUser(undefined)}
        />
      )}
    </>
  );
};

export default memo(ManageUsers);
