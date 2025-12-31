import { memo, useCallback, useMemo, useState } from 'react';
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

const PENDING_COLOR = 'red';
const ACTIVATED_COLOR = '#00FF7F';

const hasModulesChanged = (mi: ModuleItem): boolean =>
  mi.enabled !== mi.o_enabled || mi.license !== mi.o_license;

const ColorStatus = memo(({ status }: { status: number }) => {
  if (status === 1) {
    return <div style={{ color: PENDING_COLOR }}>Pending Activation</div>;
  }
  return <div style={{ color: ACTIVATED_COLOR }}>Activated</div>;
});

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

  const modules_theme = useTheme(
    useMemo(
      () => ({
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
      }),
      []
    )
  );

  const onDialogClose = useCallback(() => {
    setDialogOpen(false);
  }, []);

  const updateModuleItem = useCallback((updatedItem: ModuleItem) => {
    void updateState(readModules(), (data: ModuleItem[]) => {
      const new_data = data.map((mi) =>
        mi.id === updatedItem.id ? { ...mi, ...updatedItem } : mi
      );
      setNumChanges(new_data.filter(hasModulesChanged).length);
      return new_data;
    });
  }, []);

  const onDialogSave = useCallback(
    (updatedItem: ModuleItem) => {
      setDialogOpen(false);
      updateModuleItem(updatedItem);
    },
    [updateModuleItem]
  );

  const editModuleItem = useCallback((mi: ModuleItem) => {
    setSelectedModuleItem(mi);
    setDialogOpen(true);
  }, []);

  const onCancel = useCallback(async () => {
    await fetchModules().then(() => {
      setNumChanges(0);
    });
  }, [fetchModules]);

  const saveModules = useCallback(async () => {
    try {
      await Promise.all(
        modules.map((condensed_mi: ModuleItem) =>
          updateModules({
            key: condensed_mi.key,
            enabled: condensed_mi.enabled,
            license: condensed_mi.license
          })
        )
      );
      toast.success(LL.MODULES_UPDATED());
    } catch (error) {
      toast.error(error instanceof Error ? error.message : String(error));
    } finally {
      await fetchModules();
      setNumChanges(0);
    }
  }, [modules, updateModules, LL, fetchModules]);

  const content = useMemo(() => {
    if (!modules) {
      return (
        <FormLoader onRetry={fetchModules} errorMessage={error?.message || ''} />
      );
    }

    if (modules.length === 0) {
      return (
        <Typography variant="body2" color="error">
          {LL.MODULES_NONE()}
        </Typography>
      );
    }

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
                    <Cell>
                      <ColorStatus status={mi.status} />
                    </Cell>
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
  }, [
    modules,
    fetchModules,
    error,
    modules_theme,
    editModuleItem,
    LL,
    numChanges,
    onCancel,
    saveModules
  ]);

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      {content}
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
