import { useContext, useEffect, useState } from 'react';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import DeleteIcon from '@mui/icons-material/Delete';
import LockIcon from '@mui/icons-material/Lock';
import LockOpenIcon from '@mui/icons-material/LockOpen';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import WarningIcon from '@mui/icons-material/Warning';
import {
  Avatar,
  Button,
  Checkbox,
  IconButton,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  MenuItem,
  TextField,
  Typography
} from '@mui/material';

import * as NetworkApi from 'api/network';
import { API } from 'api/app';

import { updateState, useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import type { ValidateFieldsError } from 'async-validator';
import {
  BlockFormControlLabel,
  BlockNavigation,
  ButtonRow,
  FormLoader,
  MessageBox,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { NetworkSettingsType } from 'types';
import { updateValueDirty, useRest } from 'utils';
import { validate } from 'validators';
import { createNetworkSettingsValidator } from 'validators/network';

import SystemMonitor from '../../status/SystemMonitor';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import { isNetworkOpen, networkSecurityMode } from './WiFiNetworkSelector';

const NetworkSettings = () => {
  const { LL } = useI18nContext();

  const { selectedNetwork, deselectNetwork } = useContext(WiFiConnectionContext);

  const [initialized, setInitialized] = useState(false);
  const [restarting, setRestarting] = useState(false);

  const {
    loadData,
    saving,
    data,
    updateDataValue,
    origData,
    dirtyFlags,
    setDirtyFlags,
    blocker,
    saveData,
    errorMessage,
    restartNeeded
  } = useRest<NetworkSettingsType>({
    read: NetworkApi.readNetworkSettings,
    update: NetworkApi.updateNetworkSettings
  });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  useEffect(() => {
    if (!initialized && data) {
      if (selectedNetwork) {
        void updateState(
          NetworkApi.readNetworkSettings(),
          (current_data: NetworkSettingsType) => ({
            ssid: selectedNetwork.ssid,
            bssid: selectedNetwork.bssid,
            password: current_data ? current_data.password : '',
            hostname: current_data?.hostname,
            static_ip_config: false,
            bandwidth20: false,
            tx_power: 0,
            nosleep: false,
            enableMDNS: true,
            enableCORS: false,
            CORSOrigin: '*'
          })
        );
      }
      setInitialized(true);
    }
  }, [initialized, setInitialized, data, selectedNetwork]);

  const updateFormValue = updateValueDirty(
    origData,
    dirtyFlags,
    setDirtyFlags,
    updateDataValue
  );

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  useEffect(() => deselectNetwork, [deselectNetwork]);

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const validateAndSubmit = async () => {
      try {
        setFieldErrors(undefined);
        await validate(createNetworkSettingsValidator(data), data);
        await saveData();
      } catch (error) {
        setFieldErrors(error as ValidateFieldsError);
      }
      deselectNetwork();
    };

    const setCancel = async () => {
      deselectNetwork();
      await loadData();
    };

    const doRestart = async () => {
      setRestarting(true);
      await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
        (error: Error) => {
          toast.error(error.message);
        }
      );
    };

    return (
      <>
        <Typography variant="h6" color="primary">
          WiFi
        </Typography>
        {selectedNetwork ? (
          <List>
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  {isNetworkOpen(selectedNetwork) ? <LockOpenIcon /> : <LockIcon />}
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={selectedNetwork.ssid}
                secondary={
                  'Security: ' +
                  networkSecurityMode(selectedNetwork) +
                  ', Ch: ' +
                  selectedNetwork.channel +
                  ', bssid: ' +
                  selectedNetwork.bssid
                }
              />
              <IconButton onClick={setCancel}>
                <DeleteIcon />
              </IconButton>
            </ListItem>
          </List>
        ) : (
          <ValidatedTextField
            fieldErrors={fieldErrors}
            name="ssid"
            label={'SSID (' + LL.NETWORK_BLANK_SSID() + ')'}
            fullWidth
            variant="outlined"
            value={data.ssid}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="bssid"
          label={'BSSID (' + LL.NETWORK_BLANK_BSSID() + ')'}
          fullWidth
          variant="outlined"
          value={data.bssid}
          onChange={updateFormValue}
          margin="normal"
        />
        {(!selectedNetwork || !isNetworkOpen(selectedNetwork)) && (
          <ValidatedPasswordField
            fieldErrors={fieldErrors}
            name="password"
            label={LL.PASSWORD()}
            fullWidth
            variant="outlined"
            value={data.password}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        <TextField
          name="tx_power"
          label={LL.TX_POWER()}
          fullWidth
          variant="outlined"
          value={data.tx_power}
          onChange={updateFormValue}
          margin="normal"
          select
        >
          <MenuItem value={0}>Auto</MenuItem>
          <MenuItem value={78}>19.5 dBm</MenuItem>
          <MenuItem value={76}>19 dBm</MenuItem>
          <MenuItem value={74}>18.5 dBm</MenuItem>
          <MenuItem value={68}>17 dBm</MenuItem>
          <MenuItem value={60}>15 dBm</MenuItem>
          <MenuItem value={52}>13 dBm</MenuItem>
          <MenuItem value={44}>11 dBm</MenuItem>
          <MenuItem value={34}>8.5 dBm</MenuItem>
          <MenuItem value={28}>7 dBm</MenuItem>
          <MenuItem value={20}>5 dBm</MenuItem>
          <MenuItem value={8}>2 dBm</MenuItem>
        </TextField>
        <BlockFormControlLabel
          control={
            <Checkbox
              name="nosleep"
              checked={data.nosleep}
              onChange={updateFormValue}
            />
          }
          label={LL.NETWORK_DISABLE_SLEEP()}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              name="bandwidth20"
              checked={data.bandwidth20}
              onChange={updateFormValue}
            />
          }
          label={LL.NETWORK_LOW_BAND()}
        />
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {LL.GENERAL_OPTIONS()}
        </Typography>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="hostname"
          label={LL.HOSTNAME()}
          fullWidth
          variant="outlined"
          value={data.hostname}
          onChange={updateFormValue}
          margin="normal"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              name="enableMDNS"
              checked={data.enableMDNS}
              onChange={updateFormValue}
            />
          }
          label={LL.NETWORK_USE_DNS()}
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              name="enableCORS"
              checked={data.enableCORS}
              onChange={updateFormValue}
            />
          }
          label={LL.NETWORK_ENABLE_CORS()}
        />
        {data.enableCORS && (
          <TextField
            name="CORSOrigin"
            label={LL.NETWORK_CORS_ORIGIN()}
            fullWidth
            variant="outlined"
            value={data.CORSOrigin}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        <BlockFormControlLabel
          control={
            <Checkbox
              name="static_ip_config"
              checked={data.static_ip_config}
              onChange={updateFormValue}
            />
          }
          label={LL.NETWORK_FIXED_IP()}
        />
        {data.static_ip_config && (
          <>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="local_ip"
              label={LL.AP_LOCAL_IP()}
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="gateway_ip"
              label={LL.NETWORK_GATEWAY()}
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="subnet_mask"
              label={LL.NETWORK_SUBNET()}
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="dns_ip_1"
              label="DNS #1"
              fullWidth
              variant="outlined"
              value={data.dns_ip_1}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="dns_ip_2"
              label="DNS #2"
              fullWidth
              variant="outlined"
              value={data.dns_ip_2}
              onChange={updateFormValue}
              margin="normal"
            />
          </>
        )}
        {restartNeeded && (
          <MessageBox my={2} level="warning" message={LL.RESTART_TEXT(0)}>
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="contained"
              color="error"
              onClick={doRestart}
            >
              {LL.RESTART()}
            </Button>
          </MessageBox>
        )}

        {!restartNeeded &&
          (selectedNetwork || (dirtyFlags && dirtyFlags.length !== 0)) && (
            <ButtonRow>
              <Button
                startIcon={<CancelIcon />}
                disabled={saving}
                variant="outlined"
                color="secondary"
                type="submit"
                onClick={loadData}
              >
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                disabled={saving}
                variant="contained"
                color="info"
                type="submit"
                onClick={validateAndSubmit}
              >
                {LL.APPLY_CHANGES(dirtyFlags.length)}
              </Button>
            </ButtonRow>
          )}
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {restarting ? <SystemMonitor /> : content()}
    </SectionContent>
  );
};

export default NetworkSettings;
