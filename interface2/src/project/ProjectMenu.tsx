import { FC } from 'react';

import { List } from '@mui/material';

import { PROJECT_PATH } from '../api/env';

import TuneIcon from '@mui/icons-material/Tune';
import DashboardIcon from '@mui/icons-material/Dashboard';
import LayoutMenuItem from '../components/layout/LayoutMenuItem';

const ProjectMenu: FC = () => (
  <List>
    <LayoutMenuItem icon={DashboardIcon} label="Dashboard" to={`/${PROJECT_PATH}/dashboard`} />
    <LayoutMenuItem icon={TuneIcon} label="Settings" to={`/${PROJECT_PATH}/settings`} />
  </List>
);

export default ProjectMenu;
