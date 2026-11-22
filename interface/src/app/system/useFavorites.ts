import { useMemo } from 'react';

import { toast } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { writeCustomizationEntities } from '../../api/app';
import {
  createMaskedEntityId,
  maskAllowed,
  withMask
} from '../main/EntityMaskToggle';
import { DeviceEntityMask } from '../main/types';
import { isFavorite } from './model';
import type { SysItem } from './model';
import type { SystemData } from './useSystemData';

export interface Favorites {
  items: SysItem[];
  // undefined when the star cannot be changed (not an administrator, no entity info)
  toggle: ((item: SysItem) => void) | undefined;
  canToggle: (item: SysItem) => boolean;
}

// The star sets the favorite flag of the entity on the device, the same flag the
// Devices page and the Dashboard use. Only administrators can change it.
export const useFavorites = (data: SystemData): Favorites => {
  const { LL } = useI18nContext();
  const items = useMemo(() => data.items.filter(isFavorite), [data.items]);

  // the current mask comes with the value; the entity list is only loaded once
  const maskOf = (i: SysItem): number => parseInt(i.dv.id.slice(0, 2), 16);
  const canToggle = (i: SysItem): boolean =>
    !!i.ent && maskAllowed(i.ent, maskOf(i), DeviceEntityMask.DV_FAVORITE);

  const setFavorite = async (i: SysItem, on: boolean) => {
    if (!i.ent) return;
    const mask = withMask(i.ent, maskOf(i), DeviceEntityMask.DV_FAVORITE, on);
    try {
      await writeCustomizationEntities({
        id: i.devId,
        entity_ids: [createMaskedEntityId({ ...i.ent, m: mask })]
      });
      await data.reload();
      // removing is not asked about; it can be undone for a while instead
      if (!on) {
        toast.info(LL.SYS_FAV_REMOVED(), {
          label: LL.UNDO(),
          onClick: () => void setFavorite(i, true)
        });
      }
    } catch (error) {
      toast.error(error instanceof Error ? error.message : String(error));
    }
  };

  const toggle = (i: SysItem) => void setFavorite(i, !isFavorite(i));

  return { items, toggle, canToggle };
};
