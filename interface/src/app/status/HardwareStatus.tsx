import { ReactElement } from 'react';

import AppsIcon from '@mui/icons-material/Apps';
import DeveloperBoardIcon from '@mui/icons-material/DeveloperBoard';
import DevicesIcon from '@mui/icons-material/Devices';
import FolderIcon from '@mui/icons-material/Folder';
import MemoryIcon from '@mui/icons-material/Memory';
import SdCardAlertIcon from '@mui/icons-material/SdCardAlert';
import SdStorageIcon from '@mui/icons-material/SdStorage';
import TapAndPlayIcon from '@mui/icons-material/TapAndPlay';
import {
  Avatar,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText
} from '@mui/material';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import BBQKeesIcon from './bbqkees.svg';

// Constants
const AVATAR_COLORS = {
  DEFAULT: '#5f9a5f',
  BBQKEES: '#003289'
} as const;

const TEMP_THRESHOLD_CELSIUS = 90; // Temperature threshold to determine F vs C

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

function formatTemperature(temp?: number): string {
  if (!temp) return '';
  const unit = temp > TEMP_THRESHOLD_CELSIUS ? 'F' : 'C';
  return `, T: ${temp} °${unit}`;
}

function formatFlashSpeed(speed: number): string {
  return (speed / 1000000).toFixed(0) + ' MHz';
}

function formatCPUCores(cores: number): string {
  return cores === 1 ? 'single-core)' : 'dual-core)';
}

// Reusable component for hardware status list items
interface HardwareListItemProps {
  icon: ReactElement;
  primary: string;
  secondary: string;
  avatarColor?: string;
  customIcon?: ReactElement | undefined;
}

const HardwareListItem = ({
  icon,
  primary,
  secondary,
  avatarColor = AVATAR_COLORS.DEFAULT,
  customIcon
}: HardwareListItemProps) => (
  <>
    <ListItem>
      <ListItemAvatar>
        <Avatar sx={{ bgcolor: avatarColor, color: 'white' }}>
          {customIcon || icon}
        </Avatar>
      </ListItemAvatar>
      <ListItemText primary={primary} secondary={secondary} />
    </ListItem>
    <Divider variant="inset" component="li" />
  </>
);

const HardwareStatus = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.HARDWARE());

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  return (
    <SectionContent>
      <List>
        <HardwareListItem
          icon={<TapAndPlayIcon />}
          primary={`${LL.HARDWARE()} ${LL.DEVICE()}`}
          secondary={data.model || data.cpu_type}
          avatarColor={data.model ? AVATAR_COLORS.BBQKEES : AVATAR_COLORS.DEFAULT}
          customIcon={
            data.model ? (
              <img
                alt="BBQKees"
                src={BBQKeesIcon}
                style={{ width: 16, verticalAlign: 'middle' }}
              />
            ) : undefined
          }
        />
        <HardwareListItem
          icon={<DevicesIcon />}
          primary="SDK"
          secondary={`${data.arduino_version} / ESP-IDF ${data.sdk_version}`}
        />
        <HardwareListItem
          icon={<DeveloperBoardIcon />}
          primary="CPU"
          secondary={`${data.esp_platform}/${data.cpu_type} (rev.${data.cpu_rev}, ${formatCPUCores(data.cpu_cores)} @ ${data.cpu_freq_mhz} Mhz${formatTemperature(data.temperature)}`}
        />
        <HardwareListItem
          icon={<MemoryIcon />}
          primary={LL.FREE_MEMORY()}
          secondary={`${formatNumber(data.free_heap)} KB (${formatNumber(data.max_alloc_heap)} KB max alloc, ${formatNumber(data.free_caps)} KB caps)`}
        />
        {data.psram_size !== undefined && data.free_psram !== undefined && (
          <HardwareListItem
            icon={<AppsIcon />}
            primary={LL.PSRAM()}
            secondary={`${formatNumber(data.psram_size)} KB / ${formatNumber(data.free_psram)} KB`}
          />
        )}
        <HardwareListItem
          icon={<SdStorageIcon />}
          primary={LL.FLASH()}
          secondary={`${formatNumber(data.flash_chip_size)} KB , ${formatFlashSpeed(data.flash_chip_speed)}`}
        />
        <HardwareListItem
          icon={<SdCardAlertIcon />}
          primary={LL.APPSIZE()}
          secondary={`${data.partition}: ${formatNumber(data.app_used)} KB / ${formatNumber(data.app_free)} KB`}
        />
        <HardwareListItem
          icon={<FolderIcon />}
          primary={LL.FILESYSTEM()}
          secondary={`${formatNumber(data.fs_used)} KB / ${formatNumber(data.fs_free)} KB`}
        />
      </List>
    </SectionContent>
  );
};

export default HardwareStatus;
