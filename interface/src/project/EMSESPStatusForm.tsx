import React, { Component, Fragment } from "react";

import { WithTheme, withTheme } from "@material-ui/core/styles";
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Typography,
  Box,
  Link
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

type EMSESPStatusFormProps = RestFormProps<EMSESPStatus> & WithTheme;

class EMSESPStatusForm extends Component<EMSESPStatusFormProps> {
  createListItems() {
    const { data, theme } = this.props;
    return (
      <Fragment>
        <Box bgcolor="info.main" border={1} p={3} mt={1} mb={0}>
          <Typography variant="body1">
            Firmware Version is <b>{data.version}</b>
            <br /><br />
            Check for news and updates on the <Link href="https://emsesp.github.io/docs/#/" color="primary">{'Wiki'}</Link>
            <br/>
            For live community chat go to <Link href="https://gitter.im/EMS-ESP/community#" color="primary">{'Gitter'}</Link>
            <br/>
            To report issues, contribute and give kudos visit <Link href="https://github.com/proddy/EMS-ESP" color="primary">{'github.com/proddy/EMS-ESP'}</Link>
          </Typography>
        </Box>
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={busStatusHighlight(data, theme)}>
              <DeviceHubIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="EMS Connection Status" secondary={busStatus(data)} />
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
                    (Tx) Send Errors
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
