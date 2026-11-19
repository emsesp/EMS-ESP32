import type { FC } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import Icon from './Icon';

export interface FieldFlagsProps {
  // changing the field only takes effect after a restart
  restart?: boolean | undefined;
  // the value differs from the saved one
  changed?: boolean | undefined;
}

// small markers after a field label
const FieldFlags: FC<FieldFlagsProps> = ({ restart, changed }) => {
  const { LL } = useI18nContext();
  return (
    <>
      {restart && (
        <Icon
          name="restart_alt"
          size="xs"
          className="muted"
          label={LL.NEEDS_RESTART()}
        />
      )}
      {changed && <span className="chg" role="img" aria-label={LL.MODIFIED()} />}
    </>
  );
};

export default FieldFlags;
