import React from 'react';
import MenuItem from '@material-ui/core/MenuItem';

type BoardProfiles = {
    [name: string]: string
};

export const BOARD_PROFILES: BoardProfiles = {
    "S32": "Gateway S32",
    "NODEMCU": "NodeMCU 32S",
    "MT-ET": "MT-ET Live D1 Mini",
    "LOLIN": "Lolin D32",
    "WEMOS": "Wemos Mini D1-32",
    "E32": "Gateway E32",
    "OLIMEX": "Olimex ESP32-EVB-EA",
    "TLK110": "Ethernet (TLK110)"
}

export function boardProfileSelectItems() {
    return Object.keys(BOARD_PROFILES).map(code => (
        <MenuItem key={code} value={code}>{BOARD_PROFILES[code]}</MenuItem>
    ));
}
