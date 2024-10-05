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
import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import { readDashboard, writeDeviceValue } from '../../api/app';
import DeviceIcon from './DeviceIcon';
import DashboardDevicesDialog from './DevicesDialog';
import { formatValue } from './deviceValue';
import { type DashboardItem, DeviceEntityMask, type DeviceValue } from './types';
import { deviceValueItemValidation } from './validators';

const Dashboard = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);

  useLayoutTitle(LL.DASHBOARD());

  const [firstLoad, setFirstLoad] = useState<boolean>(true);
  const [showAll, setShowAll] = useState<boolean>(true);

  const [deviceValueDialogOpen, setDeviceValueDialogOpen] = useState<boolean>(false);

  const [selectedDashboardItem, setSelectedDashboardItem] =
    useState<DashboardItem>();

  const {
    data,
    send: fetchDashboard,
    error,
    loading
  } = useRequest(readDashboard, {
    initialData: []
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
      onChange: null // not used but needed
    },
    {
      treeIcon: {
        margin: '4px',
        iconDefault: null,
        iconRight: <ChevronRightIcon color="primary" />,
        iconDown: <ExpandMoreIcon color="primary" />
      },
      indentation: 50
    }
  );

  useInterval(() => {
    if (!deviceValueDialogOpen) {
      void fetchDashboard();
    }
  }, 3000);

  // auto expand on first load
  useEffect(() => {
    if (firstLoad && Array.isArray(data) && data.length && !tree.state.ids.length) {
      tree.fns.onToggleAll({});
      setFirstLoad(false);
    }
  }, [loading]);

  const showName = (di: DashboardItem) => {
    if (di.id < 100) {
      // if its a device and has entities
      if (di.nodes?.length) {
        return (
          <>
            <span style="font-size: 14px">
              <DeviceIcon type_id={di.t ?? 0} />
              &nbsp;&nbsp;{di.n}
            </span>
            <span style={{ color: 'lightblue' }}>&nbsp;({di.nodes?.length})</span>
          </>
        );
      }
    }
    return <span style="color:lightgrey">{di.dv ? di.dv.id.slice(2) : ''}</span>;
  };

  const hasMask = (id: string, mask: number) =>
    (parseInt(id.slice(0, 2), 16) & mask) === mask;

  const editDashboardValue = (di: DashboardItem) => {
    setSelectedDashboardItem(di);
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

    return (
      <>
        <Typography mb={2} variant="body1" color="warning">
          {LL.DASHBOARD_1()}
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
            {!loading && data.length === 0 ? (
              <Typography variant="subtitle2" color="warning">
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
                      <Row key={di.id} item={di}>
                        {di.id > 99 ? (
                          <Cell>{showName(di)}</Cell>
                        ) : (
                          <CellTree item={di}>{showName(di)}</CellTree>
                        )}
                        <Cell pinRight>
                          <span style={{ color: 'lightgrey' }}>
                            {di.dv && formatValue(LL, di.dv.v, di.dv.u)}
                          </span>
                        </Cell>

                        <Cell stiff>
                          {me.admin &&
                            di.dv?.c &&
                            !hasMask(di.dv.id, DeviceEntityMask.DV_READONLY) && (
                              <IconButton
                                size="small"
                                onClick={() => editDashboardValue(di)}
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
        <DashboardDevicesDialog
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
