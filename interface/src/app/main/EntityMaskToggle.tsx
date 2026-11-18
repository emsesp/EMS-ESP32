import { Icon, type IconName, cx } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { TranslationFunctions } from 'i18n/i18n-types';

import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

export interface MaskOption {
  bit: DeviceEntityMask;
  icon: IconName;
  label: (LL: TranslationFunctions) => string;
  help: (LL: TranslationFunctions) => string;
}

export const MASKS: MaskOption[] = [
  {
    bit: DeviceEntityMask.DV_FAVORITE,
    icon: 'star',
    label: (LL) => LL.MASK_FAVORITE(),
    help: (LL) => LL.CUSTOMIZATIONS_HELP_2()
  },
  {
    bit: DeviceEntityMask.DV_READONLY,
    icon: 'edit_off',
    label: (LL) => LL.MASK_READONLY(),
    help: (LL) => LL.CUSTOMIZATIONS_HELP_3()
  },
  {
    bit: DeviceEntityMask.DV_API_MQTT_EXCLUDE,
    icon: 'comments_disabled',
    label: (LL) => LL.MASK_NO_API(),
    help: (LL) => LL.CUSTOMIZATIONS_HELP_4()
  },
  {
    bit: DeviceEntityMask.DV_WEB_EXCLUDE,
    icon: 'visibility_off',
    label: (LL) => LL.MASK_HIDDEN(),
    help: (LL) => LL.CUSTOMIZATIONS_HELP_5()
  },
  {
    bit: DeviceEntityMask.DV_DELETED,
    icon: 'delete_forever',
    label: (LL) => LL.MASK_REMOVED(),
    help: (LL) => LL.CUSTOMIZATIONS_HELP_6()
  }
];

const has = (m: number, bit: number) => (m & bit) === bit;

// whether the flag can be switched for this entity with its current mask
export const maskAllowed = (de: DeviceEntity, m: number, bit: DeviceEntityMask) => {
  switch (bit) {
    case DeviceEntityMask.DV_FAVORITE:
      return de.n !== undefined && !has(m, DeviceEntityMask.DV_WEB_EXCLUDE);
    case DeviceEntityMask.DV_READONLY:
      return de.w;
    case DeviceEntityMask.DV_API_MQTT_EXCLUDE:
      return de.n !== '' && !has(m, DeviceEntityMask.DV_DELETED);
    case DeviceEntityMask.DV_WEB_EXCLUDE:
      return de.n !== undefined && !has(m, DeviceEntityMask.DV_DELETED);
    default:
      return true;
  }
};

// the mask with one flag set or cleared; an entity without a name that can't be written
// is also hidden, and a hidden entity can't be a favorite
export const withMask = (de: DeviceEntity, m: number, bit: number, on: boolean) => {
  let next = on ? m | bit : m & ~bit;
  if (de.n === '' && has(next, DeviceEntityMask.DV_READONLY)) {
    next |= DeviceEntityMask.DV_WEB_EXCLUDE;
  }
  if (has(next, DeviceEntityMask.DV_WEB_EXCLUDE)) {
    next &= ~DeviceEntityMask.DV_FAVORITE;
  }
  return next;
};

interface EntityMaskToggleProps {
  de: DeviceEntity;
  // the mask shown, defaults to de.m
  mask?: number;
  onChange: (mask: number) => void;
}

// one toggle per mask flag, as in the customizations table
const EntityMaskToggle = ({ de, mask = de.m, onChange }: EntityMaskToggleProps) => {
  const { LL } = useI18nContext();

  return (
    <span className="cz-toggles" role="group" aria-label={LL.OPTIONS()}>
      {MASKS.map((o) => {
        const on = has(mask, o.bit);
        return (
          <button
            key={o.bit}
            type="button"
            className={cx('cz-tog', `b${o.bit}`, on && 'on')}
            aria-pressed={on}
            aria-label={o.label(LL)}
            title={o.label(LL)}
            disabled={!maskAllowed(de, mask, o.bit)}
            onClick={() => onChange(withMask(de, mask, o.bit, !on))}
          >
            <Icon name={o.icon} size="sm" />
          </button>
        );
      })}
    </span>
  );
};

export default EntityMaskToggle;
