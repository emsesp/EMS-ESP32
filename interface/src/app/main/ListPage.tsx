import type { ReactNode } from 'react';

import BlockNavigation from 'components/routing/BlockNavigation';
import { Loader, Tag } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { PendingList, PendingState } from 'utils';

import { ChangesBar } from './ChangesBar';

interface ListPageProps<T> {
  list: PendingList<T>;
  children: ReactNode;
}

// loader, unsaved-changes guard and save bar around a list page that uses usePendingList
const ListPage = <T,>({ list, children }: ListPageProps<T>) => {
  const { LL } = useI18nContext();

  if (!list.data) {
    return <Loader error={list.error?.message} onRetry={() => void list.reload()} />;
  }

  return (
    <>
      {list.blocker && <BlockNavigation blocker={list.blocker} />}
      {children}
      {list.changes > 0 && (
        <ChangesBar
          message={LL.UNSAVED_CHANGES({ num: list.changes })}
          saving={list.saving}
          onDiscard={list.cancel}
          onSave={() => void list.save()}
        />
      )}
    </>
  );
};

// row class for the coloured left edge of a pending row
export const pendingClass = (state: PendingState) =>
  state ? `st-${state}` : undefined;

// "new" / "changed" after the name of a pending row
export const PendingTag = ({ state }: { state: PendingState }) => {
  const { LL } = useI18nContext();
  if (state === 'new') return <Tag kind="new">{LL.NEW(0)}</Tag>;
  if (state === 'mod') return <Tag kind="mod">{LL.MODIFIED()}</Tag>;
  return null;
};

export default ListPage;
