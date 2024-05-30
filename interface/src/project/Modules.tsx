import { useState } from 'react';
import type { FC } from 'react';
import { useBlocker } from 'react-router-dom';
import { toast } from 'react-toastify';

import CancelIcon from '@mui/icons-material/Cancel';
import CircleIcon from '@mui/icons-material/Circle';
import WarningIcon from '@mui/icons-material/Warning';
import { Box, Button, Typography } from '@mui/material';

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
import { updateState, useRequest } from 'alova';
import {
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import * as EMSESP from './api';
import type { ModuleItem, Modules } from './types';

const Modules: FC = () => {
  const { LL } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);

  const {
    data: modules,
    send: fetchModules,
    error
  } = useRequest(EMSESP.readModules, {
    initialData: [],
    force: true
  });

  const { send: writeModules } = useRequest(
    (data: Modules) => EMSESP.writeModules(data),
    {
      immediate: false
    }
  );

  const modules_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 48px 180px 120px 100px repeat(1, minmax(160px, 1fr));
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      &:nth-of-type(1) {
        text-align: center;
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
        border-top: 1px solid #565656;
        border-bottom: 1px solid #565656;
      }
      &:hover .td {
        border-top: 1px solid #177ac9;
        border-bottom: 1px solid #177ac9;
      }
      &:nth-of-type(odd) .td {
        background-color: #303030;
      }
    `
  });

  const onCancel = async () => {
    await fetchModules().then(() => {
      setNumChanges(0);
    });
  };

  function hasModulesChanged(mi: ModuleItem) {
    return mi.enabled !== mi.o_enabled;
  }

  const selectModule = (updatedItem: ModuleItem) => {
    updatedItem.enabled = !updatedItem.enabled;
    updateState('modules', (data: ModuleItem[]) => {
      const new_data = data.map((mi) =>
        mi.id === updatedItem.id ? { ...mi, ...updatedItem } : mi
      );
      setNumChanges(new_data.filter((mi) => hasModulesChanged(mi)).length);
      return new_data;
    });
  };

  const saveModules = async () => {
    await writeModules({
      modules: modules.map((condensed_mi) => ({
        name: condensed_mi.name,
        enabled: condensed_mi.enabled
      }))
    })
      .then(() => {
        toast.success('Modules saved');
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        await fetchModules();
        setNumChanges(0);
      });
  };

  const renderModules = () => {
    if (!modules) {
      return <FormLoader onRetry={fetchModules} errorMessage={error?.message} />;
    }

    useLayoutTitle('Modules');

    if (modules.length === 0) {
      return (
        <Typography variant="body2" color="error">
          No modules detected
        </Typography>
      );
    }

    return (
      <Table
        data={{ nodes: modules }}
        theme={modules_theme}
        layout={{ custom: true }}
      >
        {(tableList: ModuleItem[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell />
                <HeaderCell stiff>{LL.NAME(0)}</HeaderCell>
                <HeaderCell stiff>Author</HeaderCell>
                <HeaderCell stiff>{LL.VERSION()}</HeaderCell>
                <HeaderCell stiff>{LL.STATUS_OF('')}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((mi: ModuleItem) => (
                <Row key={mi.id} item={mi} onClick={() => selectModule(mi)}>
                  <Cell stiff>
                    {mi.enabled ? (
                      <CircleIcon
                        color="success"
                        sx={{ fontSize: 16, verticalAlign: 'middle' }}
                      />
                    ) : (
                      <CircleIcon
                        color="error"
                        sx={{ fontSize: 16, verticalAlign: 'middle' }}
                      />
                    )}
                  </Cell>
                  <Cell>{mi.name}</Cell>
                  <Cell>{mi.author}</Cell>
                  <Cell>{mi.version}</Cell>
                  <Cell>{mi.status}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Box mb={2} color="warning.main">
        <Typography variant="body2">
          Activate or de-activate EMS-ESP library modules by selecting (**
          experimental **)
        </Typography>
      </Box>
      {renderModules()}

      <Box mt={1} display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges !== 0 && (
            <ButtonRow>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                onClick={onCancel}
                color="secondary"
              >
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={saveModules}
              >
                {LL.APPLY_CHANGES(numChanges)}
              </Button>
            </ButtonRow>
          )}
        </Box>
      </Box>
    </SectionContent>
  );
};

export default Modules;
