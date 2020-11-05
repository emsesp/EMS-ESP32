import React, { Component } from "react";
import { Link, withRouter, RouteComponentProps } from "react-router-dom";

import { List, ListItem, ListItemIcon, ListItemText } from "@material-ui/core";

import TuneIcon from '@material-ui/icons/Tune';
import DashboardIcon from "@material-ui/icons/Dashboard";

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
            <DashboardIcon />
          </ListItemIcon>
          <ListItemText primary="Dashboard" />
        </ListItem>
        <ListItem to='/ems-esp/settings' selected={path.startsWith('/ems-esp/settings')} button component={Link} disabled={!authenticatedContext.me.admin}>
          <ListItemIcon>
            <TuneIcon />
          </ListItemIcon>
          <ListItemText primary="Settings" /> 
        </ListItem>
      </List>
    );
  }
}

export default withRouter(withAuthenticatedContext(ProjectMenu));
