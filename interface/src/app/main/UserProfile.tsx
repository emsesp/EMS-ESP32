import { useContext } from 'react';
import { Link } from 'react-router';

import { Button, Icon, LanguageGrid, LinkList, Section, Tag } from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import './UserProfile.css';

const UserProfile = () => {
  const { LL } = useI18nContext();
  const { me, signOut } = useContext(AuthenticatedContext);

  return (
    <>
      <Section>
        <div className="profile">
          <span className="avatar big">
            <Icon name="person" />
          </span>
          <div className="grow">
            <div className="h6 profile-name">{me.username}</div>
            <div className="row wrap">
              <Tag
                kind={me.admin ? 'warn' : 'ok'}
                icon={me.admin ? 'admin_panel_settings' : 'group'}
              >
                {me.admin ? LL.ADMINISTRATOR() : LL.GUEST()}
              </Tag>
              <span className="cap">
                {me.admin ? LL.ADMIN_ACCESS_TEXT() : LL.GUEST_ACCESS_TEXT()}
              </span>
            </div>
          </div>
          <Button icon="logout" onClick={() => signOut(true)}>
            {LL.SIGN_OUT()}
          </Button>
        </div>
      </Section>

      <Section
        title={LL.LANGUAGE()}
        icon="translate"
        desc={LL.LANGUAGE_BROWSER_ONLY()}
      >
        <LanguageGrid />
        {me.admin && (
          <p className="cap profile-note">
            {LL.LANGUAGE_ENTITIES_HINT()}{' '}
            <Link to="/settings/application">{LL.APPLICATION()}</Link>
          </p>
        )}
      </Section>

      {me.admin && (
        <Section title={LL.ADMINISTRATION()} icon="admin_panel_settings">
          <LinkList
            items={[
              {
                to: '/settings/security/users',
                icon: 'group',
                title: LL.MANAGE_USERS(),
                text: LL.SECURITY_1()
              },
              {
                to: '/settings/security/apikeys',
                icon: 'key',
                title: LL.API_KEYS(),
                text: LL.API_KEYS_SHORT()
              }
            ]}
          />
        </Section>
      )}
    </>
  );
};

export default UserProfile;
