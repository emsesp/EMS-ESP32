import { useEffect, useState } from 'react';

import ChevronRightIcon from '@mui/icons-material/ChevronRight';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import UnfoldLessIcon from '@mui/icons-material/UnfoldLess';
import UnfoldMoreIcon from '@mui/icons-material/UnfoldMore';
import { Box, ToggleButton, ToggleButtonGroup, Typography } from '@mui/material';

import { Body, Cell, Row, Table } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { CellTree, useTree } from '@table-library/react-table-library/tree';
import { useAutoRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import { readDashboard } from '../../api/app';
import { formatValue } from './deviceValue';
import type { DashboardItem } from './types';

const Dashboard = () => {
  const { LL } = useI18nContext();

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

  const dashboard_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: minmax(80px, auto) 120px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    Row: `
      background-color: #1e1e1e;
      // position: relative;
      // cursor: pointer;
      .td {
        height: 24px;
      }
      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
    `
  });

  function onTreeChange(action, state) {
    // do nothing for now
  }

  const tree = useTree(
    { nodes: data },
    {
      onChange: onTreeChange
    },
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
  }, [data]);

  const showName = (di: DashboardItem) => {
    if (di.id < 100) {
      if (di.nodes?.length) {
        return (
          <span>
            <span style={{ color: '#2196f3' }}>{di.n}</span>
            <span style={{ color: 'lightblue' }}>&nbsp;({di.nodes?.length})</span>
          </span>
        );
      }
      return <div style={{ color: '#2196f3' }}>{di.n}</div>;
    }

    return <div>{di.n}</div>;
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
                    <Cell pinRight>{formatValue(LL, di.v, di.u)}</Cell>
                  </Row>
                ))}
              </Body>
            )}
          </Table>
        </Box>
      </>
    );
  };

  return <SectionContent>{renderContent()}</SectionContent>;
};

export default Dashboard;
