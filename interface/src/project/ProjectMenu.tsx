import React, { Component } from "react";
import { Link, withRouter, RouteComponentProps } from "react-router-dom";

import { List, ListItem, ListItemIcon, ListItemText } from "@material-ui/core";

import SettingsIcon from '@material-ui/icons/Settings';
import SettingsRemoteIcon from "@material-ui/icons/SettingsRemote";

class ProjectMenu extends Component<RouteComponentProps> {
  render() {
    const path = this.props.match.url;
    return (
      <List>
        <ListItem to='/ems-esp/status/' selected={path.startsWith('/ems-esp/status/')} button component={Link}>
          <ListItemIcon>
            <SettingsRemoteIcon />
          </ListItemIcon>
          <ListItemText primary="Dashboard" />
        </ListItem>
        <ListItem to='/ems-esp/settings/' selected={path.startsWith('/ems-esp/settings/')} button component={Link}>
          <ListItemIcon>
            <SettingsIcon />
          </ListItemIcon>
          <ListItemText primary="Settings" />
        </ListItem>
      </List>
    );
  }
}

export default withRouter(ProjectMenu);
