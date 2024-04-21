import { ToggleButton, ToggleButtonGroup } from '@mui/material';

import OptionIcon from './OptionIcon';
import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

interface EntityMaskToggleProps {
  onUpdate: (de: DeviceEntity) => void;
  de: DeviceEntity;
}

const EntityMaskToggle = ({ onUpdate, de }: EntityMaskToggleProps) => {
  const getMaskNumber = (newMask: string[]) => {
    let new_mask = 0;
    for (const entry of newMask) {
      new_mask |= Number(entry);
    }
    return new_mask;
  };

  const getMaskString = (m: number) => {
    const new_masks: string[] = [];
    if ((m & 1) === 1) {
      new_masks.push('1');
    }
    if ((m & 2) === 2) {
      new_masks.push('2');
    }
    if ((m & 4) === 4) {
      new_masks.push('4');
    }
    if ((m & 8) === 8) {
      new_masks.push('8');
    }
    if ((m & 128) === 128) {
      new_masks.push('128');
    }
    return new_masks;
  };

  return (
    <ToggleButtonGroup
      size="small"
      color="secondary"
      value={getMaskString(de.m)}
      onChange={(event, mask: string[]) => {
        de.m = getMaskNumber(mask);
        if (de.n === '' && de.m & DeviceEntityMask.DV_READONLY) {
          de.m = de.m | DeviceEntityMask.DV_WEB_EXCLUDE;
        }
        if (de.m & DeviceEntityMask.DV_WEB_EXCLUDE) {
          de.m = de.m & ~DeviceEntityMask.DV_FAVORITE;
        }
        onUpdate(de);
      }}
    >
      <ToggleButton value="8" disabled={(de.m & 0x81) !== 0 || de.n === undefined}>
        <OptionIcon
          type="favorite"
          isSet={
            (de.m & DeviceEntityMask.DV_FAVORITE) === DeviceEntityMask.DV_FAVORITE
          }
        />
      </ToggleButton>
      <ToggleButton value="4" disabled={!de.w || (de.m & 0x83) >= 3}>
        <OptionIcon
          type="readonly"
          isSet={
            (de.m & DeviceEntityMask.DV_READONLY) === DeviceEntityMask.DV_READONLY
          }
        />
      </ToggleButton>
      <ToggleButton value="2" disabled={de.n === '' || (de.m & 0x80) !== 0}>
        <OptionIcon
          type="api_mqtt_exclude"
          isSet={
            (de.m & DeviceEntityMask.DV_API_MQTT_EXCLUDE) ===
            DeviceEntityMask.DV_API_MQTT_EXCLUDE
          }
        />
      </ToggleButton>
      <ToggleButton value="1" disabled={de.n === undefined || (de.m & 0x80) !== 0}>
        <OptionIcon
          type="web_exclude"
          isSet={
            (de.m & DeviceEntityMask.DV_WEB_EXCLUDE) ===
            DeviceEntityMask.DV_WEB_EXCLUDE
          }
        />
      </ToggleButton>
      <ToggleButton value="128">
        <OptionIcon
          type="deleted"
          isSet={
            (de.m & DeviceEntityMask.DV_DELETED) === DeviceEntityMask.DV_DELETED
          }
        />
      </ToggleButton>
    </ToggleButtonGroup>
  );
};

export default EntityMaskToggle;
