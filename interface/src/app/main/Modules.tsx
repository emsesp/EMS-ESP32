import { useState } from 'react';

import { Button, Message, Section, Switch, Tag, cx } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { usePendingList } from 'utils';

import { readModules, writeModules } from '../../api/app';
import ListPage from './ListPage';
import './Modules.css';
import ModulesDialog from './ModulesDialog';
import type { ModuleItem } from './types';

const MODULE_PENDING = 1;

const Modules = () => {
  const { LL } = useI18nContext();
  const [editing, setEditing] = useState<{ key: string; item: ModuleItem }>();

  const list = usePendingList<ModuleItem>({
    read: readModules,
    write: (items) =>
      writeModules({
        modules: items.map((mi) => ({
          key: mi.key,
          enabled: mi.enabled,
          license: mi.license
        }))
      }),
    savedText: LL.MODULES_UPDATED()
  });

  // the module as loaded, for the changed markers
  const original = (key: string) => list.data?.[Number(key.slice(1))];

  return (
    <ListPage list={list}>
      <Section title={LL.MODULES()} icon="extension">
        <p className="cap mods-desc">{LL.MODULES_TEXT()}</p>
        {list.rows.length === 0 ? (
          <Message level="info">{LL.MODULES_NONE()}</Message>
        ) : (
          <div className="mods-cards">
            {list.rows.map(({ key, item: mi, state }) => (
              <div
                key={key}
                className={cx('mods-card', mi.enabled && 'on', state && 'changed')}
              >
                <Switch
                  label={mi.name}
                  helperText={`v${mi.version} · ${LL.BY_AUTHOR({ name: mi.author })}`}
                  checked={mi.enabled}
                  changed={mi.enabled !== original(key)?.enabled}
                  onChange={(enabled) => list.update(key, { ...mi, enabled })}
                />
                <div className="row wrap mods-status">
                  <Tag kind={mi.status === MODULE_PENDING ? 'warn' : 'ok'}>
                    {mi.status === MODULE_PENDING
                      ? LL.MODULE_PENDING()
                      : LL.MODULE_ACTIVATED()}
                  </Tag>
                  {mi.message && <span className="cap grow">{mi.message}</span>}
                  <Button
                    size="sm"
                    icon="key"
                    onClick={() => setEditing({ key, item: mi })}
                  >
                    {LL.LICENSE_KEY()}
                  </Button>
                </div>
              </div>
            ))}
          </div>
        )}
      </Section>

      {editing && (
        <ModulesDialog
          selectedItem={editing.item}
          original={original(editing.key)}
          onClose={() => setEditing(undefined)}
          onSave={(item) => {
            list.update(editing.key, item);
            setEditing(undefined);
          }}
        />
      )}
    </ListPage>
  );
};

export default Modules;
