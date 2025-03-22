import { useContext, useState } from 'react';
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

const Sensors = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  const [selectedTemperatureSensor, setSelectedTemperatureSensor] =
    useState<TemperatureSensor>();
  const [selectedAnalogSensor, setSelectedAnalogSensor] = useState<AnalogSensor>();
  const [temperatureDialogOpen, setTemperatureDialogOpen] = useState<boolean>(false);
  const [analogDialogOpen, setAnalogDialogOpen] = useState<boolean>(false);
  const [creating, setCreating] = useState<boolean>(false);

  const { data: sensorData, send: fetchSensorData } = useRequest(
    () => readSensorData(),
    {
      initialData: {
        ts: [],
        as: [],
        analog_enabled: false,
        platform: 'ESP32'
      }
    }
  );

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

  useInterval(() => {
    if (!temperatureDialogOpen && !analogDialogOpen) {
      void fetchSensorData();
    }
  });

  const common_theme = useTheme({
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
      }
      .th {
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
      }
    `,
    Cell: `
      &:last-of-type {
        text-align: right;
      },
    `
  });

  const temperature_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: minmax(0, 1fr) 35%;
      `
    }
  ]);

  const analog_theme = useTheme([
    common_theme,
    {
      Table: `
        --data-table-library_grid-template-columns: 80px repeat(1, minmax(0, 1fr)) 120px 110px;
      `
    }
  ]);

  const RenderTemperatureSensors = () => (
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
                  style={{ fontSize: '14px', justifyContent: 'flex-start' }}
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
                  style={{ fontSize: '14px', justifyContent: 'flex-end' }}
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
              <Row key={ts.id} item={ts} onClick={() => updateTemperatureSensor(ts)}>
                <Cell>{ts.n}</Cell>
                <Cell>{formatValue(ts.t, ts.u)}</Cell>
              </Row>
            ))}
          </Body>
        </>
      )}
    </Table>
  );

  const getSortIcon = (state: State, sortKey: unknown) => {
    if (state.sortKey === sortKey && state.reverse) {
      return <KeyboardArrowDownOutlinedIcon />;
    }
    if (state.sortKey === sortKey && !state.reverse) {
      return <KeyboardArrowUpOutlinedIcon />;
    }
    return <UnfoldMoreOutlinedIcon />;
  };

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
        GPIO: (array) => array.sort((a, b) => a.g - b.g),
        // eslint-disable-next-line @typescript-eslint/no-unsafe-return
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        TYPE: (array) => array.sort((a, b) => a.t - b.t),
        VALUE: (array) => array.sort((a, b) => a.v - b.v)
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
        // eslint-disable-next-line @typescript-eslint/no-unsafe-return
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        VALUE: (array) => array.sort((a, b) => a.t - b.t)
      }
    }
  );

  useLayoutTitle(LL.SENSORS());

  const formatDurationMin = (duration_min: number) => {
    const days = Math.trunc((duration_min * 60000) / 86400000);
    const hours = Math.trunc((duration_min * 60000) / 3600000) % 24;
    const minutes = Math.trunc((duration_min * 60000) / 60000) % 60;

    let formatted = '';
    if (days) {
      formatted += LL.NUM_DAYS({ num: days }) + ' ';
    }
    if (hours) {
      formatted += LL.NUM_HOURS({ num: hours }) + ' ';
    }
    if (minutes) {
      formatted += LL.NUM_MINUTES({ num: minutes });
    }
    return formatted;
  };

  function formatValue(value: unknown, uom: DeviceValueUOM) {
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
  }

  const updateTemperatureSensor = (ts: TemperatureSensor) => {
    if (me.admin) {
      ts.o_n = ts.n;
      setSelectedTemperatureSensor(ts);
      setTemperatureDialogOpen(true);
    }
  };

  const onTemperatureDialogClose = () => {
    setTemperatureDialogOpen(false);
    void fetchSensorData();
  };

  const onTemperatureDialogSave = async (ts: TemperatureSensor) => {
    await sendTemperatureSensor({ id: ts.id, name: ts.n, offset: ts.o })
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
  };

  const updateAnalogSensor = (as: AnalogSensor) => {
    if (me.admin) {
      setCreating(false);
      as.o_n = as.n;
      setSelectedAnalogSensor(as);
      setAnalogDialogOpen(true);
    }
  };

  const onAnalogDialogClose = () => {
    setAnalogDialogOpen(false);
    void fetchSensorData();
  };

  const addAnalogSensor = () => {
    setCreating(true);
    setSelectedAnalogSensor({
      id: Math.floor(Math.random() * (Math.floor(200) - 100) + 100),
      n: '',
      g: 21, // default GPIO 21 which is safe for all platforms
      u: 0,
      v: 0,
      o: 0,
      t: 0,
      f: 1,
      d: false,
      o_n: ''
    });
    setAnalogDialogOpen(true);
  };

  const onAnalogDialogSave = async (as: AnalogSensor) => {
    await sendAnalogSensor({
      id: as.id,
      gpio: as.g,
      name: as.n,
      offset: as.o,
      factor: as.f,
      uom: as.u,
      type: as.t,
      deleted: as.d
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
  };

  const RenderAnalogSensors = () => (
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
                  style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                  endIcon={getSortIcon(analog_sort.state, 'GPIO')}
                  onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'GPIO' })}
                >
                  GPIO
                </Button>
              </HeaderCell>
              <HeaderCell resize>
                <Button
                  fullWidth
                  style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                  endIcon={getSortIcon(analog_sort.state, 'NAME')}
                  onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                >
                  {LL.NAME(0)}
                </Button>
              </HeaderCell>
              <HeaderCell stiff>
                <Button
                  fullWidth
                  style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                  endIcon={getSortIcon(analog_sort.state, 'TYPE')}
                  onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'TYPE' })}
                >
                  {LL.TYPE(0)}
                </Button>
              </HeaderCell>
              <HeaderCell stiff>
                <Button
                  fullWidth
                  style={{ fontSize: '14px', justifyContent: 'flex-end' }}
                  endIcon={getSortIcon(analog_sort.state, 'VALUE')}
                  onClick={() => analog_sort.fns.onToggleSort({ sortKey: 'VALUE' })}
                >
                  {LL.VALUE(0)}
                </Button>
              </HeaderCell>
            </HeaderRow>
          </Header>
          <Body>
            {tableList.map((a: AnalogSensor) => (
              <Row key={a.id} item={a} onClick={() => updateAnalogSensor(a)}>
                <Cell stiff>{a.g}</Cell>
                <Cell>{a.n}</Cell>
                <Cell stiff>{AnalogTypeNames[a.t]} </Cell>
                {(a.t === AnalogType.DIGITAL_OUT && a.g !== 25 && a.g !== 26) ||
                a.t === AnalogType.DIGITAL_IN ? (
                  <Cell stiff>{a.v ? LL.ON() : LL.OFF()}</Cell>
                ) : (
                  <Cell stiff>{a.t ? formatValue(a.v, a.u) : ''}</Cell>
                )}
              </Row>
            ))}
          </Body>
        </>
      )}
    </Table>
  );

  return (
    <SectionContent>
      <Typography sx={{ pb: 1 }} variant="h6" color="secondary">
        {LL.TEMP_SENSORS()}
      </Typography>
      <RenderTemperatureSensors />
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
      <Typography sx={{ pt: 4, pb: 1 }} variant="h6" color="secondary">
        {LL.ANALOG_SENSORS()}
      </Typography>
      <RenderAnalogSensors />
      {selectedAnalogSensor && (
        <DashboardSensorsAnalogDialog
          open={analogDialogOpen}
          onClose={onAnalogDialogClose}
          onSave={onAnalogDialogSave}
          creating={creating}
          selectedItem={selectedAnalogSensor}
          validator={analogSensorItemValidation(
            sensorData.as,
            selectedAnalogSensor,
            creating,
            sensorData.platform
          )}
        />
      )}
      {sensorData?.analog_enabled === true && me.admin && (
        <Box mt={1} display="flex" flexWrap="wrap" justifyContent="flex-end">
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
