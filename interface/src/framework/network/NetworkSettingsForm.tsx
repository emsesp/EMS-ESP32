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
  ListItemSecondaryAction,
  ListItemText,
  Typography,
  InputAdornment,
  TextField
} from '@mui/material';
import { updateState, useRequest } from 'alova';
import { useContext, useEffect, useState } from 'react';
import { toast } from 'react-toastify';
import RestartMonitor from '../system/RestartMonitor';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import { isNetworkOpen, networkSecurityMode } from './WiFiNetworkSelector';
import type { ValidateFieldsError } from 'async-validator';
import type { FC } from 'react';

import type { NetworkSettings } from 'types';
import * as NetworkApi from 'api/network';
import {
  BlockFormControlLabel,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField,
  MessageBox,
  BlockNavigation
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';
import { numberValue, updateValueDirty, useRest2 } from 'utils';

import { validate } from 'validators';
import { createNetworkSettingsValidator } from 'validators/network';

const WiFiSettingsForm: FC = () => {
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
  } = useRest2<NetworkSettings>({
    read: NetworkApi.readNetworkSettings,
    update: NetworkApi.updateNetworkSettings
  });

  const { send: restartCommand } = useRequest(EMSESP.restart(), {
    immediate: false
  });

  useEffect(() => {
    if (!initialized && data) {
      if (selectedNetwork) {
        updateState('networkSettings', (current_data) => ({
          ssid: selectedNetwork.ssid,
          password: '',
          hostname: current_data?.hostname,
          static_ip_config: false,
          enableIPv6: false,
          bandwidth20: false,
          tx_power: 20,
          nosleep: false,
          enableMDNS: true,
          enableCORS: false,
          CORSOrigin: '*'
        }));
      }
      setInitialized(true);
    }
  }, [initialized, setInitialized, data, selectedNetwork]);

  const updateFormValue = updateValueDirty(origData, dirtyFlags, setDirtyFlags, updateDataValue);

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
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    };

    const restart = async () => {
      await restartCommand().catch((error) => {
        toast.error(error.message);
      });
      setRestarting(true);
    };

    return (
      <>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          WiFi
        </Typography>
        {selectedNetwork ? (
          <List>
            <ListItem>
              <ListItemAvatar>
                <Avatar>{isNetworkOpen(selectedNetwork) ? <LockOpenIcon /> : <LockIcon />}</Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={selectedNetwork.ssid}
                secondary={'Security: ' + networkSecurityMode(selectedNetwork) + ', Ch: ' + selectedNetwork.channel}
              />
              <ListItemSecondaryAction>
                <IconButton aria-label="Manual Config" onClick={deselectNetwork}>
                  <DeleteIcon />
                </IconButton>
              </ListItemSecondaryAction>
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
        <ValidatedTextField
          fieldErrors={fieldErrors}
          name="tx_power"
          label={LL.TX_POWER()}
          InputProps={{
            endAdornment: <InputAdornment position="end">dBm</InputAdornment>
          }}
          fullWidth
          variant="outlined"
          value={numberValue(data.tx_power)}
          onChange={updateFormValue}
          type="number"
          margin="normal"
        />
        <BlockFormControlLabel
          control={<Checkbox name="nosleep" checked={data.nosleep} onChange={updateFormValue} />}
          label={LL.NETWORK_DISABLE_SLEEP()}
        />
        <BlockFormControlLabel
          control={<Checkbox name="bandwidth20" checked={data.bandwidth20} onChange={updateFormValue} />}
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
          control={<Checkbox name="enableMDNS" checked={data.enableMDNS} onChange={updateFormValue} />}
          label={LL.NETWORK_USE_DNS()}
        />
        <BlockFormControlLabel
          control={<Checkbox name="enableCORS" checked={data.enableCORS} onChange={updateFormValue} />}
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
          control={<Checkbox name="enableIPv6" checked={data.enableIPv6} onChange={updateFormValue} />}
          label={LL.NETWORK_ENABLE_IPV6()}
        />
        <BlockFormControlLabel
          control={<Checkbox name="static_ip_config" checked={data.static_ip_config} onChange={updateFormValue} />}
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
          <MessageBox my={2} level="warning" message={LL.RESTART_TEXT()}>
            <Button startIcon={<PowerSettingsNewIcon />} variant="contained" color="error" onClick={restart}>
              {LL.RESTART()}
            </Button>
          </MessageBox>
        )}

        {!restartNeeded && dirtyFlags && dirtyFlags.length !== 0 && (
          <ButtonRow>
            <Button
              startIcon={<CancelIcon />}
              disabled={saving}
              variant="outlined"
              color="primary"
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
    <SectionContent title={LL.SETTINGS_OF(LL.NETWORK(1))} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {restarting ? <RestartMonitor /> : content()}
    </SectionContent>
  );
};

export default WiFiSettingsForm;
