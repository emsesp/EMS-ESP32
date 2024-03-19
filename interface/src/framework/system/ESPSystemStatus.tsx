import AppsIcon from '@mui/icons-material/Apps';
import DeveloperBoardIcon from '@mui/icons-material/DeveloperBoard';
import DevicesIcon from '@mui/icons-material/Devices';
import FolderIcon from '@mui/icons-material/Folder';
import MemoryIcon from '@mui/icons-material/Memory';
import RefreshIcon from '@mui/icons-material/Refresh';
import SdCardAlertIcon from '@mui/icons-material/SdCardAlert';
import SdStorageIcon from '@mui/icons-material/SdStorage';
import { Avatar, Box, Button, Divider, List, ListItem, ListItemAvatar, ListItemText } from '@mui/material';

import { useRequest } from 'alova';
import type { FC } from 'react';

import * as SystemApi from 'api/system';
import { ButtonRow, FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

const ESPSystemStatus: FC = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.STATUS_OF('ESP32'));

  const { data: data, send: loadData, error } = useRequest(SystemApi.readESPSystemStatus, { force: true });

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <List>
          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
                <DevicesIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="SDK" secondary={data.arduino_version + ' / ESP-IDF ' + data.sdk_version} />
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
                (data.cpu_cores == 1 ? 'single-core)' : 'dual-core)') +
                ' @ ' +
                data.cpu_freq_mhz +
                ' Mhz'
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
              primary={LL.HEAP()}
              secondary={formatNumber(data.free_heap) + ' KB / ' + formatNumber(data.max_alloc_heap) + ' KB '}
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
                  secondary={formatNumber(data.psram_size) + ' KB / ' + formatNumber(data.free_psram) + ' KB'}
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
                formatNumber(data.flash_chip_size) + ' KB / ' + (data.flash_chip_speed / 1000000).toFixed(0) + ' MHz'
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
                data.partition + ': ' + formatNumber(data.app_used) + ' KB / ' + formatNumber(data.app_free) + ' KB'
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
              secondary={formatNumber(data.fs_used) + ' KB / ' + formatNumber(data.fs_free) + ' KB'}
            />
          </ListItem>
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
            <ButtonRow>
              <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
                {LL.REFRESH()}
              </Button>
            </ButtonRow>
          </Box>
        </Box>
      </>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default ESPSystemStatus;
