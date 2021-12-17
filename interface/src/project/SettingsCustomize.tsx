import { FC } from 'react';

import { SectionContent, MessageBox } from '../components';

const SettingsCustomize: FC = () => (
  <SectionContent title="Device Entity Customization" titleGutter>
    <MessageBox
      level="info"
      message="Coming soon! This is where you can customize which device entries are visible in the web dashboard, MQTT and API"
      my={2}
    />
  </SectionContent>
);

export default SettingsCustomize;
