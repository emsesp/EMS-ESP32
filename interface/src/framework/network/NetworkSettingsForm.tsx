import { FC, useContext, useEffect, useState } from 'react';

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
  InputAdornment
} from '@mui/material';

import LockOpenIcon from '@mui/icons-material/LockOpen';
import DeleteIcon from '@mui/icons-material/Delete';
import SaveIcon from '@mui/icons-material/Save';
import LockIcon from '@mui/icons-material/Lock';

import {
  BlockFormControlLabel,
  ButtonRow,
  FormLoader,
  SectionContent,
  ValidatedPasswordField,
  ValidatedTextField
} from '../../components';
import { NetworkSettings } from '../../types';
import * as NetworkApi from '../../api/network';
import { numberValue, updateValue, useRest } from '../../utils';

import { WiFiConnectionContext } from './WiFiConnectionContext';
import { isNetworkOpen, networkSecurityMode } from './WiFiNetworkSelector';
import { ValidateFieldsError } from 'async-validator';
import { validate } from '../../validators';
import { createNetworkSettingsValidator } from '../../validators/network';

import { useI18nContext } from '../../i18n/i18n-react';

const WiFiSettingsForm: FC = () => {
  const { LL } = useI18nContext();

  const { selectedNetwork, deselectNetwork } = useContext(WiFiConnectionContext);

  const [initialized, setInitialized] = useState(false);
  const { loadData, saving, data, setData, saveData, errorMessage } = useRest<NetworkSettings>({
    read: NetworkApi.readNetworkSettings,
    update: NetworkApi.updateNetworkSettings
  });

  useEffect(() => {
    if (!initialized && data) {
      if (selectedNetwork) {
        setData({
          ssid: selectedNetwork.ssid,
          password: '',
          hostname: data?.hostname,
          static_ip_config: false,
          enableIPv6: false,
          bandwidth20: false,
          tx_power: 20,
          nosleep: false,
          enableMDNS: true
        });
      }
      setInitialized(true);
    }
  }, [initialized, setInitialized, data, setData, selectedNetwork]);

  const updateFormValue = updateValue(setData);

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
        saveData();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
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

        <BlockFormControlLabel
          control={<Checkbox name="enableMDNS" checked={data.enableMDNS} onChange={updateFormValue} />}
          label={LL.NETWORK_USE_DNS()}
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
              label="Local IP"
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="gateway_ip"
              label="Gateway"
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="subnet_mask"
              label="Subnet"
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="dns_ip_1"
              label="DNS IP #1"
              fullWidth
              variant="outlined"
              value={data.dns_ip_1}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="dns_ip_2"
              label="DNS IP #2"
              fullWidth
              variant="outlined"
              value={data.dns_ip_2}
              onChange={updateFormValue}
              margin="normal"
            />
          </>
        )}
        <ButtonRow>
          <Button
            startIcon={<SaveIcon />}
            disabled={saving}
            variant="outlined"
            color="primary"
            type="submit"
            onClick={validateAndSubmit}
          >
            {LL.SAVE()}
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title={LL.SETTINGS_OF( LL.NETWORK(1) )} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default WiFiSettingsForm;
