import { useContext, useEffect, useState } from 'react';
import { IconContext } from 'react-icons/lib';
import { toast } from 'react-toastify';

import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import ConstructionIcon from '@mui/icons-material/Construction';
import EditIcon from '@mui/icons-material/Edit';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import UnfoldLessIcon from '@mui/icons-material/UnfoldLess';
import UnfoldMoreIcon from '@mui/icons-material/UnfoldMore';
import {
  Box,
  IconButton,
  ToggleButton,
  ToggleButtonGroup,
  Tooltip,
  Typography
} from '@mui/material';
import Grid from '@mui/material/Grid2';

import { Body, Cell, Row, Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { CellTree, useTree } from '@table-library/react-table-library/tree';
import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval, usePersistState } from 'utils';

import { readDashboard, writeDeviceValue } from '../../api/app';
import DeviceIcon from './DeviceIcon';
import DevicesDialog from './DevicesDialog';
import { formatValue } from './deviceValue';
import {
  type DashboardItem,
  DeviceEntityMask,
  DeviceType,
  type DeviceValue
} from './types';
import { deviceValueItemValidation } from './validators';

const Dashboard = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  useLayoutTitle(LL.DASHBOARD());

  const [showAll, setShowAll] = usePersistState(true, 'showAll');

  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState<boolean>(false);
  const [parentNodes, setParentNodes] = useState<number>(0);
  const [selectedDashboardItem, setSelectedDashboardItem] =
    useState<DashboardItem>();

  const {
    data,
    send: fetchDashboard,
    error,
    loading
  } = useRequest(readDashboard, {
    initialData: []
  }).onSuccess((event) => {
    if (event.data.length !== parentNodes) {
      setParentNodes(event.data.length); // count number of parents/devices
    }
  });

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    {
      immediate: false
    }
  );

  const deviceValueDialogSave = async (devicevalue: DeviceValue) => {
    if (!selectedDashboardItem) {
      return;
    }
    const id = selectedDashboardItem.parentNode.id; // this is the parent ID
    await sendDeviceValue({ id, c: devicevalue.c ?? '', v: devicevalue.v })
      .then(() => {
        toast.success(LL.WRITE_CMD_SENT());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(() => {
        setDeviceValueDialogOpen(false);
        setSelectedDashboardItem(undefined);
      });
  };

  const dashboard_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: minmax(80px, auto) 120px 32px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 28px;
      }
    `,
    Row: `
      cursor: pointer;
      background-color: #1e1e1e;
      &:nth-of-type(odd) .td {
          background-color: #303030;
      },
      &:hover .td {
        background-color: #177ac9;
      },
    `,
    BaseCell: `
      &:nth-of-type(2) {
        text-align: right;
      }
      &:nth-of-type(3) {
        text-align: right;
      }
    `
  });

  const tree = useTree(
    { nodes: data },
    {
      onChange: undefined // not used but needed
    },
    {
      treeIcon: {
        margin: '4px',
        iconDefault: null,
        iconRight: (
          <ChevronRightIcon
            sx={{ fontSize: 16, verticalAlign: 'middle' }}
            color="info"
          />
        ),
        iconDown: (
          <ExpandMoreIcon
            sx={{ fontSize: 16, verticalAlign: 'middle' }}
            color="info"
          />
        )
      },
      indentation: 45
    }
  );

  useInterval(() => {
    if (!deviceValueDialogOpen) {
      void fetchDashboard();
    }
  });

  useEffect(() => {
    showAll
      ? tree.fns.onAddAll(data.map((item: DashboardItem) => item.id)) // expand tree
      : tree.fns.onRemoveAll(); // collapse tree
  }, [parentNodes]);

  const showType = (n?: string, t?: number) => {
    // if we have a name show it
    if (n) {
      return n;
    }
    if (t) {
      // otherwise pick translation based on type
      switch (t) {
        case DeviceType.CUSTOM:
          return LL.CUSTOM_ENTITIES(0);
        case DeviceType.ANALOGSENSOR:
          return LL.ANALOG_SENSORS();
        case DeviceType.TEMPERATURESENSOR:
          return LL.TEMP_SENSORS();
        case DeviceType.SCHEDULER:
          return LL.SCHEDULER();
        default:
          break;
      }
    }
    return '';
  };

  const showName = (di: DashboardItem) => {
    if (di.id < 100) {
      // if its a device (parent node) and has entities
      if (di.nodes?.length) {
        return (
          <span style={{ fontWeight: 'bold', fontSize: '14px' }}>
            <DeviceIcon type_id={di.t ?? 0} />
            &nbsp;&nbsp;{showType(di.n, di.t)}
            <span style={{ color: 'lightblue' }}>&nbsp;({di.nodes?.length})</span>
          </span>
        );
      }
    }
    if (di.dv) {
      return <span>{di.dv.id.slice(2)}</span>;
    }
  };

  const hasMask = (id: string, mask: number) =>
    (parseInt(id.slice(0, 2), 16) & mask) === mask;

  const editDashboardValue = (di: DashboardItem) => {
    if (me.admin && di.dv?.c) {
      setSelectedDashboardItem(di);
      setDeviceValueDialogOpen(true);
    }
  };

  const handleShowAll = (
    event: React.MouseEvent<HTMLElement>,
    toggle: boolean | null
  ) => {
    if (toggle !== null) {
      tree.fns.onToggleAll({});
      setShowAll(toggle);
    }
  };

  const renderContent = () => {
    if (!data) {
      return <FormLoader onRetry={fetchDashboard} errorMessage={error?.message} />;
    }

    return (
      <>
        <Box
          sx={{
            backgroundColor: 'black',
            pt: 1,
            pl: 2
          }}
        >
          <Grid container spacing={0} justifyContent="flex-start">
            <Grid size={11}>
              <Typography mb={2} variant="body1" color="warning">
                {LL.DASHBOARD_1()}&nbsp;
                <ConstructionIcon
                  sx={{ color: '#9e9e9e', fontSize: 20, verticalAlign: 'middle' }}
                />
              </Typography>
            </Grid>

            <Grid size={1} alignItems="end">
              <ToggleButtonGroup
                color="primary"
                size="small"
                value={showAll}
                exclusive
                onChange={handleShowAll}
              >
                <ToggleButton value={true}>
                  <UnfoldMoreIcon sx={{ fontSize: 18 }} />
                </ToggleButton>
                <ToggleButton value={false}>
                  <UnfoldLessIcon sx={{ fontSize: 18 }} />
                </ToggleButton>
              </ToggleButtonGroup>
            </Grid>
          </Grid>
        </Box>

        <Box
          padding={1}
          justifyContent="center"
          flexDirection="column"
          sx={{
            borderRadius: 1,
            border: '1px solid grey'
          }}
        >
          <IconContext.Provider
            value={{
              color: 'lightblue',
              size: '18',
              style: { verticalAlign: 'middle' }
            }}
          >
            {!loading && data.length === 0 ? (
              <Typography variant="subtitle2" color="secondary">
                {LL.NO_DATA()}
              </Typography>
            ) : (
              <Table
                data={{ nodes: data }}
                theme={dashboard_theme}
                layout={{ custom: true }}
                tree={tree}
              >
                {(tableList: DashboardItem[]) => (
                  <Body>
                    {tableList.map((di: DashboardItem) => (
                      <Row
                        key={di.id}
                        item={di}
                        onClick={() => editDashboardValue(di)}
                      >
                        {di.id > 99 ? (
                          <>
                            <Cell>{showName(di)}</Cell>
                            <Cell>
                              <Tooltip
                                placement="left"
                                title={formatValue(LL, di.dv?.v, di.dv?.u)}
                                arrow
                              >
                                <span>{formatValue(LL, di.dv?.v, di.dv?.u)}</span>
                              </Tooltip>
                            </Cell>

                            <Cell>
                              {me.admin &&
                                di.dv?.c &&
                                !hasMask(di.dv.id, DeviceEntityMask.DV_READONLY) && (
                                  <IconButton
                                    size="small"
                                    onClick={() => editDashboardValue(di)}
                                  >
                                    <EditIcon
                                      color="primary"
                                      sx={{ fontSize: 16 }}
                                    />
                                  </IconButton>
                                )}
                            </Cell>
                          </>
                        ) : (
                          <>
                            <CellTree item={di}>{showName(di)}</CellTree>
                            <Cell />
                            <Cell />
                          </>
                        )}
                      </Row>
                    ))}
                  </Body>
                )}
              </Table>
            )}
          </IconContext.Provider>
        </Box>
      </>
    );
  };

  return (
    <SectionContent>
      {renderContent()}
      {selectedDashboardItem && selectedDashboardItem.dv && (
        <DevicesDialog
          open={deviceValueDialogOpen}
          onClose={() => setDeviceValueDialogOpen(false)}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDashboardItem.dv}
          writeable={true}
          validator={deviceValueItemValidation(selectedDashboardItem.dv)}
          progress={submitting}
        />
      )}
    </SectionContent>
  );
};

export default Dashboard;
