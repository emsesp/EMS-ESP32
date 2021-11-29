import { FC } from 'react';

import { SectionContent, MessageBox } from '../components';

const SettingsEntities: FC = () => (
  <SectionContent title="Device Entities" titleGutter>
    <MessageBox
      level="info"
      message="Coming soon! This is where you can customize which device entries are visible in the web dashboard and MQTT"
      my={2}
    />
  </SectionContent>
);

export default SettingsEntities;
