import { alovaInstance } from './endpoints';

import type { Features } from 'types';

export const readFeatures = () => alovaInstance.Get<Features>('/rest/features');
