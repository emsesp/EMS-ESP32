import { useCallback, useMemo } from 'react';

import { ToggleButton, ToggleButtonGroup } from '@mui/material';

import OptionIcon from './OptionIcon';
import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

interface EntityMaskToggleProps {
  onUpdate: (de: DeviceEntity) => void;
  de: DeviceEntity;
}

// Available mask values
const MASK_VALUES = [
  DeviceEntityMask.DV_WEB_EXCLUDE, // 1
  DeviceEntityMask.DV_API_MQTT_EXCLUDE, // 2
  DeviceEntityMask.DV_READONLY, // 4
  DeviceEntityMask.DV_FAVORITE, // 8
  DeviceEntityMask.DV_DELETED // 128
];

/**
 * Converts an array of mask strings to a bitmask number
 */
const getMaskNumber = (newMask: string[]): number => {
  return newMask.reduce((mask, entry) => mask | Number(entry), 0);
};

/**
 * Converts a bitmask number to an array of mask strings
 */
const getMaskString = (mask: number): string[] => {
  return MASK_VALUES.filter((value) => (mask & value) === value).map((value) =>
    String(value)
  );
};

/**
 * Checks if a specific mask bit is set
 */
const hasMask = (mask: number, flag: number): boolean => (mask & flag) === flag;

const EntityMaskToggle = ({ onUpdate, de }: EntityMaskToggleProps) => {
  const handleChange = useCallback(
    (_event: unknown, mask: string[]) => {
      // Convert selected masks to a number
      const newMask = getMaskNumber(mask);
      const updatedDe = { ...de };

      // Apply business logic for mask interactions
      // If entity has no name and is set to readonly, also exclude from web
      if (updatedDe.n === '' && hasMask(newMask, DeviceEntityMask.DV_READONLY)) {
        updatedDe.m = newMask | DeviceEntityMask.DV_WEB_EXCLUDE;
      } else {
        updatedDe.m = newMask;
      }

      // If excluded from web, cannot be favorite
      if (hasMask(updatedDe.m, DeviceEntityMask.DV_WEB_EXCLUDE)) {
        updatedDe.m = updatedDe.m & ~DeviceEntityMask.DV_FAVORITE;
      }

      onUpdate(updatedDe);
    },
    [de, onUpdate]
  );

  // Memoize mask string value
  const maskStringValue = useMemo(() => getMaskString(de.m), [de.m]);

  // Memoize disabled states
  const isFavoriteDisabled = useMemo(
    () =>
      hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_DELETED) ||
      de.n === undefined,
    [de.m, de.n]
  );

  const isReadonlyDisabled = useMemo(
    () =>
      !de.w ||
      hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_FAVORITE),
    [de.w, de.m]
  );

  const isApiMqttExcludeDisabled = useMemo(
    () => de.n === '' || hasMask(de.m, DeviceEntityMask.DV_DELETED),
    [de.n, de.m]
  );

  const isWebExcludeDisabled = useMemo(
    () => de.n === undefined || hasMask(de.m, DeviceEntityMask.DV_DELETED),
    [de.n, de.m]
  );

  // Memoize mask flag checks
  const isFavoriteSet = useMemo(
    () => hasMask(de.m, DeviceEntityMask.DV_FAVORITE),
    [de.m]
  );
  const isReadonlySet = useMemo(
    () => hasMask(de.m, DeviceEntityMask.DV_READONLY),
    [de.m]
  );
  const isApiMqttExcludeSet = useMemo(
    () => hasMask(de.m, DeviceEntityMask.DV_API_MQTT_EXCLUDE),
    [de.m]
  );
  const isWebExcludeSet = useMemo(
    () => hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE),
    [de.m]
  );
  const isDeletedSet = useMemo(
    () => hasMask(de.m, DeviceEntityMask.DV_DELETED),
    [de.m]
  );

  return (
    <ToggleButtonGroup
      size="small"
      color="secondary"
      value={maskStringValue}
      onChange={handleChange}
    >
      <ToggleButton value="8" disabled={isFavoriteDisabled}>
        <OptionIcon type="favorite" isSet={isFavoriteSet} />
      </ToggleButton>
      <ToggleButton value="4" disabled={isReadonlyDisabled}>
        <OptionIcon type="readonly" isSet={isReadonlySet} />
      </ToggleButton>
      <ToggleButton value="2" disabled={isApiMqttExcludeDisabled}>
        <OptionIcon type="api_mqtt_exclude" isSet={isApiMqttExcludeSet} />
      </ToggleButton>
      <ToggleButton value="1" disabled={isWebExcludeDisabled}>
        <OptionIcon type="web_exclude" isSet={isWebExcludeSet} />
      </ToggleButton>
      <ToggleButton value="128">
        <OptionIcon type="deleted" isSet={isDeletedSet} />
      </ToggleButton>
    </ToggleButtonGroup>
  );
};

export default EntityMaskToggle;
