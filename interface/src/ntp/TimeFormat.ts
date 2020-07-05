import moment, { Moment } from 'moment';

export const formatIsoDateTime = (isoDateString: string) => moment.parseZone(isoDateString).format('ll @ HH:mm:ss');

export const formatLocalDateTime = (moment: Moment) => moment.format('YYYY-MM-DDTHH:mm');
