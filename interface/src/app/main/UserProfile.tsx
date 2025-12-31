import { memo, useCallback, useContext } from 'react';

import PersonIcon from '@mui/icons-material/Person';
import {
  Avatar,
  Box,
  Button,
  Divider,
  List,
  ListItem,
  ListItemText,
  Typography
} from '@mui/material';

import { AuthenticatedContext } from '@/contexts/authentication';
import { SectionContent, useLayoutTitle } from 'components';
import { LanguageSelector } from 'components/inputs';
import { useI18nContext } from 'i18n/i18n-react';

const UserProfileComponent = () => {
  const { LL } = useI18nContext();
  const { me, signOut } = useContext(AuthenticatedContext);

  useLayoutTitle(LL.USER_PROFILE());

  const handleSignOut = useCallback(() => {
    signOut(true);
  }, [signOut]);

  return (
    <SectionContent>
      <List sx={{ flexGrow: 1 }}>
        <ListItem disablePadding>
          <Avatar sx={{ bgcolor: '#9e9e9e', color: 'white' }}>
            <PersonIcon />
          </Avatar>
          <ListItemText
            sx={{ pl: 2, color: '#2196f3' }}
            primary={me.username}
            secondary={'(' + (me.admin ? LL.ADMINISTRATOR() : LL.GUEST()) + ')'}
          />
        </ListItem>
      </List>
      <Box mt={2} mb={2} display="flex" alignItems="center">
        <Typography mr={2} variant="body1" align="center">
          {LL.LANGUAGE()}:
        </Typography>
        <LanguageSelector />
      </Box>
      <Divider />
      <Button
        sx={{ mt: 2 }}
        variant="outlined"
        color="primary"
        onClick={handleSignOut}
      >
        {LL.SIGN_OUT()}
      </Button>
    </SectionContent>
  );
};

const UserProfile = memo(UserProfileComponent);

export default UserProfile;
