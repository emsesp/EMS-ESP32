import Schema from 'validators/schema';

import { IP_OR_HOSTNAME_VALIDATOR, required } from './shared';

export const createNTPSettingsValidator = () =>
  new Schema({
    server: [required(), IP_OR_HOSTNAME_VALIDATOR],
    tz_label: [required()]
  });
