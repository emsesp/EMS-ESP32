import { useCallback, useMemo, useState } from 'react';
import { useBlocker } from 'react-router';
import { toast } from 'react-toastify';

import AddIcon from '@mui/icons-material/Add';
import CancelIcon from '@mui/icons-material/Cancel';
import EditOutlinedIcon from '@mui/icons-material/EditOutlined';
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
import { useInterval } from 'utils';

import { readCustomEntities, writeCustomEntities } from '../../api/app';
import SettingsCustomEntitiesDialog from './CustomEntitiesDialog';
import { DeviceValueTypeNames, DeviceValueUOM_s } from './types';
import type { Entities, EntityItem } from './types';
import { entityItemValidation } from './validators';

const MIN_ID = -100;
const MAX_ID = 100;
const ICON_SIZE = 12;

const CustomEntities = () => {
  const { LL } = useI18nContext();
  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);
  const [selectedEntityItem, setSelectedEntityItem] = useState<EntityItem>();
  const [creating, setCreating] = useState<boolean>(false);
  const [dialogOpen, setDialogOpen] = useState<boolean>(false);

  useLayoutTitle(LL.CUSTOM_ENTITIES(0));

  const {
    data: entities,
    send: fetchEntities,
    error
  } = useRequest(readCustomEntities, {
    initialData: []
  });

  const intervalCallback = useCallback(() => {
    if (!dialogOpen && !numChanges) {
      void fetchEntities();
    }
  }, [dialogOpen, numChanges, fetchEntities]);

  useInterval(intervalCallback);

  const { send: writeEntities } = useRequest(
    (data: Entities) => writeCustomEntities(data),
    { immediate: false }
  );

  const hasEntityChanged = useCallback((ei: EntityItem) => {
    return (
      ei.id !== ei.o_id ||
      ei.ram !== ei.o_ram ||
      (ei?.name || '') !== (ei?.o_name || '') ||
      ei.device_id !== ei.o_device_id ||
      ei.type_id !== ei.o_type_id ||
      ei.offset !== ei.o_offset ||
      ei.uom !== ei.o_uom ||
      ei.factor !== ei.o_factor ||
      ei.value_type !== ei.o_value_type ||
      ei.writeable !== ei.o_writeable ||
      ei.hide !== ei.o_hide ||
      ei.deleted !== ei.o_deleted ||
      (ei.value || '') !== (ei.o_value || '')
    );
  }, []);

  const entity_theme = useMemo(
    () =>
      useTheme({
        Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(60px, 1fr)) minmax(80px, auto) 80px 80px 80px 120px;
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
      &:nth-of-type(6) {
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
        border-bottom: 1px solid #565656;
      }
      &:hover .td {
        background-color: #177ac9;
      }
    `
      }),
    []
  );

  const saveEntities = useCallback(async () => {
    await writeEntities({
      entities: entities
        .filter((ei: EntityItem) => !ei.deleted)
        .map((condensed_ei: EntityItem) => ({
          id: condensed_ei.id,
          ram: condensed_ei.ram,
          name: condensed_ei.name,
          device_id: condensed_ei.device_id,
          type_id: condensed_ei.type_id,
          offset: condensed_ei.offset,
          factor: condensed_ei.factor,
          uom: condensed_ei.uom,
          writeable: condensed_ei.writeable,
          hide: condensed_ei.hide,
          value_type: condensed_ei.value_type,
          value: condensed_ei.value
        }))
    })
      .then(() => {
        toast.success(LL.ENTITIES_UPDATED());
      })
      .catch((error: Error) => {
        toast.error(error.message);
      })
      .finally(async () => {
        await fetchEntities();
        setNumChanges(0);
      });
  }, [entities, writeEntities, LL, fetchEntities]);

  const editEntityItem = useCallback((ei: EntityItem) => {
    setCreating(false);
    setSelectedEntityItem(ei);
    setDialogOpen(true);
  }, []);

  const onDialogClose = useCallback(() => {
    setDialogOpen(false);
  }, []);

  const onDialogCancel = useCallback(async () => {
    await fetchEntities().then(() => {
      setNumChanges(0);
    });
  }, [fetchEntities]);

  const onDialogSave = useCallback(
    (updatedItem: EntityItem) => {
      setDialogOpen(false);
      void updateState(readCustomEntities(), (data: EntityItem[]) => {
        const new_data = creating
          ? [
              ...data.filter((ei) => creating || ei.o_id !== updatedItem.o_id),
              updatedItem
            ]
          : data.map((ei) =>
              ei.id === updatedItem.id ? { ...ei, ...updatedItem } : ei
            );
        setNumChanges(new_data.filter((ei) => hasEntityChanged(ei)).length);
        return new_data;
      });
    },
    [creating, hasEntityChanged]
  );

  const onDialogDup = useCallback((item: EntityItem) => {
    setCreating(true);
    setSelectedEntityItem({
      id: Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID),
      name: item.name + '_',
      ram: item.ram,
      device_id: item.device_id,
      type_id: item.type_id,
      offset: item.offset,
      factor: item.factor,
      uom: item.uom,
      value_type: item.value_type,
      writeable: item.writeable,
      deleted: false,
      hide: item.hide,
      value: item.value
    });
    setDialogOpen(true);
  }, []);

  const addEntityItem = useCallback(() => {
    setCreating(true);
    setSelectedEntityItem({
      id: Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID),
      name: '',
      ram: 0,
      device_id: '0',
      type_id: '0',
      offset: 0,
      factor: 1,
      uom: 0,
      value_type: 0,
      writeable: false,
      deleted: false,
      hide: false,
      value: ''
    });
    setDialogOpen(true);
  }, []);

  const formatValue = useCallback((value: unknown, uom: number) => {
    return value === undefined
      ? ''
      : typeof value === 'number'
        ? new Intl.NumberFormat().format(value) +
          (uom === 0 ? '' : ` ${DeviceValueUOM_s[uom]}`)
        : `${value as string}${uom === 0 ? '' : ` ${DeviceValueUOM_s[uom]}`}`;
  }, []);

  const showHex = useCallback((value: number, digit: number) => {
    return `0x${value.toString(16).toUpperCase().padStart(digit, '0')}`;
  }, []);

  const filteredAndSortedEntities = useMemo(
    () =>
      entities
        ?.filter((ei: EntityItem) => !ei.deleted)
        .sort((a: EntityItem, b: EntityItem) => a.name.localeCompare(b.name)) ?? [],
    [entities]
  );

  const renderEntity = useCallback(() => {
    if (!entities) {
      return (
        <FormLoader onRetry={fetchEntities} errorMessage={error?.message || ''} />
      );
    }

    return (
      <Table
        data={{
          nodes: filteredAndSortedEntities
        }}
        theme={entity_theme}
        layout={{ custom: true }}
      >
        {(tableList: EntityItem[]) => (
          <>
            <Header>
              <HeaderRow>
                <HeaderCell>{LL.NAME(0)}</HeaderCell>
                <HeaderCell stiff>{LL.ID_OF(LL.DEVICE())}</HeaderCell>
                <HeaderCell stiff>{LL.ID_OF(LL.TYPE(1))}</HeaderCell>
                <HeaderCell stiff>{LL.OFFSET()}</HeaderCell>
                <HeaderCell stiff>{LL.TYPE(0)}</HeaderCell>
                <HeaderCell stiff>{LL.VALUE(0)}</HeaderCell>
              </HeaderRow>
            </Header>
            <Body>
              {tableList.map((ei: EntityItem) => (
                <Row key={ei.name} item={ei} onClick={() => editEntityItem(ei)}>
                  <Cell>
                    {ei.name}&nbsp;
                    {ei.writeable && (
                      <EditOutlinedIcon
                        color="primary"
                        sx={{ fontSize: ICON_SIZE }}
                      />
                    )}
                  </Cell>
                  <Cell>
                    {ei.ram === 1 ? '' : showHex(ei.device_id as number, 2)}
                  </Cell>
                  <Cell>{ei.ram === 1 ? '' : showHex(ei.type_id as number, 3)}</Cell>
                  <Cell>{ei.ram === 1 ? '' : ei.offset}</Cell>
                  <Cell>
                    {ei.ram === 1 ? 'RAM' : DeviceValueTypeNames[ei.value_type]}
                  </Cell>
                  <Cell>{formatValue(ei.value, ei.uom)}</Cell>
                </Row>
              ))}
            </Body>
          </>
        )}
      </Table>
    );
  }, [
    entities,
    error,
    fetchEntities,
    entity_theme,
    editEntityItem,
    LL,
    filteredAndSortedEntities,
    showHex,
    formatValue
  ]);

  return (
    <SectionContent>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Box mb={2} color="warning.main">
        <Typography variant="body1">{LL.ENTITIES_HELP_1()}.</Typography>
      </Box>

      {renderEntity()}

      {selectedEntityItem && (
        <SettingsCustomEntitiesDialog
          open={dialogOpen}
          creating={creating}
          onClose={onDialogClose}
          onSave={onDialogSave}
          onDup={onDialogDup}
          selectedItem={selectedEntityItem}
          validator={entityItemValidation(entities, selectedEntityItem)}
        />
      )}

      <Box mt={2} display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges > 0 && (
            <ButtonRow>
              <Button
                startIcon={<CancelIcon />}
                variant="outlined"
                onClick={onDialogCancel}
                color="secondary"
              >
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
          <Button
            startIcon={<AddIcon />}
            variant="outlined"
            color="primary"
            onClick={addEntityItem}
          >
            {LL.ADD(0)}
          </Button>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default CustomEntities;
