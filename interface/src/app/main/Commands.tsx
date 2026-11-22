import { useState } from 'react';

import { callAction, readCommands, writeCommands } from 'api/app';

import { useRequest } from 'alova/client';
import {
  Button,
  IconButton,
  Section,
  SortHeader,
  Switch,
  Table,
  Tag,
  TextField,
  byNumber,
  byText,
  cx,
  rowAction,
  toast,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { usePendingList } from 'utils';
import type { PendingState } from 'utils';

import EntityDialog from './EntityDialog';
import ListPage, { PendingTag, pendingClass } from './ListPage';
import type { CommandItem } from './types';
import { commandItemValidation } from './validators';

const MIN_ID = -100;
const MAX_ID = 100;

// a random id in the range the firmware expects, not used by another row
const newId = (items: CommandItem[]) => {
  let id: number;
  do {
    id = Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID);
  } while (items.some((ci) => ci.id === id));
  return id;
};

interface Editing {
  // undefined while adding
  key?: string;
  state?: PendingState;
  item: CommandItem;
}

const Commands = () => {
  const { LL } = useI18nContext();
  const [editing, setEditing] = useState<Editing>();

  const list = usePendingList<CommandItem>({
    read: readCommands,
    write: (items) =>
      writeCommands({
        commands: items.map(({ id, cmd, value, name, guest }) => ({
          id,
          cmd,
          value,
          name,
          guest
        }))
      }),
    pollMs: 30000,
    savedText: LL.COMMANDS_UPDATED()
  });

  const { send: execute } = useRequest(
    (name: string) => callAction({ action: 'executeCommand', param: name }),
    { immediate: false }
  )
    .onSuccess(() => toast.success(LL.COMMAND_EXECUTED()))
    .onError((e) => toast.error(String(e.error?.message || e.error)));

  const add = () =>
    setEditing({
      item: { id: newId(list.items), cmd: '', value: '', name: '', guest: false }
    });

  const save = (item: CommandItem) => {
    if (editing?.key) list.update(editing.key, item);
    else list.add(item);
    setEditing(undefined);
  };

  const rows = list.rows;
  const sort = useTableSort(rows, {
    name: byText((r) => r.item.name),
    cmd: byText((r) => r.item.cmd),
    value: byText((r) => r.item.value),
    guest: byNumber((r) => r.item.guest)
  });

  return (
    <ListPage list={list}>
      <Section
        desc={`${LL.COMMANDS_HELP_1()}.`}
        title={LL.COMMANDS()}
        icon="playlist_play"
        actions={
          <Button icon="add" onClick={add}>
            {LL.ADD(0)}
          </Button>
        }
      >
        <div className="tbl-wrap">
          <Table stack>
            <thead>
              <tr>
                <SortHeader {...sort.header('name')}>{LL.NAME(0)}</SortHeader>
                <SortHeader {...sort.header('cmd')}>{LL.COMMAND(0)}</SortHeader>
                <SortHeader {...sort.header('value')}>{LL.VALUE()}</SortHeader>
                <SortHeader {...sort.header('guest')}>{LL.WHO_CAN_RUN()}</SortHeader>
                <th />
              </tr>
            </thead>
            <tbody>
              {sort.rows.map(({ key, item, state }) => (
                <tr
                  key={key}
                  {...(state !== 'del' &&
                    rowAction(
                      () => setEditing({ key, state, item }),
                      `${LL.EDIT()} ${item.name}`
                    ))}
                  className={cx(state !== 'del' && 'click', pendingClass(state))}
                >
                  <td>
                    {item.name}
                    <PendingTag state={state} />
                  </td>
                  <td data-label={LL.COMMAND(0)} className="mono lb">
                    {item.cmd}
                  </td>
                  <td data-label={LL.VALUE()} className="mono">
                    {item.value || <span className="muted">-</span>}
                  </td>
                  <td data-label={LL.WHO_CAN_RUN()}>
                    {item.guest ? (
                      <Tag kind="ok" icon="group">
                        {LL.ROLE_GUEST()}
                      </Tag>
                    ) : (
                      <Tag kind="warn" icon="admin_panel_settings">
                        {LL.ROLE_ADMIN()}
                      </Tag>
                    )}
                  </td>
                  <td className="act">
                    {state === 'del' ? (
                      <Button
                        size="sm"
                        color="neutral"
                        icon="undo"
                        onClick={() => list.undo(key)}
                      >
                        {LL.UNDO()}
                      </Button>
                    ) : (
                      !state &&
                      item.cmd !== '' && (
                        <IconButton
                          icon="play_arrow"
                          label={`${LL.RUN()} ${item.name}`}
                          onClick={(e) => {
                            e.stopPropagation();
                            void execute(item.name);
                          }}
                        />
                      )
                    )}
                  </td>
                </tr>
              ))}
              {rows.length === 0 && (
                <tr>
                  <td colSpan={5} className="tbl-empty">
                    {LL.CMD_NONE()}
                  </td>
                </tr>
              )}
            </tbody>
          </Table>
        </div>
      </Section>

      {editing && (
        <EntityDialog
          title={editing.key ? LL.EDIT_COMMAND() : LL.ADD_COMMAND()}
          item={editing.item}
          creating={!editing.key}
          validator={() =>
            commandItemValidation(
              list.items,
              editing.key ? editing.item.name : undefined
            )
          }
          onClose={() => setEditing(undefined)}
          onSave={save}
          onRemove={
            editing.key
              ? () => {
                  list.remove(editing.key!);
                  setEditing(undefined);
                }
              : undefined
          }
          extraActions={(v) =>
            // only the saved command can run, so not while it has unsaved edits
            editing.key &&
            !editing.state &&
            v.cmd !== '' &&
            JSON.stringify(v) === JSON.stringify(editing.item) && (
              <Button icon="play_arrow" onClick={() => void execute(v.name)}>
                {LL.RUN()}
              </Button>
            )
          }
          size="md"
        >
          {(f) => (
            <>
              <TextField label={LL.NAME(0)} mono fullWidth {...f.text('name')} />
              <TextField
                label={LL.COMMAND(0)}
                mono
                fullWidth
                placeholder="thermostat/hc1/seltemp"
                {...f.text('cmd')}
              />
              <TextField label={LL.VALUE()} mono fullWidth {...f.text('value')} />
              <Switch label={LL.COMMAND_GUEST()} {...f.check('guest')} />
            </>
          )}
        </EntityDialog>
      )}
    </ListPage>
  );
};

export default Commands;
