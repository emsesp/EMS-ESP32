import { ToggleButton, ToggleButtonGroup } from '@mui/material';

import OptionIcon from './OptionIcon';
import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

interface EntityMaskToggleProps {
  onUpdate: (de: DeviceEntity) => void;
  de: DeviceEntity;
}

const MASK_VALUES = [
  DeviceEntityMask.DV_WEB_EXCLUDE, // 1
  DeviceEntityMask.DV_API_MQTT_EXCLUDE, // 2
  DeviceEntityMask.DV_READONLY, // 4
  DeviceEntityMask.DV_FAVORITE, // 8
  DeviceEntityMask.DV_DELETED // 128
];

const getMaskNumber = (newMask: string[]): number =>
  newMask.reduce((mask, entry) => mask | Number(entry), 0);

const getMaskString = (mask: number): string[] =>
  MASK_VALUES.filter((value) => (mask & value) === value).map((value) =>
    String(value)
  );

const hasMask = (mask: number, flag: number): boolean => (mask & flag) === flag;

const EntityMaskToggle = ({ onUpdate, de }: EntityMaskToggleProps) => {
  const handleChange = (_event: unknown, mask: string[]) => {
    const newMask = getMaskNumber(mask);
    const updatedDe = { ...de };

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
  };

  return (
    <ToggleButtonGroup
      size="small"
      color="secondary"
      value={getMaskString(de.m)}
      onChange={handleChange}
    >
      <ToggleButton
        value="8"
        disabled={
          hasMask(
            de.m,
            DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_DELETED
          ) || de.n === undefined
        }
      >
        <OptionIcon
          type="favorite"
          isSet={hasMask(de.m, DeviceEntityMask.DV_FAVORITE)}
        />
      </ToggleButton>
      <ToggleButton
        value="4"
        disabled={
          !de.w ||
          hasMask(
            de.m,
            DeviceEntityMask.DV_WEB_EXCLUDE | DeviceEntityMask.DV_FAVORITE
          )
        }
      >
        <OptionIcon
          type="readonly"
          isSet={hasMask(de.m, DeviceEntityMask.DV_READONLY)}
        />
      </ToggleButton>
      <ToggleButton
        value="2"
        disabled={de.n === '' || hasMask(de.m, DeviceEntityMask.DV_DELETED)}
      >
        <OptionIcon
          type="api_mqtt_exclude"
          isSet={hasMask(de.m, DeviceEntityMask.DV_API_MQTT_EXCLUDE)}
        />
      </ToggleButton>
      <ToggleButton
        value="1"
        disabled={de.n === undefined || hasMask(de.m, DeviceEntityMask.DV_DELETED)}
      >
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
