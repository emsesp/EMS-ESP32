import React, { Component, Fragment } from "react";

import { WithTheme, withTheme } from "@material-ui/core/styles";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Avatar,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
} from "@material-ui/core";

import BuildIcon from "@material-ui/icons/Build";
import RefreshIcon from "@material-ui/icons/Refresh";
import DeviceHubIcon from "@material-ui/icons/DeviceHub";
import SpeakerNotesOffIcon from "@material-ui/icons/SpeakerNotesOff";
import TimerIcon from "@material-ui/icons/Timer";
import BatteryUnknownIcon from "@material-ui/icons/BatteryUnknown";

import {
  RestFormProps,
  FormActions,
  FormButton,
  HighlightAvatar,
} from "../components";
import {
  busStatus,
  busStatusHighlight,
  isConnected,
  mqttStatusHighlight,
} from "./EMSESPStatus";

import { EMSESPStatus } from "./types";

type EMSESPStatusFormProps = RestFormProps<EMSESPStatus> & WithTheme;

class EMSESPStatusForm extends Component<EMSESPStatusFormProps> {
  createListItems() {
    const { data, theme } = this.props;
    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <BuildIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Firmware Version" secondary={data.version} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <TimerIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="System Uptime" secondary={data.uptime} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>
              <BatteryUnknownIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary="Free System Memory"
            secondary={data.free_mem + "%"}
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={mqttStatusHighlight(data, theme)}>
              <SpeakerNotesOffIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText
            primary="MQTT Publish failures"
            secondary={data.mqtt_fails}
          />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={busStatusHighlight(data, theme)}>
              <DeviceHubIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="EMS Bus Status" secondary={busStatus(data)} />
        </ListItem>
        {isConnected(data) && (
          <Fragment>
            <Table size="small" padding="default">
              <TableHead>
                <TableRow>
                  <TableCell>Statistic</TableCell>
                  <TableCell align="center"># Telegrams</TableCell>
                  <TableCell />
                </TableRow>
              </TableHead>
              <TableBody>
                <TableRow>
                  <TableCell component="th" scope="row">
                    (Rx) Received telegrams
                  </TableCell>
                  <TableCell align="center">{data.rx_received}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">
                    (Rx) Incomplete telegrams
                  </TableCell>
                  <TableCell align="center">{data.crc_errors}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">
                    (Tx) Successfully sent telegrams
                  </TableCell>
                  <TableCell align="center">{data.tx_sent}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">
                    (Tx) Send failures
                  </TableCell>
                  <TableCell align="center">{data.tx_errors}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </Fragment>
        )}
        <Divider variant="inset" component="li" />
        <ListItem></ListItem>
      </Fragment>
    );
  }

  render() {
    return (
      <Fragment>
        <List>{this.createListItems()}</List>
        <FormActions>
          <FormButton
            startIcon={<RefreshIcon />}
            variant="contained"
            color="secondary"
            onClick={this.props.loadData}
          >
            Refresh
          </FormButton>
        </FormActions>
      </Fragment>
    );
  }
}

export default withTheme(EMSESPStatusForm);
