import { memo, useState } from 'react';

import * as SecurityApi from 'api/security';

import { useRequest } from 'alova/client';
import {
  Button,
  ConfirmDialog,
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
import { formatDateTimeMinutes } from 'utils';

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

  const create = async () => {
    if (!label.trim() || sending) return;
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
    created ? formatDateTimeMinutes(new Date(created * 1000)) : LL.UNKNOWN();

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
        desc={LL.API_KEYS_TEXT()}
        title={LL.API_KEYS()}
        icon="key"
        actions={
          <Button icon="add" disabled={full} onClick={openCreate}>
            {LL.CREATE_KEY()}
          </Button>
        }
      >
        {full && (
          <Message level="warning">{LL.API_KEYS_LIMIT({ num: MAX_KEYS })}</Message>
        )}
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
                    <Tag kind="warn" icon="edit">
                      {LL.API_KEY_READ_WRITE()}
                    </Tag>
                  ) : (
                    <Tag kind="muted">{LL.API_KEY_READ()}</Tag>
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
            {data.keys.length === 0 && (
              <tr>
                <td colSpan={4} className="tbl-empty">
                  {LL.NO_API_KEYS()}
                </td>
              </tr>
            )}
          </tbody>
        </Table>
      </Section>

      <Dialog
        open={creating}
        title={LL.CREATE_KEY()}
        onClose={() => setCreating(false)}
        onSubmit={() => void create()}
        actions={
          <>
            <Button icon="cancel" color="neutral" onClick={() => setCreating(false)}>
              {LL.CANCEL()}
            </Button>
            <Button type="submit" icon="add" disabled={!label.trim() || sending}>
              {LL.CREATE_KEY()}
            </Button>
          </>
        }
      >
        <div className="form">
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
        </div>
      </Dialog>

      {/* the key is shown only once, so only the Close button closes this */}
      <Dialog
        open={!!newKey}
        title={LL.NEW_API_KEY()}
        actions={
          <>
            <Button
              color="neutral"
              icon="close"
              onClick={() => setNewKey(undefined)}
            >
              {LL.CLOSE()}
            </Button>
            <Button icon="content_copy" onClick={copyKey}>
              {LL.COPY()}
            </Button>
          </>
        }
      >
        <Message level="warning">{LL.API_KEY_SHOW_ONCE()}</Message>
        <div className="mono sc-key">{newKey}</div>
      </Dialog>

      <ConfirmDialog
        open={!!revoking}
        title={LL.REVOKE_KEY()}
        label={LL.REVOKE()}
        icon="delete"
        color="error"
        onClose={() => setRevoking(undefined)}
        onConfirm={() => revoking && revoke(revoking)}
      >
        {LL.REVOKE_KEY_TEXT({ name: revoking?.label ?? '' })}
      </ConfirmDialog>
    </>
  );
};

export default memo(ApiKeys);
