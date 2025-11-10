import { memo, useCallback, useContext } from 'react';

import PersonIcon from '@mui/icons-material/Person';
import { Avatar, Box, Button, List, ListItem, ListItemText } from '@mui/material';

import { AuthenticatedContext } from '@/contexts/authentication';
import { SectionContent, useLayoutTitle } from 'components';
import { LanguageSelector } from 'components/inputs';
import { useI18nContext } from 'i18n/i18n-react';

const UserProfileComponent = () => {
  const { LL } = useI18nContext();
  const { me, signOut } = useContext(AuthenticatedContext);

  useLayoutTitle(LL.USER(1));

  const handleSignOut = useCallback(() => {
    signOut(true);
  }, [signOut]);

  return (
    <SectionContent>
      <Box padding={2} justifyContent="center" flexDirection="column">
        <Box display="flex" alignItems="center" justifyContent="space-between">
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
          <LanguageSelector />
        </Box>
        <Button
          sx={{ mt: 2 }}
          variant="outlined"
          fullWidth
          color="primary"
          onClick={handleSignOut}
        >
          {LL.SIGN_OUT()}
        </Button>
      </Box>
    </SectionContent>
  );
};

const UserProfile = memo(UserProfileComponent);

export default UserProfile;
