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

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

const HardwareStatus = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.HARDWARE());

  const { data, send: loadData, error } = useRequest(SystemApi.readSystemStatus);

  useInterval(() => {
    void loadData();
  });

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            {data.model ? (
              <Avatar sx={{ bgcolor: '#003289', color: 'white' }}>
                <img
                  alt="BBQKees"
                  src={BBQKeesIcon}
                  style={{ width: 16, verticalAlign: 'middle' }}
                />
              </Avatar>
            ) : (
              <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
                <TapAndPlayIcon />
              </Avatar>
            )}
          </ListItemAvatar>
          <ListItemText
            primary={LL.HARDWARE() + ' ' + LL.DEVICE()}
            secondary={data.model ? data.model : data.cpu_type}
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <DevicesIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary="SDK"
            secondary={data.arduino_version + ' / ESP-IDF ' + data.sdk_version}
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <DeveloperBoardIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary="CPU"
            secondary={
              data.esp_platform +
              '/' +
              data.cpu_type +
              ' (rev.' +
              data.cpu_rev +
              ', ' +
              (data.cpu_cores === 1 ? 'single-core)' : 'dual-core)') +
              ' @ ' +
              data.cpu_freq_mhz +
              ' Mhz' +
              // bit of a hack : if the CPU temp is higher than 90 (=32 Fahrenheit if using Celsius), show F, otherwise C
              (data.temperature
                ? ', T: ' +
                  data.temperature +
                  ' °' +
                  (data.temperature > 90 ? 'F' : 'C')
                : '')
            }
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <MemoryIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.FREE_MEMORY()}
            secondary={
              formatNumber(data.free_heap) +
              ' KB (' +
              formatNumber(data.max_alloc_heap) +
              ' KB max alloc, ' +
              formatNumber(data.free_caps) +
              ' KB caps)'
            }
          />
        </ListItem>
        {data.psram_size !== undefined && data.free_psram !== undefined && (
          <>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
                  <AppsIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={LL.PSRAM()}
                secondary={
                  formatNumber(data.psram_size) +
                  ' KB / ' +
                  formatNumber(data.free_psram) +
                  ' KB'
                }
              />
            </ListItem>
          </>
        )}
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <SdStorageIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.FLASH()}
            secondary={
              formatNumber(data.flash_chip_size) +
              ' KB , ' +
              (data.flash_chip_speed / 1000000).toFixed(0) +
              ' MHz'
            }
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <SdCardAlertIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.APPSIZE()}
            secondary={
              data.partition +
              ': ' +
              formatNumber(data.app_used) +
              ' KB / ' +
              formatNumber(data.app_free) +
              ' KB'
            }
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
              <FolderIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.FILESYSTEM()}
            secondary={
              formatNumber(data.fs_used) +
              ' KB / ' +
              formatNumber(data.fs_free) +
              ' KB'
            }
          />
        </ListItem>
        <Divider variant="inset" component="li" />
      </List>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default HardwareStatus;
