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
  const handleChange = (_event: unknown, mask: string[]) => {
    // Convert selected masks to a number
    const newMask = getMaskNumber(mask);

    // Apply business logic for mask interactions
    // If entity has no name and is set to readonly, also exclude from web
    if (de.n === '' && hasMask(newMask, DeviceEntityMask.DV_READONLY)) {
      de.m = newMask | DeviceEntityMask.DV_WEB_EXCLUDE;
    } else {
      de.m = newMask;
    }

    // If excluded from web, cannot be favorite
    if (hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE)) {
      de.m = de.m & ~DeviceEntityMask.DV_FAVORITE;
    }

    onUpdate(de);
  };

  // Check if favorite button should be disabled
  const isFavoriteDisabled =
    hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_DELETED) ||
    de.n === undefined;

  // Check if readonly button should be disabled
  const isReadonlyDisabled =
    !de.w ||
    hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_FAVORITE);

  // Check if api/mqtt exclude button should be disabled
  const isApiMqttExcludeDisabled =
    de.n === '' || hasMask(de.m, DeviceEntityMask.DV_DELETED);

  // Check if web exclude button should be disabled
  const isWebExcludeDisabled =
    de.n === undefined || hasMask(de.m, DeviceEntityMask.DV_DELETED);

  return (
    <ToggleButtonGroup
      size="small"
      color="secondary"
      value={getMaskString(de.m)}
      onChange={handleChange}
    >
      <ToggleButton value="8" disabled={isFavoriteDisabled}>
        <OptionIcon
          type="favorite"
          isSet={hasMask(de.m, DeviceEntityMask.DV_FAVORITE)}
        />
      </ToggleButton>
      <ToggleButton value="4" disabled={isReadonlyDisabled}>
        <OptionIcon
          type="readonly"
          isSet={hasMask(de.m, DeviceEntityMask.DV_READONLY)}
        />
      </ToggleButton>
      <ToggleButton value="2" disabled={isApiMqttExcludeDisabled}>
        <OptionIcon
          type="api_mqtt_exclude"
          isSet={hasMask(de.m, DeviceEntityMask.DV_API_MQTT_EXCLUDE)}
        />
      </ToggleButton>
      <ToggleButton value="1" disabled={isWebExcludeDisabled}>
        <OptionIcon
          type="web_exclude"
          isSet={hasMask(de.m, DeviceEntityMask.DV_WEB_EXCLUDE)}
        />
      </ToggleButton>
      <ToggleButton value="128">
        <OptionIcon
          type="deleted"
          isSet={hasMask(de.m, DeviceEntityMask.DV_DELETED)}
        />
      </ToggleButton>
    </ToggleButtonGroup>
  );
};

export default EntityMaskToggle;
