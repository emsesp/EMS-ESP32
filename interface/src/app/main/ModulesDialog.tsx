import { useState } from 'react';

import { Button, Dialog, Switch, TextArea } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import type { ModuleItem } from './types';

interface ModulesDialogProps {
  onClose: () => void;
  onSave: (mi: ModuleItem) => void;
  selectedItem: ModuleItem;
  // the module as loaded, for the changed markers
  original: ModuleItem | undefined;
}

const ModulesDialog = ({
  onClose,
  onSave,
  selectedItem,
  original
}: ModulesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<ModuleItem>(selectedItem);

  return (
    <Dialog
      open
      title={selectedItem.name}
      onClose={onClose}
      onSubmit={() => onSave(editItem)}
      actions={
        <>
          <Button icon="cancel" color="secondary" onClick={onClose}>
            {LL.CANCEL()}
          </Button>
          <Button
            type="submit"
            icon="check"
            disabled={
              editItem.enabled === selectedItem.enabled &&
              editItem.license === selectedItem.license
            }
          >
            {LL.UPDATE()}
          </Button>
        </>
      }
    >
      <div className="form">
        <Switch
          label={LL.ENABLED()}
          checked={editItem.enabled}
          changed={editItem.enabled !== original?.enabled}
          onChange={(enabled) => setEditItem((mi) => ({ ...mi, enabled }))}
        />
        <TextArea
          label={LL.LICENSE_KEY()}
          name="license"
          rows={6}
          value={editItem.license}
          changed={editItem.license !== original?.license}
          onChange={(e) => {
            const license = e.currentTarget.value;
            setEditItem((mi) => ({ ...mi, license }));
          }}
        />
      </div>
    </Dialog>
  );
};

export default ModulesDialog;
