import { useCallback, useState } from 'react';
import { useBlocker } from 'react-router';
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
import { updateState, useRequest } from 'alova/client';
import {
  BlockNavigation,
  ButtonRow,
  FormLoader,
  SectionContent,
  useLayoutTitle
} from 'components';
import { useI18nContext } from 'i18n/i18n-react';

import { readModules, writeModules } from '../../api/app';
import ModulesDialog from './ModulesDialog';
import type { ModuleItem } from './types';

const Modules = () => {
  const { LL } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);

  const [selectedModuleItem, setSelectedModuleItem] = useState<ModuleItem>();
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  useLayoutTitle(LL.MODULES());

  const {
    data: modules,
    send: fetchModules,
    error
  } = useRequest(readModules, {
    initialData: []
  });

  const { send: updateModules } = useRequest(
    (data: { key: string; enabled: boolean; license: string }) => writeModules(data),
    {
      immediate: false
    }
  );

  const modules_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: 48px 180px 120px 100px repeat(1, minmax(160px, 1fr)) 180px;
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

  const onDialogClose = () => {
    setDialogOpen(false);
  };

  const onDialogSave = (updatedItem: ModuleItem) => {
    setDialogOpen(false);
    updateModuleItem(updatedItem);
  };

  const editModuleItem = useCallback((mi: ModuleItem) => {
    setSelectedModuleItem(mi);
    setDialogOpen(true);
  }, []);

  const onCancel = async () => {
    await fetchModules().then(() => {
      setNumChanges(0);
    });
  };

  function hasModulesChanged(mi: ModuleItem) {
    return mi.enabled !== mi.o_enabled || mi.license !== mi.o_license;
  }

  const updateModuleItem = (updatedItem: ModuleItem) => {
    void updateState(readModules(), (data: ModuleItem[]) => {
      const new_data = data.map((mi) =>
        mi.id === updatedItem.id ? { ...mi, ...updatedItem } : mi
      );
      setNumChanges(new_data.filter((mi) => hasModulesChanged(mi)).length);
      return new_data;
    });
  };

  const saveModules = async () => {
    await updateModules({
      modules: modules.map((condensed_mi) => ({
        key: condensed_mi.key,
        enabled: condensed_mi.enabled,
        license: condensed_mi.license
      }))
    })
      .then(() => {
        toast.success(LL.MODULES_UPDATED());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        await fetchModules();
        setNumChanges(0);
      });
  };

  const renderContent = () => {
    if (!modules) {
      return <FormLoader onRetry={fetchModules} errorMessage={error?.message} />;
    }

    if (modules.length === 0) {
      return (
        <Typography variant="body2" color="error">
          {LL.MODULES_NONE()}
        </Typography>
      );
    }

    const colorStatus = (status: number) => {
      if (status === 1) {
        return <div style={{ color: 'red' }}>Pending Activation</div>;
      }
      return <div style={{ color: '#00FF7F' }}>Activated</div>;
    };

    return (
      <>
        <Box mb={2} color="warning.main">
          <Typography variant="body1">{LL.MODULES_DESCRIPTION()}.</Typography>
        </Box>
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
                  <HeaderCell>{LL.NAME(0)}</HeaderCell>
                  <HeaderCell>Author</HeaderCell>
                  <HeaderCell>{LL.VERSION()}</HeaderCell>
                  <HeaderCell>Message</HeaderCell>
                  <HeaderCell>{LL.STATUS_OF('')}</HeaderCell>
                </HeaderRow>
              </Header>
              <Body>
                {tableList.map((mi: ModuleItem) => (
                  <Row key={mi.id} item={mi} onClick={() => editModuleItem(mi)}>
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
                    <Cell>{mi.message}</Cell>
                    <Cell>{colorStatus(mi.status)}</Cell>
                  </Row>
                ))}
              </Body>
            </>
          )}
        </Table>

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
      </>
    );
  };

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {renderContent()}
      {selectedModuleItem && (
        <ModulesDialog
          open={dialogOpen}
          onClose={onDialogClose}
          onSave={onDialogSave}
          selectedItem={selectedModuleItem}
        />
      )}
    </SectionContent>
  );
};

export default Modules;
