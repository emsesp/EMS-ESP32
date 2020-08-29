import React, { Component } from "react";
import { Link, withRouter, RouteComponentProps } from "react-router-dom";

import { List, ListItem, ListItemIcon, ListItemText } from "@material-ui/core";

import SettingsIcon from '@material-ui/icons/Settings';
import SettingsRemoteIcon from "@material-ui/icons/SettingsRemote";

import { withAuthenticatedContext, AuthenticatedContextProps } from '../authentication';

type ProjectProps = AuthenticatedContextProps & RouteComponentProps;

class ProjectMenu extends Component<ProjectProps> {
  render() {
    const { authenticatedContext } = this.props;
    const path = this.props.match.url;
    return (
      <List>
        <ListItem to='/ems-esp/' selected={path.startsWith('/ems-esp/status') || path.startsWith('/ems-esp/devices') || path.startsWith('/ems-esp/help')} button component={Link}>
          <ListItemIcon>
            <SettingsRemoteIcon />
          </ListItemIcon>
          <ListItemText primary="Dashboard" />
        </ListItem>
        <ListItem to='/ems-esp/settings' selected={path.startsWith('/ems-esp/settings')} button component={Link} disabled={!authenticatedContext.me.admin}>
          <ListItemIcon>
            <SettingsIcon />
          </ListItemIcon>
          <ListItemText primary="Settings" /> 
        </ListItem>
      </List>
    );
  }
}

export default withRouter(withAuthenticatedContext(ProjectMenu));
