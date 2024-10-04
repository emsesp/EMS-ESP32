import { useContext, useEffect, useState } from 'react';
import { IconContext } from 'react-icons/lib';
import { toast } from 'react-toastify';

import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import EditIcon from '@mui/icons-material/Edit';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import UnfoldLessIcon from '@mui/icons-material/UnfoldLess';
import UnfoldMoreIcon from '@mui/icons-material/UnfoldMore';
import {
  Box,
  IconButton,
  ToggleButton,
  ToggleButtonGroup,
  Typography
} from '@mui/material';

import { Body, Cell, Row, Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { CellTree, useTree } from '@table-library/react-table-library/tree';
import { useAutoRequest, useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import { readDashboard, writeDeviceValue } from '../../api/app';
import DeviceIcon from './DeviceIcon';
import DashboardDevicesDialog from './DevicesDialog';
import { formatValue } from './deviceValue';
import { type DashboardItem, type DeviceValue } from './types';
import { deviceValueItemValidation } from './validators';

const Dashboard = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  useLayoutTitle('Dashboard'); // TODO translate

  const [firstLoad, setFirstLoad] = useState<boolean>(true);
  const [showAll, setShowAll] = useState<boolean>(true);

  const {
    data,
    send: fetchDashboard,
    error
  } = useAutoRequest(readDashboard, {
    initialData: [],
    pollingTime: 1500
  });

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    {
      immediate: false
    }
  );

  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState(false);
  const [selectedDeviceValue, setSelectedDeviceValue] = useState<DeviceValue>();

  const deviceValueDialogClose = () => {
    setDeviceValueDialogOpen(false);
    void sendDeviceData(selectedDevice);
  };
  const deviceValueDialogSave = async (devicevalue: DeviceValue) => {
    const id = Number(device_select.state.id);
    await sendDeviceValue({ id, c: devicevalue.c ?? '', v: devicevalue.v })
      .then(() => {
        toast.success(LL.WRITE_CMD_SENT());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        setDeviceValueDialogOpen(false);
        await sendDeviceData(id);
        setSelectedDeviceValue(undefined);
      });
  };

  const dashboard_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: minmax(80px, auto) 120px 40px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    Row: `
      background-color: #1e1e1e;
      .td {
        height: 22px;
      }
      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
    `
  });

  const tree = useTree(
    { nodes: data },
    {
      treeIcon: {
        margin: '4px',
        iconDefault: null,
        iconRight: <ChevronRightIcon color="primary" />,
        iconDown: <ExpandMoreIcon color="primary" />
      },
      indentation: 28
    }
  );

  // auto expand on first load
  useEffect(() => {
    if (data.length && firstLoad && !tree.state.ids.length) {
      tree.fns.onToggleAll({});
      setFirstLoad(false);
    }
  });

  const showName = (di: DashboardItem) => {
    if (di.id < 100) {
      // if its a device row
      if (di.nodes?.length) {
        return (
          <>
            <span style="font-size: 14px;">
              <DeviceIcon type_id={di.t} />
              &nbsp;&nbsp;{di.n}
            </span>
            <span style={{ color: 'lightblue' }}>&nbsp;({di.nodes?.length})</span>
          </>
        );
      }
    }
    return <div style={{ color: '#d3d3d3' }}>{di.n}</div>;
  };

  const showDeviceValue = (di: DashboardItem) => {
    // convert di to dv
    // TODO should we not just use dv?
    const dv: DeviceValue = {
      id: '  ' + di.n,
      v: di.v,
      u: di.u,
      c: di.c,
      l: di.l
    };
    setSelectedDeviceValue(dv);
    setDeviceValueDialogOpen(true);
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

    if (data.length === 0) {
      return (
        <Typography variant="body2" color="warning">
          {/* TODO translate */}
          No entities found.
        </Typography>
      );
    }

    return (
      <>
        <Typography mb={2} variant="body2" color="warning">
          {/* TODO translate */}
          The dashboard shows all EMS entities that are marked as favorite, and the
          sensors.
        </Typography>

        <ToggleButtonGroup
          color="primary"
          size="small"
          value={showAll}
          exclusive
          onChange={handleShowAll}
        >
          <ToggleButton value={true}>
            <UnfoldMoreIcon fontSize="small" />
          </ToggleButton>
          <ToggleButton value={false}>
            <UnfoldLessIcon fontSize="small" />
          </ToggleButton>
        </ToggleButtonGroup>

        <Box
          mt={2}
          padding={1}
          justifyContent="center"
          flexDirection="column"
          sx={{
            borderRadius: 2,
            border: '1px solid grey'
          }}
        >
          <IconContext.Provider
            value={{
              color: 'lightblue',
              size: '16',
              style: { verticalAlign: 'middle' }
            }}
          >
            <Table
              data={{ nodes: data }}
              theme={dashboard_theme}
              layout={{ custom: true }}
              tree={tree}
            >
              {(tableList: DashboardItem[]) => (
                <Body>
                  {tableList.map((di: DashboardItem) => (
                    <Row key={di.id} item={di} disabled={di.nodes?.length === 0}>
                      {di.nodes?.length === 0 ? (
                        <Cell>{showName(di)}</Cell>
                      ) : (
                        <CellTree item={di}>{showName(di)}</CellTree>
                      )}
                      <Cell pinRight>
                        <div style={{ color: '#d3d3d3' }}>
                          {formatValue(LL, di.v, di.u)}
                        </div>
                      </Cell>

                      <Cell stiff>
                        {me.admin && di.c && (
                          <IconButton
                            size="small"
                            onClick={() => showDeviceValue(di)}
                          >
                            <EditIcon color="primary" sx={{ fontSize: 16 }} />
                          </IconButton>
                        )}
                      </Cell>
                    </Row>
                  ))}
                </Body>
              )}
            </Table>
          </IconContext.Provider>
        </Box>
      </>
    );
  };

  return (
    <SectionContent>
      {renderContent()}
      {selectedDeviceValue && (
        <DashboardDevicesDialog
          open={deviceValueDialogOpen}
          onClose={deviceValueDialogClose}
          onSave={deviceValueDialogSave}
          selectedItem={selectedDeviceValue}
          writeable={true}
          validator={deviceValueItemValidation(selectedDeviceValue)}
          progress={submitting}
        />
      )}
    </SectionContent>
  );
};

export default Dashboard;
