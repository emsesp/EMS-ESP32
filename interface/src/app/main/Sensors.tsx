import { useCallback, useContext, useMemo, useState } from 'react';
import { toast } from 'react-toastify';

import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import { Box, Button, Typography } from '@mui/material';

import { SortToggleType, useSort } from '@table-library/react-table-library/sort';
import {
  Body,
  Cell,
  Header,
  HeaderCell,
  HeaderRow,
  Row,
  Table
} from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import type { State } from '@table-library/react-table-library/types/common';
import { useRequest } from 'alova/client';
import { SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import {
  readSensorData,
  writeAnalogSensor,
  writeTemperatureSensor
} from '../../api/app';
import DashboardSensorsAnalogDialog from './SensorsAnalogDialog';
import DashboardSensorsTemperatureDialog from './SensorsTemperatureDialog';
import {
  AnalogType,
  AnalogTypeNames,
  DeviceValueUOM,
  DeviceValueUOM_s
} from './types';
import type {
  AnalogSensor,
  TemperatureSensor,
  WriteAnalogSensor,
  WriteTemperatureSensor
} from './types';
import {
  analogSensorItemValidation,
  temperatureSensorItemValidation
} from './validators';

// Constants
const MS_PER_SECOND = 1000;
const MS_PER_MINUTE = 60 * MS_PER_SECOND;
const MS_PER_HOUR = 60 * MS_PER_MINUTE;
const MS_PER_DAY = 24 * MS_PER_HOUR;
const DEFAULT_GPIO = 99; // not set
const MIN_TEMP_ID = -100;
const MAX_TEMP_ID = 100;
const GPIO_25 = 25;
const GPIO_26 = 26;

const HEADER_BUTTON_STYLE: React.CSSProperties = {
  fontSize: '14px',
  justifyContent: 'flex-start'
};

const HEADER_BUTTON_STYLE_END: React.CSSProperties = {
  fontSize: '14px',
  justifyContent: 'flex-end'
};

const common_theme = {
  BaseRow: `
    font-size: 14px;
    .td {
      height: 32px;
    }
  `,
  HeaderRow: `
    text-transform: uppercase;
    background-color: black;
    color: #90CAF9;
    .th {
      border-bottom: 1px solid #565656;
      height: 36px;
    }
  `,
  Row: `
    background-color: #1e1e1e;
    position: relative;
    cursor: pointer;
    .td {
      padding: 8px;
      border-bottom: 1px solid #565656;
    }
    &:hover .td {
      background-color: #177ac9;
      color: white;
    }
  `,
  Cell: `
    &:last-of-type {
      text-align: right;
    },
  `
};

const temperature_theme_config = {
  Table: `
    --data-table-library_grid-template-columns: minmax(0, 1fr) 35%;
  `
};

const analog_theme_config = {
  Table: `
    --data-table-library_grid-template-columns: 80px repeat(1, minmax(0, 1fr)) 120px 110px;
  `
};

const Sensors = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [selectedTemperatureSensor, setSelectedTemperatureSensor] =
    useState<TemperatureSensor>();
  const [selectedAnalogSensor, setSelectedAnalogSensor] = useState<AnalogSensor>();
  const [temperatureDialogOpen, setTemperatureDialogOpen] = useState<boolean>(false);
  const [analogDialogOpen, setAnalogDialogOpen] = useState<boolean>(false);
  const [creating, setCreating] = useState<boolean>(false);

  const { data: sensorData, send: fetchSensorData } = useRequest(readSensorData, {
    initialData: {
      ts: [],
      as: [],
      analog_enabled: false,
      valid_gpio_list: [],
      platform: 'ESP32'
    }
  });

  const { send: sendTemperatureSensor } = useRequest(
    (data: WriteTemperatureSensor) => writeTemperatureSensor(data),
    {
      immediate: false
    }
  );

  const { send: sendAnalogSensor } = useRequest(
    (data: WriteAnalogSensor) => writeAnalogSensor(data),
    {
      immediate: false
    }
  );

  const intervalCallback = useCallback(() => {
    if (!temperatureDialogOpen && !analogDialogOpen) {
      void fetchSensorData();
    }
  }, [temperatureDialogOpen, analogDialogOpen, fetchSensorData]);

  useInterval(intervalCallback);

  const temperature_theme = useTheme([common_theme, temperature_theme_config]);
  const analog_theme = useTheme([common_theme, analog_theme_config]);

  const getSortIcon = useCallback((state: State, sortKey: unknown) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }
    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
    return <UnfoldMoreOutlinedIcon />;
  }, []);

  const analog_sort = useSort(
    { nodes: sensorData.as },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        GPIO: (array) =>
          [...array].sort((a, b) => (a as AnalogSensor).g - (b as AnalogSensor).g),
        NAME: (array) =>
          [...array].sort((a, b) =>
            (a as AnalogSensor).n.localeCompare((b as AnalogSensor).n)
          ),
        TYPE: (array) =>
          [...array].sort((a, b) => (a as AnalogSensor).t - (b as AnalogSensor).t),
        VALUE: (array) =>
          [...array].sort((a, b) => (a as AnalogSensor).v - (b as AnalogSensor).v)
      }
    }
  );

  const temperature_sort = useSort(
    { nodes: sensorData.ts },
    {},
    {
      sortIcon: {
        iconDefault: <UnfoldMoreOutlinedIcon />,
        iconUp: <KeyboardArrowUpOutlinedIcon />,
        iconDown: <KeyboardArrowDownOutlinedIcon />
      },
      sortToggleType: SortToggleType.AlternateWithReset,
      sortFns: {
        NAME: (array) =>
          [...array].sort((a, b) =>
            (a as TemperatureSensor).n.localeCompare((b as TemperatureSensor).n)
          ),
        VALUE: (array) =>
          [...array].sort(
            (a, b) =>
              ((a as TemperatureSensor).t ?? 0) - ((b as TemperatureSensor).t ?? 0)
          )
      }
    }
  );

  useLayoutTitle(LL.SENSORS());

  const formatDurationMin = useCallback(
    (duration_min: number) => {
      const totalMs = duration_min * MS_PER_MINUTE;
      const days = Math.trunc(totalMs / MS_PER_DAY);
      const hours = Math.trunc(totalMs / MS_PER_HOUR) % 24;
      const minutes = Math.trunc(totalMs / MS_PER_MINUTE) % 60;

      const parts: string[] = [];
      if (days > 0) {
        parts.push(LL.NUM_DAYS({ num: days }));
      }
      if (hours > 0) {
        parts.push(LL.NUM_HOURS({ num: hours }));
      }
      if (minutes > 0) {
        parts.push(LL.NUM_MINUTES({ num: minutes }));
      }
      return parts.join(' ');
    },
    [LL]
  );

  const formatValue = useCallback(
    (value: unknown, uom: DeviceValueUOM) => {
      if (value === undefined) {
        return '';
      }
      if (typeof value !== 'number') {
        return value as string;
      }
      switch (uom) {
        case DeviceValueUOM.HOURS:
          return value ? formatDurationMin(value * 60) : LL.NUM_HOURS({ num: 0 });
        case DeviceValueUOM.MINUTES:
          return value ? formatDurationMin(value) : LL.NUM_MINUTES({ num: 0 });
        case DeviceValueUOM.SECONDS:
          return LL.NUM_SECONDS({ num: value });
        case DeviceValueUOM.NONE:
          return new Intl.NumberFormat().format(value);
        case DeviceValueUOM.DEGREES:
        case DeviceValueUOM.DEGREES_R:
        case DeviceValueUOM.FAHRENHEIT:
          return (
            new Intl.NumberFormat(undefined, {
              minimumFractionDigits: 1
            }).format(value) +
            ' ' +
            DeviceValueUOM_s[uom]
          );
        default:
          return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
      }
    },
    [formatDurationMin, LL]
  );

  const updateTemperatureSensor = useCallback(
    (ts: TemperatureSensor) => {
      if (me.admin) {
        ts.o_n = ts.n;
        setSelectedTemperatureSensor(ts);
        setTemperatureDialogOpen(true);
      }
    },
    [me.admin]
  );

  const onTemperatureDialogClose = useCallback(() => {
    setTemperatureDialogOpen(false);
    void fetchSensorData();
  }, [fetchSensorData]);

  const onTemperatureDialogSave = useCallback(
    async (ts: TemperatureSensor) => {
      await sendTemperatureSensor({
        id: ts.id,
        name: ts.n,
        offset: ts.o,
        is_system: ts.s
      })
        .then(() => {
          toast.success(LL.UPDATED_OF(LL.SENSOR(1)));
        })
        .catch(() => {
          toast.error(LL.UPDATE_OF(LL.SENSOR(2)) + ' ' + LL.FAILED(1));
        })
        .finally(() => {
          setTemperatureDialogOpen(false);
          setSelectedTemperatureSensor(undefined);
          void fetchSensorData();
        });
    },
    [sendTemperatureSensor, LL, fetchSensorData]
  );

  const updateAnalogSensor = useCallback(
    (as: AnalogSensor) => {
      if (me.admin) {
        setCreating(false);
        as.o_n = as.n;
        setSelectedAnalogSensor(as);
        setAnalogDialogOpen(true);
      }
    },
    [me.admin]
  );

  const onAnalogDialogClose = useCallback(() => {
    setAnalogDialogOpen(false);
    void fetchSensorData();
  }, [fetchSensorData]);

  const addAnalogSensor = useCallback(() => {
    setCreating(true);
    setSelectedAnalogSensor({
      id: Math.floor(Math.random() * (MAX_TEMP_ID - MIN_TEMP_ID) + MIN_TEMP_ID),
      n: '',
      g: DEFAULT_GPIO,
      u: 0,
      v: 0,
      o: 0,
      t: 0,
      f: 1,
      d: false,
      o_n: '',
      s: false
    });
    setAnalogDialogOpen(true);
  }, []);

  const onAnalogDialogSave = useCallback(
    async (as: AnalogSensor) => {
      await sendAnalogSensor({
        id: as.id,
        gpio: as.g,
        name: as.n,
        offset: as.o,
        factor: as.f,
        uom: as.u,
        type: as.t,
        deleted: as.d,
        is_system: as.s
      })
        .then(() => {
          toast.success(LL.UPDATED_OF(LL.ANALOG_SENSOR(2)));
        })
        .catch(() => {
          toast.error(LL.UPDATE_OF(LL.ANALOG_SENSOR(5)) + ' ' + LL.FAILED(1));
        })
        .finally(() => {
          setAnalogDialogOpen(false);
          setSelectedAnalogSensor(undefined);
          void fetchSensorData();
        });
    },
    [sendAnalogSensor, LL, fetchSensorData]
  );

  const RenderAnalogSensors = useMemo(
    () => (
      <Table
        data={{ nodes: sensorData.as }}
        theme={analog_theme}
        sort={analog_sort}
        layout={{ custom: true }}
      >
        {(tableList: AnalogSensor[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE}
                    endIcon={getSortIcon(analog_sort.state, 'GPIO')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'GPIO' })}
                  >
                    GPIO
                  </Button>
                </HeaderCell>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE}
                    endIcon={getSortIcon(analog_sort.state, 'NAME')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                  >
                    {LL.NAME(0)}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE}
                    endIcon={getSortIcon(analog_sort.state, 'TYPE')}
                    onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'TYPE' })}
                  >
                    {LL.TYPE(0)}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE_END}
                    endIcon={getSortIcon(analog_sort.state, 'VALUE')}
                    onClick={() =>
                      analog_sort.fns.onToggleSort({ sortKey: 'VALUE' })
                    }
                  >
                    {LL.VALUE(0)}
                  </Button>
                </HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((as: AnalogSensor) => (
                <Row
                  style={{ color: as.s ? 'grey' : 'inherit' }}
                  key={as.id}
                  item={as}
                  onClick={() => updateAnalogSensor(as)}
                >
                  <Cell stiff>{as.g !== 99 ? as.g : ''}</Cell>
                  <Cell>{as.n}</Cell>
                  <Cell stiff>{AnalogTypeNames[as.t]} </Cell>
                  {(as.t === AnalogType.DIGITAL_OUT &&
                    as.g !== GPIO_25 &&
                    as.g !== GPIO_26) ||
                  as.t === AnalogType.DIGITAL_IN ||
                  as.t === AnalogType.PULSE ? (
                    <Cell stiff>
                      {as.g !== 99 ? (as.v ? LL.ON() : LL.OFF()) : ''}
                    </Cell>
                  ) : (
                    <Cell stiff>
                      {as.t !== AnalogType.NOTUSED && as.g !== 99
                        ? formatValue(as.v, as.u)
                        : ''}
                    </Cell>
                  )}
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    ),
    [
      analog_sort,
      analog_theme,
      getSortIcon,
      sensorData.as,
      LL,
      updateAnalogSensor,
      formatValue
    ]
  );

  const RenderTemperatureSensors = useMemo(
    () => (
      <Table
        data={{ nodes: sensorData.ts }}
        theme={temperature_theme}
        sort={temperature_sort}
        layout={{ custom: true }}
      >
        {(tableList: TemperatureSensor[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell resize>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE}
                    endIcon={getSortIcon(temperature_sort.state, 'NAME')}
                    onClick={() =>
                      temperature_sort.fns.onToggleSort({ sortKey: 'NAME' })
                    }
                  >
                    {LL.NAME(0)}
                  </Button>
                </HeaderCell>
                <HeaderCell stiff>
                  <Button
                    fullWidth
                    style={HEADER_BUTTON_STYLE_END}
                    endIcon={getSortIcon(temperature_sort.state, 'VALUE')}
                    onClick={() =>
                      temperature_sort.fns.onToggleSort({ sortKey: 'VALUE' })
                    }
                  >
                    {LL.VALUE(0)}
                  </Button>
                </HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((ts: TemperatureSensor) => (
                <Row
                  style={{ color: ts.s ? 'grey' : 'inherit' }}
                  key={ts.id}
                  item={ts}
                  onClick={() => updateTemperatureSensor(ts)}
                >
                  <Cell>{ts.n}</Cell>
                  <Cell>{formatValue(ts.t, ts.u)}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    ),
    [
      temperature_sort,
      temperature_theme,
      getSortIcon,
      sensorData.ts,
      LL,
      updateTemperatureSensor,
      formatValue
    ]
  );

  return (
    <SectionContent>
      <Typography sx={{ pb: 1 }} variant="h6" color="primary">
        {LL.TEMP_SENSORS()}
      </Typography>
      {RenderTemperatureSensors}
      {selectedTemperatureSensor && (
        <DashboardSensorsTemperatureDialog
          open={temperatureDialogOpen}
          onClose={onTemperatureDialogClose}
          onSave={onTemperatureDialogSave}
          selectedItem={selectedTemperatureSensor}
          validator={temperatureSensorItemValidation(
            sensorData.ts,
            selectedTemperatureSensor
          )}
        />
      )}
      <Typography sx={{ pt: 4, pb: 1 }} variant="h6" color="primary">
        {LL.ANALOG_SENSORS()}
      </Typography>
      {RenderAnalogSensors}
      {selectedAnalogSensor && (
        <DashboardSensorsAnalogDialog
          open={analogDialogOpen}
          onClose={onAnalogDialogClose}
          onSave={onAnalogDialogSave}
          creating={creating}
          selectedItem={selectedAnalogSensor}
          analogGPIOList={sensorData.valid_gpio_list}
          validator={analogSensorItemValidation(sensorData.as, selectedAnalogSensor)}
        />
      )}
      {sensorData?.analog_enabled === true && me.admin && (
        <Box mt={2} display="flex" flexWrap="wrap" justifyContent="flex-end">
          <Button
            variant="outlined"
            color="primary"
            startIcon={<AddCircleOutlineOutlinedIcon />}
            onClick={addAnalogSensor}
          >
            {LL.ADD(0)}
          </Button>
        </Box>
      )}
    </SectionContent>
  );
};

export default Sensors;
