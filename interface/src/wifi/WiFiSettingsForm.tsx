import React, { Fragment } from 'react';
import { TextValidator, ValidatorForm } from 'react-material-ui-form-validator';

import { Checkbox, List, ListItem, ListItemText, ListItemAvatar, ListItemSecondaryAction } from '@material-ui/core';

import Avatar from '@material-ui/core/Avatar';
import IconButton from '@material-ui/core/IconButton';
import LockIcon from '@material-ui/icons/Lock';
import LockOpenIcon from '@material-ui/icons/LockOpen';
import DeleteIcon from '@material-ui/icons/Delete';
import SaveIcon from '@material-ui/icons/Save';

import { RestFormProps, PasswordValidator, BlockFormControlLabel, FormActions, FormButton } from '../components';
import { isIP, isHostname, optional } from '../validators';

import { WiFiConnectionContext } from './WiFiConnectionContext';
import { isNetworkOpen, networkSecurityMode } from './WiFiSecurityModes';
import { WiFiSettings } from './types';

type WiFiStatusFormProps = RestFormProps<WiFiSettings>;

class WiFiSettingsForm extends React.Component<WiFiStatusFormProps> {

  static contextType = WiFiConnectionContext;
  context!: React.ContextType<typeof WiFiConnectionContext>;

  constructor(props: WiFiStatusFormProps, context: WiFiConnectionContext) {
    super(props);

    const { selectedNetwork } = context;
    if (selectedNetwork) {
      const wifiSettings: WiFiSettings = {
        ssid: selectedNetwork.ssid,
        password: "",
        hostname: props.data.hostname,
        static_ip_config: false,
      }
      props.setData(wifiSettings);
    }
  }

  componentWillMount() {
    ValidatorForm.addValidationRule('isIP', isIP);
    ValidatorForm.addValidationRule('isHostname', isHostname);
    ValidatorForm.addValidationRule('isOptionalIP', optional(isIP));
  }

  deselectNetworkAndLoadData = () => {
    this.context.deselectNetwork();
    this.props.loadData();
  }

  componentWillUnmount() {
    this.context.deselectNetwork();
  }

  render() {
    const { selectedNetwork, deselectNetwork } = this.context;
    const { data, handleValueChange, saveData } = this.props;
    return (
      <ValidatorForm onSubmit={saveData} ref="WiFiSettingsForm">
        {
          selectedNetwork ?
            <List>
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    {isNetworkOpen(selectedNetwork) ? <LockOpenIcon /> : <LockIcon />}
                  </Avatar>
                </ListItemAvatar>
                <ListItemText
                  primary={selectedNetwork.ssid}
                  secondary={"Security: " + networkSecurityMode(selectedNetwork) + ", Ch: " + selectedNetwork.channel}
                />
                <ListItemSecondaryAction>
                  <IconButton aria-label="Manual Config" onClick={deselectNetwork}>
                    <DeleteIcon />
                  </IconButton>
                </ListItemSecondaryAction>
              </ListItem>
            </List>
            :
            <TextValidator
              validators={['matchRegexp:^.{0,32}$']}
              errorMessages={['SSID must be 32 characters or less']}
              name="ssid"
              label="SSID"
              fullWidth
              variant="outlined"
              value={data.ssid}
              onChange={handleValueChange('ssid')}
              margin="normal"
            />
        }
        {
          (!selectedNetwork || !isNetworkOpen(selectedNetwork)) &&
          <PasswordValidator
            validators={['matchRegexp:^.{0,64}$']}
            errorMessages={['Password must be 64 characters or less']}
            name="password"
            label="Password"
            fullWidth
            variant="outlined"
            value={data.password}
            onChange={handleValueChange('password')}
            margin="normal"
          />
        }
        <TextValidator
          validators={['required', 'isHostname']}
          errorMessages={['Hostname is required', "Not a valid hostname"]}
          name="hostname"
          label="Hostname"
          fullWidth
          variant="outlined"
          value={data.hostname}
          onChange={handleValueChange('hostname')}
          margin="normal"
        />
        <BlockFormControlLabel
          control={
            <Checkbox
              value="static_ip_config"
              checked={data.static_ip_config}
              onChange={handleValueChange("static_ip_config")}
            />
          }
          label="Static IP Config"
        />
        {
          data.static_ip_config &&
          <Fragment>
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Local IP is required', 'Must be an IP address']}
              name="local_ip"
              label="Local IP"
              fullWidth
              variant="outlined"
              value={data.local_ip}
              onChange={handleValueChange('local_ip')}
              margin="normal"
            />
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Gateway IP is required', 'Must be an IP address']}
              name="gateway_ip"
              label="Gateway"
              fullWidth
              variant="outlined"
              value={data.gateway_ip}
              onChange={handleValueChange('gateway_ip')}
              margin="normal"
            />
            <TextValidator
              validators={['required', 'isIP']}
              errorMessages={['Subnet mask is required', 'Must be an IP address']}
              name="subnet_mask"
              label="Subnet"
              fullWidth
              variant="outlined"
              value={data.subnet_mask}
              onChange={handleValueChange('subnet_mask')}
              margin="normal"
            />
            <TextValidator
              validators={['isOptionalIP']}
              errorMessages={['Must be an IP address']}
              name="dns_ip_1"
              label="DNS IP #1"
              fullWidth
              variant="outlined"
              value={data.dns_ip_1}
              onChange={handleValueChange('dns_ip_1')}
              margin="normal"
            />
            <TextValidator
              validators={['isOptionalIP']}
              errorMessages={['Must be an IP address']}
              name="dns_ip_2"
              label="DNS IP #2"
              fullWidth
              variant="outlined"
              value={data.dns_ip_2}
              onChange={handleValueChange('dns_ip_2')}
              margin="normal"
            />
          </Fragment>
        }
        <FormActions>
          <FormButton startIcon={<SaveIcon />} variant="contained" color="primary" type="submit">
            Save
          </FormButton>
        </FormActions>
      </ValidatorForm>
    );
  }
}

export default WiFiSettingsForm;