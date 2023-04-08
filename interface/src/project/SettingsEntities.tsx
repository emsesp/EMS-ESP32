import { FC, useState, useEffect, useCallback } from 'react';
import { unstable_useBlocker as useBlocker } from 'react-router-dom';

import {
  Button,
  Typography,
  Box,
  Grid,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  TextField,
  MenuItem,
  InputAdornment
} from '@mui/material';

import { useTheme } from '@table-library/react-table-library/theme';
import { Table, Header, HeaderRow, HeaderCell, Body, Row, Cell } from '@table-library/react-table-library/table';

import { toast } from 'react-toastify';

import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';
import WarningIcon from '@mui/icons-material/Warning';
import CancelIcon from '@mui/icons-material/Cancel';
import DoneIcon from '@mui/icons-material/Done';
import AddIcon from '@mui/icons-material/Add';

import { ValidatedTextField, ButtonRow, FormLoader, SectionContent, BlockNavigation } from 'components';

import { DeviceValueUOM_s, EntityItem } from './types';
import { extractErrorMessage, updateValue } from 'utils';

import { validate } from 'validators';
import { entityItemValidation } from './validators';

import { useI18nContext } from 'i18n/i18n-react';

import { ValidateFieldsError } from 'async-validator';

import * as EMSESP from './api';

const SettingsEntities: FC = () => {
  const { LL, locale } = useI18nContext();

  const [numChanges, setNumChanges] = useState<number>(0);
  const blocker = useBlocker(numChanges !== 0);

  const emptyEntity = {
    device_id: 8,
    type_id: 2,
    offset: 0,
    factor: 1,
    uom: 0,
    val_type: 2,
    name: 'name',
    deleted: false
  };
  const [entity, setEntity] = useState<EntityItem[]>([emptyEntity]);
  const [entityItem, setEntityItem] = useState<EntityItem>();
  const [errorMessage, setErrorMessage] = useState<string>();
  const [creating, setCreating] = useState<boolean>(false);

  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  useEffect(() => {
    setNumChanges(getNumChanges());
  });

  const entity_theme = useTheme({
    Table: `
      --data-table-library_grid-template-columns: repeat(1, minmax(60px, 1fr)) minmax(60px, auto) minmax(60px, auto) minmax(60px, auto) minmax(60px, auto);
    `,
    BaseRow: `
      font-size: 14px;
      .td {
        height: 32px;
      }
    `,
    BaseCell: `
      padding-left: 10px;
      padding-right: 10px;
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
        text-align: right;
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
      setOriginalEntity(response.data.entity);
    } catch (error) {
      setErrorMessage(extractErrorMessage(error, LL.PROBLEM_LOADING()));
    }
  }, [LL]);

  useEffect(() => {
    fetchEntities();
  }, [fetchEntities]);

  const setOriginalEntity = (data: EntityItem[]) => {
    setEntity(
      data.map((ei) => ({
        ...ei,
        o_device_id: ei.device_id,
        o_type_id: ei.type_id,
        o_offset: ei.offset,
        o_factor: ei.factor,
        o_uom: ei.uom,
        o_val_type: ei.val_type,
        o_name: ei.name,
        o_deleted: ei.deleted
      }))
    );
  };

  function hasEntityChanged(ei: EntityItem) {
    return (
      ei.device_id !== ei.o_device_id ||
      ei.type_id !== ei.o_type_id ||
      ei.name !== ei.o_name ||
      ei.offset !== ei.o_offset ||
      ei.uom !== ei.o_uom ||
      ei.factor !== ei.o_factor ||
      ei.val_type !== ei.o_val_type ||
      ei.deleted !== ei.o_deleted
    );
  }

  const getNumChanges = () => {
    if (!entity) {
      return 0;
    }
    return entity.filter((ei) => hasEntityChanged(ei)).length;
  };

  const saveEntity = async () => {
    if (entity) {
      try {
        const response = await EMSESP.writeEntities({
          entity: entity
            .filter((ei) => !ei.deleted)
            .map((condensed_ei) => {
              return {
                device_id: condensed_ei.device_id,
                type_id: condensed_ei.type_id,
                offset: condensed_ei.offset,
                factor: condensed_ei.factor,
                val_type: condensed_ei.val_type,
                uom: condensed_ei.uom,
                name: condensed_ei.name
              };
            })
        });
        if (response.status === 200) {
          toast.success(LL.ENTITIES_SAVED());
        } else {
          toast.error(LL.PROBLEM_UPDATING());
        }
      } catch (error) {
        toast.error(extractErrorMessage(error, LL.PROBLEM_UPDATING()));
      }
      setOriginalEntity(entity);
    }
  };

  const editEntityItem = (ei: EntityItem) => {
    setCreating(false);
    setEntityItem(ei);
  };

  const addEntityItem = () => {
    setCreating(true);
    setEntityItem({
      device_id: 8,
      type_id: 2,
      offset: 0,
      factor: 1,
      val_type: 2,
      uom: 0,
      name: '',
      deleted: false
    });
  };

  const updateEntityItem = () => {
    if (entityItem) {
      setEntity([...entity.filter((ei) => creating || ei.o_name !== entityItem.o_name), entityItem]);
    }
    setEntityItem(undefined);
  };

  function formatValue(value: any, uom: number) {
    if (value === undefined) {
      return '';
    }
    if (uom === 0) {
      return new Intl.NumberFormat().format(value);
    }
    return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
  }

  function showHex(value: string, digit: number) {
    if (digit === 4) {
      return '0x' + ('000' + value).slice(-4);
    }
    return '0x' + ('0' + value).slice(-2);
  }

  const renderEntity = () => {
    if (!entity) {
      return <FormLoader errorMessage={errorMessage} />;
    }

    return (
      <Table
        data={{ nodes: entity.filter((ei) => !ei.deleted).sort((a, b) => a.name.localeCompare(b.time)) }}
        theme={entity_theme}
        layout={{ custom: true }}
      >
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
                  <Cell>{showHex(ei.device_id, 2)}</Cell>
                  <Cell>{showHex(ei.type_id, 4)}</Cell>
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

  const removeEntityItem = (ei: EntityItem) => {
    ei.deleted = true;
    setEntityItem(ei);
    updateEntityItem();
  };

  const validateEntityItem = async () => {
    if (entityItem) {
      try {
        setFieldErrors(undefined);
        await validate(entityItemValidation(entity, entityItem), entityItem);
        updateEntityItem();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  const closeDialog = () => {
    setEntityItem(undefined);
    setFieldErrors(undefined);
  };

  const renderEditEntity = () => {
    if (entityItem) {
      return (
        <Dialog open={!!entityItem} onClose={() => closeDialog()}>
          <DialogTitle>
            {creating ? LL.ADD(1) + ' ' + LL.NEW(1) : LL.EDIT()}&nbsp;{LL.CUSTOM_ENTITIES(1)}
          </DialogTitle>
          <DialogContent dividers>
            <Box display="flex" flexWrap="wrap" mb={1}>
              <Box flexGrow={1}></Box>
              <Box flexWrap="nowrap" whiteSpace="nowrap"></Box>
            </Box>
            <Grid container spacing={2}>
              <Grid item xs={12}>
                <ValidatedTextField
                  fieldErrors={fieldErrors}
                  name="name"
                  label={LL.NAME(0)}
                  value={entityItem.name}
                  margin="normal"
                  fullWidth
                  onChange={updateValue(setEntityItem)}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="device_id"
                  label={LL.ID_OF(LL.DEVICE())}
                  margin="normal"
                  fullWidth
                  value={entityItem.device_id}
                  onChange={updateValue(setEntityItem)}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">0x</InputAdornment>
                  }}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="type_id"
                  label={LL.ID_OF(LL.TYPE(1))}
                  margin="normal"
                  fullWidth
                  value={entityItem.type_id}
                  onChange={updateValue(setEntityItem)}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">0x</InputAdornment>
                  }}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="offset"
                  label="Offset"
                  margin="normal"
                  fullWidth
                  type="number"
                  value={entityItem.offset}
                  onChange={updateValue(setEntityItem)}
                />
              </Grid>
              <Grid item xs={4}>
                <ValidatedTextField
                  name="val_type"
                  label="Value Type"
                  value={entityItem.val_type}
                  variant="outlined"
                  onChange={updateValue(setEntityItem)}
                  margin="normal"
                  fullWidth
                  select
                >
                  <MenuItem value={0}>BOOL</MenuItem>
                  <MenuItem value={1}>INT</MenuItem>
                  <MenuItem value={2}>UINT</MenuItem>
                  <MenuItem value={3}>SHORT</MenuItem>
                  <MenuItem value={4}>USHORT</MenuItem>
                  <MenuItem value={5}>ULONG</MenuItem>
                  <MenuItem value={6}>TIME</MenuItem>
                </ValidatedTextField>
              </Grid>
              {entityItem.val_type !== 0 && (
                <>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="factor"
                      label={LL.FACTOR()}
                      value={entityItem.factor}
                      variant="outlined"
                      onChange={updateValue(setEntityItem)}
                      fullWidth
                      margin="normal"
                      type="number"
                      inputProps={{ step: '0.001' }}
                    />
                  </Grid>
                  <Grid item xs={4}>
                    <ValidatedTextField
                      name="uom"
                      label={LL.UNIT()}
                      value={entityItem.uom}
                      margin="normal"
                      fullWidth
                      onChange={updateValue(setEntityItem)}
                      select
                    >
                      {DeviceValueUOM_s.map((val, i) => (
                        <MenuItem key={i} value={i}>
                          {val}
                        </MenuItem>
                      ))}
                    </ValidatedTextField>
                  </Grid>
                </>
              )}
            </Grid>
          </DialogContent>
          <DialogActions>
            {!creating && (
              <Box flexGrow={1}>
                <Button
                  startIcon={<RemoveIcon />}
                  variant="outlined"
                  color="error"
                  onClick={() => removeEntityItem(entityItem)}
                >
                  {LL.REMOVE()}
                </Button>
              </Box>
            )}
            <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => closeDialog()} color="secondary">
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={creating ? <AddIcon /> : <DoneIcon />}
              variant="outlined"
              type="submit"
              onClick={() => validateEntityItem()}
              color="primary"
            >
              {creating ? LL.ADD(0) : LL.UPDATE()}
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  return (
    <SectionContent title={LL.CUSTOM_ENTITIES(0)} titleGutter>
      {blocker ? <BlockNavigation blocker={blocker} /> : null}
      <Box mb={2} color="warning.main">
        <Typography variant="body2">{LL.ENTITIES_HELP_1()}</Typography>
      </Box>
      {renderEntity()}
      {renderEditEntity()}
      <Box display="flex" flexWrap="wrap">
        <Box flexGrow={1}>
          {numChanges !== 0 && (
            <ButtonRow>
              <Button startIcon={<CancelIcon />} variant="outlined" onClick={() => fetchEntities()} color="secondary">
                {LL.CANCEL()}
              </Button>
              <Button
                startIcon={<WarningIcon color="warning" />}
                variant="contained"
                color="info"
                onClick={() => saveEntity()}
              >
                {LL.APPLY_CHANGES(numChanges)}
              </Button>
            </ButtonRow>
          )}
        </Box>
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button startIcon={<AddIcon />} variant="outlined" color="secondary" onClick={() => addEntityItem()}>
              {LL.ADD(0)}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </SectionContent>
  );
};

export default SettingsEntities;
