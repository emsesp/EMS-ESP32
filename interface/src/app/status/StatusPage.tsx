import { type ReactNode, useContext } from 'react';

import {
  ButtonLink,
  Hero,
  type IconName,
  KeyValueList,
  type KeyValueRow,
  Section,
  type StatusState
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

export interface StatusPageProps {
  state: StatusState;
  icon: IconName;
  title: string;
  text?: ReactNode;
  // admins get a button to these settings
  settings?: { to: string; label: string; icon?: IconName };
  // e.g. a StatRow between the hero and the details
  children?: ReactNode;
  rows: (KeyValueRow | false | null | undefined)[];
}

// hero, optional extras, then a Details card; used by Network, MQTT, NTP and Access Point
const StatusPage = ({
  state,
  icon,
  title,
  text,
  settings,
  children,
  rows
}: StatusPageProps) => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  return (
    <>
      <Hero
        state={state}
        icon={icon}
        title={title}
        text={text}
        actions={
          settings &&
          me.admin && (
            <ButtonLink
              to={settings.to}
              color="secondary"
              icon={settings.icon ?? 'settings'}
            >
              {settings.label}
            </ButtonLink>
          )
        }
      />
      {children}
      <Section icon="info" title={LL.DETAILS()}>
        <KeyValueList rows={rows} />
      </Section>
    </>
  );
};

export default StatusPage;
