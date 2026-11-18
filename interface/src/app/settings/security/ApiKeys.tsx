import { memo, useState } from 'react';
import type { FormEvent } from 'react';

import * as SecurityApi from 'api/security';

import { useRequest } from 'alova/client';
import {
  Button,
  Dialog,
  IconButton,
  Loader,
  Message,
  Section,
  Segmented,
  SortHeader,
  Table,
  Tag,
  TextField,
  byNumber,
  byText,
  toast,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { ApiKeyType } from 'types';

const MAX_KEYS = 10;
const MAX_LABEL = 32;

const ApiKeys = () => {
  const { LL } = useI18nContext();

  const [creating, setCreating] = useState(false);
  const [label, setLabel] = useState('');
  const [write, setWrite] = useState(false);
  const [newKey, setNewKey] = useState<string>();
  const [revoking, setRevoking] = useState<ApiKeyType>();

  const { data, send: loadKeys, error } = useRequest(SecurityApi.readApiKeys);

  const { send: sendAction, loading: sending } = useRequest(
    (action: { action: string; label?: string; write?: boolean; id?: string }) =>
      SecurityApi.apiKeyAction(action),
    { immediate: false }
  );

  const openCreate = () => {
    setLabel('');
    setWrite(false);
    setCreating(true);
  };

  const create = async (e?: FormEvent) => {
    e?.preventDefault();
    if (!label.trim()) return;
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
    setRevoking(undefined);
    await sendAction({ action: 'revoke', id: key.id }).catch((err: Error) => {
      toast.error(err.message);
    });
    await loadKeys();
  };

  // navigator.clipboard is missing on plain http, e.g. when opened by IP address
  const copyKey = () => {
    if (!newKey) return;
    void Promise.resolve()
      .then(() => navigator.clipboard.writeText(newKey))
      .then(() => toast.success(LL.COPIED()))
      .catch(() => toast.error(LL.COPY_FAILED()));
  };

  const formatCreated = (created: number) =>
    created ? new Date(created * 1000).toLocaleString() : LL.UNKNOWN();

  const sort = useTableSort(data?.keys ?? [], {
    name: byText((k) => k.label),
    access: byNumber((k) => k.write),
    created: byNumber((k) => k.created)
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadKeys()} />;
  }

  const full = data.keys.length >= MAX_KEYS;

  return (
    <>
      <Section
        title={LL.API_KEYS()}
        icon="key"
        actions={
          <Button
            variant="contained"
            icon="add"
            disabled={full}
            onClick={openCreate}
          >
            {LL.CREATE_KEY()}
          </Button>
        }
      >
        <p className="cap sc-desc">{LL.API_KEYS_TEXT()}</p>
        {full && <p className="cap warnc">{LL.API_KEYS_LIMIT({ num: MAX_KEYS })}</p>}
        <Table stack>
          <thead>
            <tr>
              <SortHeader {...sort.header('name')}>{LL.NAME(0)}</SortHeader>
              <SortHeader {...sort.header('access')}>
                {LL.API_KEY_ACCESS()}
              </SortHeader>
              <SortHeader {...sort.header('created')}>{LL.CREATED()}</SortHeader>
              <th />
            </tr>
          </thead>
          <tbody>
            {sort.rows.map((k) => (
              <tr key={k.id}>
                <td className="mono">{k.label}</td>
                <td data-label={LL.API_KEY_ACCESS()}>
                  {k.write ? (
                    <Tag kind="mod">{LL.API_KEY_READ_WRITE()}</Tag>
                  ) : (
                    <Tag kind="sys">{LL.API_KEY_READ()}</Tag>
                  )}
                </td>
                <td data-label={LL.CREATED()} className="cap">
                  {formatCreated(k.created)}
                </td>
                <td className="act">
                  <IconButton
                    icon="delete"
                    label={LL.REVOKE()}
                    disabled={sending}
                    onClick={() => setRevoking(k)}
                  />
                </td>
              </tr>
            ))}
          </tbody>
        </Table>
        {data.keys.length === 0 && (
          <div className="tbl-empty">{LL.NO_API_KEYS()}</div>
        )}
      </Section>

      <Dialog
        open={creating}
        title={LL.CREATE_KEY()}
        onClose={() => setCreating(false)}
        actions={
          <>
            <Button color="secondary" onClick={() => setCreating(false)}>
              {LL.CANCEL()}
            </Button>
            <Button
              variant="contained"
              icon="key"
              disabled={!label.trim() || sending}
              onClick={() => void create()}
            >
              {LL.CREATE_KEY()}
            </Button>
          </>
        }
      >
        <form className="form" onSubmit={(e) => void create(e)}>
          <TextField
            label={LL.NAME(0)}
            name="label"
            fullWidth
            mono
            autoComplete="off"
            placeholder="home_assistant"
            value={label}
            maxLength={MAX_LABEL}
            onChange={(e) => setLabel(e.currentTarget.value.slice(0, MAX_LABEL))}
          />
          <Segmented
            label={LL.API_KEY_ACCESS()}
            fullWidth
            options={[
              { value: 'read', label: LL.API_KEY_READ() },
              { value: 'write', label: LL.API_KEY_READ_WRITE() }
            ]}
            value={write ? 'write' : 'read'}
            onChange={(v) => setWrite(v === 'write')}
          />
          <button type="submit" hidden />
        </form>
      </Dialog>

      <Dialog
        open={!!newKey}
        title={LL.NEW_API_KEY()}
        onClose={() => setNewKey(undefined)}
        actions={
          <>
            <Button icon="content_copy" onClick={copyKey}>
              {LL.COPY()}
            </Button>
            <Button variant="contained" onClick={() => setNewKey(undefined)}>
              {LL.CLOSE()}
            </Button>
          </>
        }
      >
        <Message level="warning">{LL.API_KEY_SHOW_ONCE()}</Message>
        <div className="mono sc-key">{newKey}</div>
      </Dialog>

      <Dialog
        open={!!revoking}
        title={LL.REVOKE_KEY()}
        onClose={() => setRevoking(undefined)}
        actions={
          <>
            <Button color="secondary" onClick={() => setRevoking(undefined)}>
              {LL.CANCEL()}
            </Button>
            <Button
              variant="contained"
              color="error"
              icon="delete"
              onClick={() => revoking && void revoke(revoking)}
            >
              {LL.REVOKE()}
            </Button>
          </>
        }
      >
        <p>{LL.REVOKE_KEY_TEXT({ name: revoking?.label ?? '' })}</p>
      </Dialog>
    </>
  );
};

export default memo(ApiKeys);
