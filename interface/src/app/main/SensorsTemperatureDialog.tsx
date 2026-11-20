import { Message, TextField } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import EntityDialog from './EntityDialog';
import { formatValue } from './deviceValue';
import { DeviceValueUOM, DeviceValueUOM_s } from './types';
import type { TemperatureSensor } from './types';
import { temperatureSensorItemValidation } from './validators';

interface SensorsTemperatureDialogProps {
  item: TemperatureSensor;
  sensors: TemperatureSensor[];
  onClose: () => void;
  onSave: (ts: TemperatureSensor) => void;
}

const SensorsTemperatureDialog = ({
  item,
  sensors,
  onClose,
  onSave
}: SensorsTemperatureDialogProps) => {
  const { LL } = useI18nContext();
  // ±5 °C, the same span in °F when the system shows Fahrenheit
  const maxOffset = item.u === DeviceValueUOM.FAHRENHEIT ? 9 : 5;

  return (
    <EntityDialog
      title={LL.EDIT_TEMP_SENSOR()}
      item={item}
      validator={() => temperatureSensorItemValidation(sensors, item.n)}
      onClose={onClose}
      onSave={onSave}
    >
      {(f) => (
        <>
          <p className="cap full">
            {LL.SENSOR_ID()} <span className="mono lb">{item.id}</span>
            {item.t !== undefined && ` · ${formatValue(LL, item.t, item.u)}`}
          </p>
          <TextField label={LL.NAME(0)} fullWidth {...f.text('n')} />
          <TextField
            label={LL.OFFSET()}
            unit={DeviceValueUOM_s[item.u]}
            min={-maxOffset}
            max={maxOffset}
            step={0.1}
            {...f.number('o')}
          />
          {item.s && (
            <Message level="info" className="full">
              {LL.SYSTEM_SENSOR()}
            </Message>
          )}
        </>
      )}
    </EntityDialog>
  );
};

export default SensorsTemperatureDialog;
