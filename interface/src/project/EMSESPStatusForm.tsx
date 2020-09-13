import React, { Component, Fragment } from "react";

import { WithTheme, withTheme, withStyles, Theme, createStyles } from "@material-ui/core/styles";
import {
  TableContainer,
  Table,
  Box,
  Typography,
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

  rxErrors = () => {
    return this.props.data.crc_errors !== 0;
  }

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
                    (Rx) Incomplete telegrams
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.crc_errors)}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    (Tx) Successfully sent telegrams
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.tx_sent)}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell >
                    (Tx) Send Errors
                  </TableCell>
                  <TableCell align="right">{formatNumber(data.tx_errors)}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
            <Fragment>
              {this.rxErrors() && (
                <Box bgcolor="warning.main" p={1} mt={0} mb={0}>
                  <Typography variant="caption" color="textPrimary">
                    <i>Note: Having a small number of incomplete Rx telegrams is normal and often caused by noise on the EMS line.</i>
                  </Typography>
                </Box>
              )}
            </Fragment>
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
