import React, { Component, Fragment } from "react";

import { WithTheme, withTheme } from "@material-ui/core/styles";
import {
  TableContainer,
  Table,
  TableBody,
  TableCell,
  TableRow,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  withWidth,
  WithWidthProps,
  isWidthDown,
} from "@material-ui/core";

import RefreshIcon from "@material-ui/icons/Refresh";
import DeviceHubIcon from "@material-ui/icons/DeviceHub";

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
} from "./EMSESPStatus";

import { EMSESPStatus } from "./EMSESPtypes";

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

type EMSESPStatusFormProps = RestFormProps<EMSESPStatus> & WithTheme & WithWidthProps;

class EMSESPStatusForm extends Component<EMSESPStatusFormProps> {

  createListItems() {
    const { data, theme, width } = this.props;
    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={busStatusHighlight(data, theme)}>
              <DeviceHubIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="Connection Status" secondary={busStatus(data)} />
        </ListItem>
        {isConnected(data) && (
          <TableContainer>
            <Table size="small" padding={isWidthDown('xs', width!) ? "none" : "default"}>
              <TableBody>
                <TableRow>
                  <TableCell>
                    Received telegrams
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.rx_received)}
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    Rx line quality
                  </TableCell>
                  <TableCell align="right">{data.rx_quality}&nbsp;%
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    Sent telegrams
                  </TableCell >
                  <TableCell align="right">{formatNumber(data.tx_sent)}
                  </TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    Tx line quality
                  </TableCell>
                  <TableCell align="right">{data.tx_quality}&nbsp;%
                  </TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </TableContainer>
        )}
      </Fragment>
    );
  }

  render() {
    return (
      <Fragment>
        <List>{this.createListItems()}</List>
        <FormActions>
          <FormButton
            startIcon={<RefreshIcon />} variant="contained" color="secondary" onClick={this.props.loadData}>
            Refresh
          </FormButton>
        </FormActions>
      </Fragment>
    );
  }
}

export default withTheme(withWidth()(EMSESPStatusForm));
