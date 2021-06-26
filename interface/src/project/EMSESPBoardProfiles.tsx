import MenuItem from '@material-ui/core/MenuItem';

type BoardProfiles = {
  [name: string]: string;
};

export const BOARD_PROFILES: BoardProfiles = {
  S32: 'BBQKees Gateway S32',
  E32: 'BBQKees Gateway E32',
  NODEMCU: 'NodeMCU 32S',
  'MH-ET': 'MH-ET Live D1 Mini',
  LOLIN: 'Lolin D32',
  OLIMEX: 'Olimex ESP32-EVB',
  TLK110: 'Generic Ethernet (TLK110)',
  LAN8720: 'Generic Ethernet (LAN8720)'
};

export function boardProfileSelectItems() {
  return Object.keys(BOARD_PROFILES).map((code) => (
    <MenuItem key={code} value={code}>
      {BOARD_PROFILES[code]}
    </MenuItem>
  ));
}
