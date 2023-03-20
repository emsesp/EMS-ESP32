import { FC, useContext } from 'react';

import { List } from '@mui/material';

import { AuthenticatedContext } from '../contexts/authentication';

import { PROJECT_PATH } from '../api/env';

import { useI18nContext } from '../i18n/i18n-react';

import TuneIcon from '@mui/icons-material/Tune';
import DashboardIcon from '@mui/icons-material/Dashboard';
import LayoutMenuItem from '../components/layout/LayoutMenuItem';
import InfoIcon from '@mui/icons-material/Info';

const ProjectMenu: FC = () => {
  const authenticatedContext = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();

  return (
    <List>
      <LayoutMenuItem icon={DashboardIcon} label={LL.DASHBOARD()} to={`/${PROJECT_PATH}/dashboard`} />
      <LayoutMenuItem
        icon={TuneIcon}
        label={LL.SETTINGS_OF('')}
        to={`/${PROJECT_PATH}/settings`}
        disabled={!authenticatedContext.me.admin}
      />
      <LayoutMenuItem icon={InfoIcon} label={LL.HELP_OF('')} to={`/${PROJECT_PATH}/help`} />
    </List>
  );
};

export default ProjectMenu;
