// Catalog of the well known values: key | section | level (1 normal, 2 expert) | optional range min:max:step
// The names of the values come translated from the firmware; the catalog only sorts them.
// Key: <device type>:<group>/<short name> (group hc, dhw, ahs, hs, src without number) or <device type>:<short name>.
// Values that are not listed show up automatically under "Other values" or "Measured values".

export type SectionId =
  | 'mode'
  | 'temps'
  | 'status'
  | 'service'
  | 'energy'
  | 'advanced'
  | 'measure'
  | 'other';

export interface CatalogEntry {
  section: SectionId;
  level: 1 | 2; // 1 = Normal, 2 = only in "All values"
  order: number;
  range?: { min: number; max: number; step: number };
}

const SOURCE = [
  'thermostat:hc/seltemp|temps|1|5:30:0.5',
  'thermostat:hc/currtemp|status|1',
  'thermostat:hc/mode|mode|1',
  'thermostat:hc/modetype|status|1',
  'thermostat:hc/program|mode|1',
  'thermostat:hc/comforttemp|temps|1|5:30:0.5',
  'thermostat:hc/ecotemp|temps|1|5:30:0.5',
  'thermostat:hc/manualtemp|temps|1|5:30:0.5',
  'thermostat:hc/reducetemp|temps|1|5:30:0.5',
  'thermostat:hc/summertemp|mode|1|10:30:1',
  'thermostat:hc/summermode|status|1',
  'thermostat:hc/summersetmode|mode|1',
  'thermostat:hc/vacationmode|status|1',
  'thermostat:hc/nofrostmode|mode|1',
  'thermostat:hc/nofrosttemp|temps|1|-20:10:1',
  'thermostat:hc/targetflowtemp|status|1',
  'thermostat:hc/heatingtype|advanced|2',
  'thermostat:hc/designtemp|advanced|2',
  'thermostat:hc/offsettemp|advanced|2',
  'thermostat:hc/baseflowtemp|advanced|2',
  'thermostat:hc/maxflowtemp|advanced|2',
  'thermostat:hc/roominfluence|advanced|2',
  'thermostat:hc/roominflfactor|advanced|2',
  'thermostat:hc/curroominfl|status|2',
  'thermostat:hc/reducemode|advanced|2',
  'thermostat:hc/noreducetemp|advanced|2',
  'thermostat:hc/fastheatup|advanced|2',
  'thermostat:hc/switchonoptimization|advanced|2',
  'thermostat:hc/controlmode|advanced|2',
  'thermostat:dhw/mode|mode|1',
  'thermostat:dhw/modetype|status|1',
  'thermostat:dhw/settemp|temps|1|30:70:1',
  'thermostat:dhw/settemplow|temps|1|30:70:1',
  'thermostat:dhw/circmode|mode|1',
  'thermostat:dhw/charge|mode|1',
  'thermostat:dhw/extra|mode|1',
  'thermostat:dhw/chargeduration|advanced|2',
  'thermostat:dhw/disinfecting|status|1',
  'thermostat:dhw/disinfectday|advanced|2',
  'thermostat:dhw/disinfecttime|advanced|2',
  'thermostat:dhw/dailyheating|advanced|2',
  'thermostat:dhw/dailyheattime|advanced|2',
  'thermostat:datetime|status|1',
  'thermostat:building|advanced|2',
  'thermostat:minexttemp|advanced|2',
  'thermostat:damping|advanced|2',
  'thermostat:dampedoutdoortemp|status|1',
  'thermostat:floordry|mode|1',
  'thermostat:floordrytemp|status|2',
  'thermostat:intoffset|advanced|2',
  'thermostat:lastcode|service|1',
  'thermostat:solar|advanced|2',
  'boiler:outdoortemp|status|1',
  'boiler:curflowtemp|status|1',
  'boiler:selflowtemp|temps|1|20:90:1',
  'boiler:rettemp|status|1',
  'boiler:syspress|status|1',
  'boiler:exhausttemp|status|2',
  'boiler:heatingactive|status|1',
  'boiler:tapwateractive|status|1',
  'boiler:burngas|status|1',
  'boiler:burngas2|status|2',
  'boiler:flamecurr|status|2',
  'boiler:fanwork|status|2',
  'boiler:ignwork|status|2',
  'boiler:heatingpump|status|1',
  'boiler:heatingpumpmod|status|2',
  'boiler:curburnpow|status|1',
  'boiler:selburnpow|status|2',
  'boiler:burnmaxpower|advanced|2',
  'boiler:burnminpower|advanced|2',
  'boiler:heatingoff|mode|1',
  'boiler:heatingtemp|temps|1|30:90:1',
  'boiler:summertemp|temps|1|10:30:1',
  'boiler:nofrostmode|mode|1',
  'boiler:nofrosttemp|temps|2',
  'boiler:chimneysweeper|service|1',
  'boiler:reset|service|2',
  'boiler:lastcode|service|1',
  'boiler:servicecode|service|1',
  'boiler:servicecodenumber|service|2',
  'boiler:maintenancemessage|service|1',
  'boiler:maintenance|service|1',
  'boiler:maintenancetime|service|1',
  'boiler:maintenancedate|service|1',
  'boiler:emergencyops|service|2',
  'boiler:emergencytemp|service|2',
  'boiler:burnstarts|energy|1',
  'boiler:burnworkmin|energy|1',
  'boiler:burn2workmin|energy|2',
  'boiler:heatworkmin|energy|1',
  'boiler:heatstarts|energy|1',
  'boiler:ubauptime|energy|1',
  'boiler:nrgtotal|energy|1',
  'boiler:nrgheat|energy|1',
  'boiler:dhw/seltemp|temps|1|30:80:1',
  'boiler:dhw/seltemplow|temps|1|30:80:1',
  'boiler:dhw/seltempsingle|temps|2|30:80:1',
  'boiler:dhw/tapactivated|mode|1',
  'boiler:dhw/curtemp|status|1',
  'boiler:dhw/curtemp2|status|2',
  'boiler:dhw/flowtemp|status|2',
  'boiler:dhw/workm|energy|1',
  'boiler:dhw/starts|energy|1',
  'boiler:dhw/meter|energy|1',
  'boiler:dhw/gasmeter|energy|1',
  'mixer:hc/flowtemphc|status|1',
  'mixer:hc/flowsettemp|temps|1|0:90:1',
  'mixer:hc/valvestatus|status|1',
  'mixer:hc/pumpstatus|mode|1',
  'mixer:hc/activated|mode|1',
  'mixer:hc/valvesettime|advanced|2',
  'mixer:hc/flowtempoffset|advanced|2'
];

export const CATALOG: ReadonlyMap<string, CatalogEntry> = new Map(
  SOURCE.map((line, order) => {
    const [key, section, level, range] = line.split('|');
    const entry: CatalogEntry = {
      section: section as SectionId,
      level: level === '1' ? 1 : 2,
      order
    };
    if (range) {
      const [min, max, step] = range.split(':').map(Number);
      entry.range = { min: min ?? 0, max: max ?? 100, step: step ?? 1 };
    }
    return [key as string, entry];
  })
);

export const SECTION_ORDER: readonly SectionId[] = [
  'mode',
  'temps',
  'status',
  'service',
  'energy',
  'advanced',
  'measure',
  'other'
];
