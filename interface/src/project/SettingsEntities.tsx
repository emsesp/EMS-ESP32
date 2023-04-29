import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import WarningIcon from '@mui/icons-material/Warning';
import { Button, Typography, Box } from '@mui/material';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';
import { useTheme } from '@table-library/react-table-library/theme';
import { useState, useEffect, useCallback } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';

import { toast } from 'react-toastify';

import SettingsEntitiesDialog from './SettingsEntitiesDialog';
import * as EMSESP from './api';
import { DeviceValueUOM_s } from './types';
import { entityItemValidation } from './validators';
import type { EntityItem } from './types';
import type { FC } from 'react';
import { ButtonRow, FormLoader, SectionContent, BlockNavigation } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import { extractErrorMessage } from 'utils';

const SettingsEntities: FC = () => {
  const { LL } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [entities, setEntities] = useState<EntityItem[]>();
  const [selectedEntityItem, setSelectedEntityItem] = useState<EntityItem>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  function hasEntityChanged(ei: EntityItem) {
    return (
      ei.id !== ei.o_id ||
      (ei?.name || '') !== (ei?.o_name || '') ||
      ei.device_id !== ei.o_device_id ||
      ei.type_id !== ei.o_type_id ||
      ei.offset !== ei.o_offset ||
      ei.uom !== ei.o_uom ||
      ei.factor !== ei.o_factor ||
      ei.value_type !== ei.o_value_type ||
      ei.writeable !== ei.o_writeable ||
      ei.deleted !== ei.o_deleted
    );
  }

  useEffect(() => {
    if (entities) {
      setNumChanges(entities ? entities.filter((ei) => hasEntityChanged(ei)).length : 0);
    }
  }, [entities]);

  const entity_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(60px, 1fr)) 80px 80px 80px 80px;
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      &:nth-of-type(1) {
        padding: 8px;
      }
      &:nth-of-type(2) {
        text-align: center;
      }
      &:nth-of-type(3) {
        text-align: center;
      }
      &:nth-of-type(4) {
        text-align: center;
      }
      &:nth-of-type(5) {
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

  const fetchEntities = useCallback(async () => {
    try {
      const response = await EMSESP.readEntities();
      setEntities(
        response.data.entities.map((ei) => ({
          ...ei,
          o_id: ei.id,
          o_device_id: ei.device_id,
          o_type_id: ei.type_id,
          o_offset: ei.offset,
          o_factor: ei.factor,
          o_uom: ei.uom,
          o_value_type: ei.value_type,
          o_name: ei.name,
          o_writeable: ei.writeable,
          o_deleted: ei.deleted
        }))
      );
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  useEffect(() => {
    void fetchEntities();
  }, [fetchEntities]);

  const saveEntities = async () => {
    if (entities) {
      try {
        const response = await EMSESP.writeEntities({
          entities: entities
            .filter((ei) => !ei.deleted)
            .map((condensed_ei) => ({
              id: condensed_ei.id,
              name: condensed_ei.name,
              device_id: condensed_ei.device_id,
              type_id: condensed_ei.type_id,
              offset: condensed_ei.offset,
              factor: condensed_ei.factor,
              uom: condensed_ei.uom,
              writeable: condensed_ei.writeable,
              value_type: condensed_ei.value_type
            }))
        });

        if (response.status === 200) {
          toast.success(LL.ENTITIES_UPDATED());
        } else {
          toast.error(LL.PROBLEM_UPDATING());
        }
        await fetchEntities();
      } catch (error) {
        toast.error(extractErrorMessage(error, LL.PROBLEM_UPDATING()));
      }
    }
  };

  const editEntityItem = useCallback((ei: EntityItem) => {
    setCreating(false);
    setSelectedEntityItem(ei);
    setDialogOpen(true);
  }, []);

  const onDialogClose = () => {
    setDialogOpen(false);
  };

  const onDialogSave = (updatedItem: EntityItem) => {
    setDialogOpen(false);
    if (entities && creating) {
      setEntities([...entities.filter((ei) => creating || ei.o_id !== updatedItem.o_id), updatedItem]);
    } else {
      setEntities(entities?.map((ei) => (ei.id === updatedItem.id ? { ...ei, ...updatedItem } : ei)));
    }
  };

  const addEntityItem = () => {
    setCreating(true);
    setSelectedEntityItem({
      id: Math.floor(Math.random() * (Math.floor(200) - 100) + 100),
      name: '',
      device_id: '',
      type_id: '',
      offset: 0,
      factor: 1,
      uom: 0,
      value_type: 0,
      writeable: false,
      deleted: false
    });
    setDialogOpen(true);
  };

  function formatValue(value: any, uom: any) {
    return value === undefined || uom === undefined
      ? ''
      : new Intl.NumberFormat().format(value) + (uom === 0 ? '' : ' ' + DeviceValueUOM_s[uom]);
  }

  function showHex(value: number, digit: number) {
    return digit === 4
      ? '0x' + ('000' + value.toString(16).toUpperCase()).slice(-4)
      : '0x' + ('0' + value.toString(16).toUpperCase()).slice(-2);
  }

  const renderEntity = () => {
    if (!entities) {
      return <FormLoader errorMessage={errorMessage} />;
    }

    return (
      <Table data={{ nodes: entities.filter((ei) => !ei.deleted) }} theme={entity_theme} layout={{ custom: true }}>
        {(tableList: any) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell>{LL.NAME(0)}</HeaderCell>
                <HeaderCell stiff>{LL.ID_OF(LL.DEVICE())}</HeaderCell>
                <HeaderCell stiff>{LL.ID_OF(LL.TYPE(1))}</HeaderCell>
                <HeaderCell stiff>Offset</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((ei: EntityItem) => (
                <Row key={ei.name} item={ei} onClick={() => editEntityItem(ei)}>
                  <Cell>{ei.name}</Cell>
                  <Cell>{showHex(ei.device_id as number, 2)}</Cell>
                  <Cell>{showHex(ei.type_id as number, 4)}</Cell>
                  <Cell>{ei.offset}</Cell>
                  <Cell>{formatValue(ei.value, ei.uom)}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    );
  };

  return (
    <SectionContent title={LL.CUSTOM_ENTITIES(0)} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.ENTITIES_HELP_1()}</Typography>
      </Box>

      {renderEntity()}

      {selectedEntityItem && (
        <SettingsEntitiesDialog
          open={dialogOpen}
          creating={creating}
          onClose={onDialogClose}
          onSave={onDialogSave}
          selectedItem={selectedEntityItem}
          validator={entityItemValidation()}
        />
      )}

      <Box display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges > 0 && (
            <ButtonRow>
              <Button startIcon={<CancelIcon />} variant="outlined" onClick={fetchEntities} color="secondary">
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={saveEntities}
              >
                {LL.APPLY_CHANGES(numChanges)}
              </Button>
            </ButtonRow>
          )}
        </Box>
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button startIcon={<AddIcon />} variant="outlined" color="secondary" onClick={addEntityItem}>
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default SettingsEntities;
