import React, { Component, Fragment } from "react";

import { WithTheme, withTheme, withStyles, Theme, createStyles } from "@material-ui/core/styles";
import {
  TableContainer,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  withWidth,
  WithWidthProps,
  isWidthDown
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
  isConnected
} from "./EMSESPStatus";

import { EMSESPStatus } from "./EMSESPtypes";

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

type EMSESPStatusFormProps = RestFormProps<EMSESPStatus> & WithTheme & WithWidthProps;

const StyledTableCell = withStyles((theme: Theme) =>
  createStyles({
    head: {
      backgroundColor: theme.palette.common.black,
      color: theme.palette.common.white,
    },
    body: {
      fontSize: 14,
    },
  }),
)(TableCell);

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
          <ListItemText primary="EMS Connection Status" secondary={busStatus(data)} />
        </ListItem>
        {isConnected(data) && (
          <TableContainer>
            <Table size="small" padding={isWidthDown('xs', width!) ? "none" : "default"}>
              <TableHead>
                <TableRow>
                  <StyledTableCell>Statistic</StyledTableCell>
                  <StyledTableCell align="right"># Telegrams</StyledTableCell>
                </TableRow>
              </TableHead>
              <TableBody>
                <TableRow>
                  <TableCell>
                    (Rx) Received telegrams
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.rx_received)}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    (Rx) Quality
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.rx_quality)}%</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    (Tx) Sent telegrams
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.tx_sent)}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    (Tx) Quality
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.tx_quality)}%</TableCell>
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
