import { memo, useContext, useEffect, useState } from 'react';
import { IconContext } from 'react-icons/lib';
import { Link } from 'react-router';

import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import EditIcon from '@mui/icons-material/Edit';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import HelpOutlineIcon from '@mui/icons-material/HelpOutlined';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
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

import { Body, Cell, Row, Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { CellTree, useTree } from '@table-library/react-table-library/tree';
import { useRequest } from 'alova/client';
import {
  ButtonTooltip,
  FormLoader,
  MessageBox,
  SectionContent,
  useLayoutTitle
} from 'components';
import { toast } from 'components/toast';
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

const Dashboard = memo(() => {
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
    error
  } = useRequest(readDashboard, {
    initialData: { connected: true, nodes: [] }
  }).onSuccess((event) => {
    if (event.data.nodes.length !== parentNodes) {
      setParentNodes(event.data.nodes.length); // count number of parents/devices
    }
  });

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    {
      immediate: false
    }
  )
    .onSuccess(() => {
      toast.success(LL.WRITE_CMD_SENT());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const deviceValueDialogSave = async (devicevalue: DeviceValue) => {
    if (!selectedDashboardItem) {
      return;
    }
    // skip if we're executing an immediate schedule as this is handled in DevicesDialog::doAction()
    if (devicevalue.v !== undefined) {
      const id = selectedDashboardItem.parentNode.id; // this is the parent ID
      await sendDeviceValue({ id, c: devicevalue.c ?? '', v: devicevalue.v });
    }
    setDeviceValueDialogOpen(false);
    setSelectedDashboardItem(undefined);
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
    { nodes: [...data.nodes] },
    {
      onChange: () => {} // not used but needed
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
    const nodeIds = data.nodes.map((item: DashboardItem) => item.id);
    showAll
      ? tree.fns.onAddAll(nodeIds) // expand tree
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
        case DeviceType.COMMAND:
          return LL.COMMANDS();
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
          <span style={{ fontSize: '15px' }}>
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
    return null;
  };

  const hasMask = (id: string, mask: number) =>
    (parseInt(id.slice(0, 2), 16) & mask) === mask;

  const editDashboardValue = (di: DashboardItem) => {
    // don't execute on parent nodes
    if (!me.admin || di.id <= 99) {
      return;
    }

    if (di.dv?.c) {
      setSelectedDashboardItem(di);
      setDeviceValueDialogOpen(true);
    }
  };

  const handleShowAll = (
    _event: React.MouseEvent<HTMLElement>,
    toggle: boolean | null
  ) => {
    if (toggle !== null) {
      tree.fns.onToggleAll({});
      setShowAll(toggle);
    }
  };

  const hasFavEntities = data.nodes.filter(
    (item: DashboardItem) => item.id <= 90
  ).length;

  const renderContent = () => {
    if (!data) {
      return (
        <FormLoader onRetry={fetchDashboard} errorMessage={error?.message || ''} />
      );
    }

    return (
      <>
        {data.connected && data.nodes.length > 0 && !hasFavEntities && (
          <MessageBox sx={{ mb: 2 }} level="warning">
            <Typography>
              {LL.NO_DATA_1()}&nbsp;
              <Link to="/customizations" style={{ color: 'white' }}>
                {LL.CUSTOMIZATIONS()}
              </Link>
              &nbsp;{LL.NO_DATA_2()}&nbsp;
              {LL.NO_DATA_3()}&nbsp;
              <Link to="/devices" style={{ color: 'white' }}>
                {LL.DEVICES()}
              </Link>
              .
            </Typography>
          </MessageBox>
        )}

        <Box
          sx={{
            display: 'flex',
            justifyContent: 'flex-end',
            flexWrap: 'nowrap',
            whiteSpace: 'nowrap'
          }}
        >
          <ToggleButtonGroup
            size="small"
            color="primary"
            value={showAll}
            exclusive
            onChange={handleShowAll}
          >
            <ButtonTooltip title={LL.ALLVALUES()}>
              <ToggleButton value={true}>
                <UnfoldMoreIcon sx={{ fontSize: 18 }} />
              </ToggleButton>
            </ButtonTooltip>
            <ButtonTooltip title={LL.COMPACT()}>
              <ToggleButton value={false}>
                <UnfoldLessIcon sx={{ fontSize: 18 }} />
              </ToggleButton>
            </ButtonTooltip>
          </ToggleButtonGroup>
        </Box>

        {data.nodes.length > 0 ? (
          <Box sx={{ mt: 1, justifyContent: 'center', flexDirection: 'column' }}>
            <IconContext.Provider
              value={{
                color: 'lightblue',
                size: '18',
                style: { verticalAlign: 'middle' }
              }}
            >
              <Table
                data={{ nodes: data.nodes }}
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
                              <ButtonTooltip
                                title={formatValue(LL, di.dv?.v, di.dv?.u)}
                              >
                                <span>{formatValue(LL, di.dv?.v, di.dv?.u)}</span>
                              </ButtonTooltip>
                            </Cell>

                            <Cell>
                              {me.admin &&
                                di.dv?.c &&
                                !hasMask(di.dv.id, DeviceEntityMask.DV_READONLY) &&
                                (di.dv.v === '' || di.dv.v === undefined ? (
                                  <IconButton
                                    size="small"
                                    aria-label={LL.RUN()}
                                    onClick={() => editDashboardValue(di)}
                                    style={{ backgroundColor: 'transparent' }}
                                  >
                                    <PlayArrowIcon
                                      color="primary"
                                      sx={{ fontSize: 16 }}
                                    />
                                  </IconButton>
                                ) : (
                                  <IconButton
                                    size="small"
                                    aria-label={
                                      LL.CHANGE_VALUE() + ' ' + LL.VALUE(0)
                                    }
                                    onClick={() => editDashboardValue(di)}
                                    style={{ backgroundColor: 'transparent' }}
                                  >
                                    <EditIcon
                                      color="primary"
                                      sx={{ fontSize: 16 }}
                                    />
                                  </IconButton>
                                ))}
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
            </IconContext.Provider>
          </Box>
        ) : (
          <Box sx={{ display: 'flex' }}>
            <Typography sx={{ mt: 1 }} color="warning" variant="body1">
              no data
            </Typography>
            <Tooltip title={LL.DASHBOARD_1()}>
              <HelpOutlineIcon
                sx={{
                  ml: 1,
                  mt: 1,
                  fontSize: 20,
                  verticalAlign: 'middle'
                }}
                color="primary"
              />
            </Tooltip>
          </Box>
        )}
      </>
    );
  };

  return (
    <SectionContent>
      {renderContent()}
      {selectedDashboardItem && selectedDashboardItem.dv && (
        <DevicesDialog
          open={deviceValueDialogOpen}
          onClose={() => {
            setDeviceValueDialogOpen(false);
            setSelectedDashboardItem(undefined);
          }}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDashboardItem.dv}
          writeable={true}
          validator={deviceValueItemValidation(selectedDashboardItem.dv)}
          progress={submitting}
        />
      )}
    </SectionContent>
  );
});

export default Dashboard;
