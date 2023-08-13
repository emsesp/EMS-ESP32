import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import KeyboardArrowDownOutlinedIcon from '@mui/icons-material/KeyboardArrowDownOutlined';
import KeyboardArrowUpOutlinedIcon from '@mui/icons-material/KeyboardArrowUpOutlined';
import RefreshIcon from '@mui/icons-material/Refresh';
import UnfoldMoreOutlinedIcon from '@mui/icons-material/UnfoldMoreOutlined';
import { Button, Typography, Box } from '@mui/material';
import { useSort, SortToggleType } from '@table-library/react-table-library/sort';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useRequest } from 'alova';
import { useState, useContext, useEffect } from 'react';

import { toast } from 'react-toastify';

import DashboardSensorsAnalogDialog from './DashboardSensorsAnalogDialog';
import DashboardSensorsTemperatureDialog from './DashboardSensorsTemperatureDialog';
import * as EMSESP from './api';

import { DeviceValueUOM, DeviceValueUOM_s, AnalogTypeNames } from './types';
import { temperatureSensorItemValidation, analogSensorItemValidation } from './validators';
import type { TemperatureSensor, AnalogSensor } from './types';
import type { FC } from 'react';
import { ButtonRow, SectionContent } from 'components';

import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

const DashboardSensors: FC = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const [selectedTemperatureSensor, setSelectedTemperatureSensor] = useState<TemperatureSensor>();
  const [selectedAnalogSensor, setSelectedAnalogSensor] = useState<AnalogSensor>();
  const [temperatureDialogOpen, setTemperatureDialogOpen] = useState<boolean>(false);
  const [analogDialogOpen, setAnalogDialogOpen] = useState<boolean>(false);
  const [creating, setCreating] = useState<boolean>(false);

  const { data: sensorData, send: fetchSensorData } = useRequest(() => EMSESP.readSensorData(), {
    initialData: {
      ts: [],
      as: [],
      analog_enabled: false
    }
  });

  const { send: writeTemperatureSensor } = useRequest((data) => EMSESP.writeTemperatureSensor(data), {
    immediate: false
  });

  const { send: writeAnalogSensor } = useRequest((data) => EMSESP.writeAnalogSensor(data), {
    immediate: false
  });

  const isAdmin = me.admin;

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
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }
      &.tr.tr-body.row-select.row-select-single-selected {
        background-color: #3d4752;
        font-weight: normal;
      }
      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
      &:nth-of-type(odd) .td {
        background-color: #303030;
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

  const getSortIcon = (state: any, sortKey: any) => {
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
        NAME: (array) => array.sort((a, b) => a.n.localeCompare(b.n)),
        VALUE: (array) => array.sort((a, b) => a.t - b.t)
      }
    }
  );

  useEffect(() => {
    const timer = setInterval(() => fetchSensorData(), 30000);
    return () => {
      clearInterval(timer);
    };
  });

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

  function formatValue(value: any, uom: number) {
    if (value === undefined) {
      return '';
    }
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return value ? formatDurationMin(value * 60) : LL.NUM_HOURS({ num: 0 });
      case DeviceValueUOM.MINUTES:
        return value ? formatDurationMin(value) : LL.NUM_MINUTES({ num: 0 });
      case DeviceValueUOM.SECONDS:
        return LL.NUM_SECONDS({ num: value });
      case DeviceValueUOM.NONE:
        if (typeof value === 'number') {
          return new Intl.NumberFormat().format(value);
        }
        return value;
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
    if (isAdmin) {
      setSelectedTemperatureSensor(ts);
      setTemperatureDialogOpen(true);
    }
  };

  const onTemperatureDialogClose = () => {
    setTemperatureDialogOpen(false);
  };

  const onTemperatureDialogSave = async (ts: TemperatureSensor) => {
    await writeTemperatureSensor({ id: ts.id, name: ts.n, offset: ts.o })
      .then(() => {
        toast.success(LL.UPDATED_OF(LL.SENSOR(1)));
      })
      .catch(() => {
        toast.error(LL.UPDATE_OF(LL.SENSOR(2)) + ' ' + LL.FAILED(1));
      })
      .finally(async () => {
        setTemperatureDialogOpen(false);
        setSelectedTemperatureSensor(undefined);
        await fetchSensorData();
      });
  };

  const updateAnalogSensor = (as: AnalogSensor) => {
    if (isAdmin) {
      setCreating(false);
      setSelectedAnalogSensor(as);
      setAnalogDialogOpen(true);
    }
  };

  const onAnalogDialogClose = () => {
    setAnalogDialogOpen(false);
  };

  const addAnalogSensor = () => {
    setCreating(true);
    setSelectedAnalogSensor({
      id: Math.floor(Math.random() * (Math.floor(200) - 100) + 100),
      n: '',
      g: 40,
      u: 0,
      v: 0,
      o: 0,
      t: 0,
      f: 1,
      d: false
    });
    setAnalogDialogOpen(true);
  };

  const onAnalogDialogSave = async (as: AnalogSensor) => {
    await writeAnalogSensor({
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
      .finally(async () => {
        setAnalogDialogOpen(false);
        setSelectedAnalogSensor(undefined);
        await fetchSensorData();
      });
  };

  const RenderTemperatureSensors = () => (
    <Table data={{ nodes: sensorData.ts }} theme={temperature_theme} sort={temperature_sort} layout={{ custom: true }}>
      {(tableList: any) => (
        <>
          <Header>
            <HeaderRow>
              <HeaderCell resize>
                <Button
                  fullWidth
                  style={{ fontSize: '14px', justifyContent: 'flex-start' }}
                  endIcon={getSortIcon(temperature_sort.state, 'NAME')}
                  onClick={() => temperature_sort.fns.onToggleSort({ sortKey: 'NAME' })}
                >
                  {LL.NAME(0)}
                </Button>
              </HeaderCell>
              <HeaderCell stiff>
                <Button
                  fullWidth
                  style={{ fontSize: '14px', justifyContent: 'flex-end' }}
                  endIcon={getSortIcon(temperature_sort.state, 'VALUE')}
                  onClick={() => temperature_sort.fns.onToggleSort({ sortKey: 'VALUE' })}
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

  const RenderAnalogSensors = () => (
    <Table data={{ nodes: sensorData.as }} theme={analog_theme} sort={analog_sort} layout={{ custom: true }}>
      {(tableList: any) => (
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
                <Cell stiff>{a.t ? formatValue(a.v, a.u) : ''}</Cell>
              </Row>
            ))}
          </Body>
        </>
      )}
    </Table>
  );

  return (
    <SectionContent title={LL.SENSOR_DATA()} titleGutter>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="secondary">
        {LL.TEMP_SENSORS()}
      </Typography>
      <RenderTemperatureSensors />
      {selectedTemperatureSensor && (
        <DashboardSensorsTemperatureDialog
          open={temperatureDialogOpen}
          onClose={onTemperatureDialogClose}
          onSave={onTemperatureDialogSave}
          selectedItem={selectedTemperatureSensor}
          validator={temperatureSensorItemValidation()}
        />
      )}

      {sensorData?.analog_enabled === true && (
        <>
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
              validator={analogSensorItemValidation(sensorData.as, creating)}
            />
          )}
        </>
      )}

      <ButtonRow>
        <Box mt={2} display="flex" flexWrap="wrap">
          <Box flexGrow={1}>
            <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={fetchSensorData}>
              {LL.REFRESH()}
            </Button>
          </Box>
          <Button
            variant="outlined"
            color="primary"
            startIcon={<AddCircleOutlineOutlinedIcon />}
            onClick={addAnalogSensor}
          >
            {LL.ADD(0) + ' ' + LL.ANALOG_SENSOR(1)}
          </Button>
        </Box>
      </ButtonRow>
    </SectionContent>
  );
};

export default DashboardSensors;
