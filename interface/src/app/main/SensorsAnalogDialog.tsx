import { Listbox, Message, TextField } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import EntityDialog from './EntityDialog';
import { AnalogType, AnalogTypeNames, DeviceValueUOM_s } from './types';
import type { AnalogSensor } from './types';
import { analogSensorItemValidation } from './validators';

interface SensorsAnalogDialogProps {
  item: AnalogSensor;
  creating: boolean;
  sensors: AnalogSensor[];
  gpios: number[];
  excludeTypes: number[];
  onClose: () => void;
  onSave: (as: AnalogSensor) => void;
  onRemove: () => void;
}

// GPIO 25 and 26 are the DAC outputs on the ESP32
const isDAC = (gpio: number) => gpio === 25 || gpio === 26;

const SensorsAnalogDialog = ({
  item,
  creating,
  sensors,
  gpios,
  excludeTypes,
  onClose,
  onSave,
  onRemove
}: SensorsAnalogDialogProps) => {
  const { LL } = useI18nContext();

  const gpioOptions = [...new Set([item.g, ...gpios])]
    .sort((a, b) => a - b)
    .map((g) => ({ value: String(g), label: String(g) }));

  const typeOptions = AnalogTypeNames.map((name, i) => ({
    value: String(i + 1),
    label: name,
    disabled: i + 1 !== item.t && excludeTypes.includes(i + 1)
  })).sort((a, b) => a.label.localeCompare(b.label));

  const uomOptions = DeviceValueUOM_s.map((u, i) => ({
    value: String(i),
    label: u
  }));

  const polarityOptions = (high: number, low: number) => [
    { value: String(high), label: LL.ACTIVEHIGH() },
    { value: String(low), label: LL.ACTIVELOW() }
  ];

  return (
    <EntityDialog
      title={creating ? LL.ADD_ANALOG_SENSOR() : LL.EDIT_ANALOG_SENSOR()}
      item={item}
      creating={creating}
      validator={() =>
        analogSensorItemValidation(sensors, creating ? undefined : item.n)
      }
      onClose={onClose}
      onSave={onSave}
      onRemove={creating || item.s ? undefined : onRemove}
    >
      {(f) => {
        const t = f.values.t;
        const sys = f.values.s;
        const isCounter =
          t === AnalogType.COUNTER ||
          (t >= AnalogType.CNT_0 && t <= AnalogType.CNT_2);
        const isFreq = t >= AnalogType.FREQ_0 && t <= AnalogType.FREQ_2;
        const isPWM = t >= AnalogType.PWM_0 && t <= AnalogType.PWM_2;
        const hasScale =
          isCounter ||
          isFreq ||
          t === AnalogType.RATE ||
          t === AnalogType.ADC ||
          t === AnalogType.TIMER;
        const isDigitalOut = t === AnalogType.DIGITAL_OUT;
        const dac = isDigitalOut && isDAC(f.values.g);

        return (
          <>
            <TextField label={LL.NAME(0)} fullWidth {...f.text('n')} />
            <Listbox
              label="GPIO"
              options={gpioOptions}
              disabled={sys || !creating}
              {...f.select('g')}
            />
            <Listbox
              label={LL.TYPE(0)}
              options={typeOptions}
              disabled={sys}
              {...f.select('t')}
              onChange={(v) => f.set({ t: Number(v), o: 0, f: 1, u: 0 })}
            />
            {hasScale && (
              <Listbox
                label={LL.UNIT()}
                options={uomOptions}
                disabled={sys}
                {...f.select('u')}
              />
            )}
            {t === AnalogType.DIGITAL_IN && (
              <Listbox
                label={LL.POLARITY()}
                options={polarityOptions(1, 0)}
                disabled={sys}
                {...f.select('f')}
              />
            )}
            {t === AnalogType.ADC && (
              <TextField
                label={LL.OFFSET()}
                unit="mV"
                min={0}
                max={3300}
                step={1}
                disabled={sys}
                {...f.number('o')}
              />
            )}
            {t === AnalogType.NTC && (
              <TextField
                label={LL.OFFSET()}
                unit="°C"
                min={-20}
                max={20}
                step={0.1}
                disabled={sys}
                {...f.number('o')}
              />
            )}
            {isCounter && (
              <TextField
                label={LL.STARTVALUE()}
                step={0.001}
                disabled={sys}
                {...f.number('o')}
              />
            )}
            {t === AnalogType.RGB && (
              <TextField
                label={`RGB ${LL.VALUE(0)}`}
                disabled={sys}
                {...f.number('o')}
              />
            )}
            {hasScale && (
              <TextField
                label={LL.FACTOR()}
                step={0.001}
                disabled={sys}
                {...f.number('f')}
              />
            )}
            {dac && (
              <TextField
                label={LL.VALUE(0)}
                min={0}
                max={255}
                step={1}
                disabled={sys}
                {...f.number('o')}
              />
            )}
            {isDigitalOut && !dac && (
              <>
                <Listbox
                  label={LL.VALUE(0)}
                  options={[
                    { value: '0', label: LL.OFF() },
                    { value: '1', label: LL.ON() }
                  ]}
                  disabled={sys}
                  {...f.select('o')}
                />
                <Listbox
                  label={LL.POLARITY()}
                  options={polarityOptions(1, -1)}
                  disabled={sys}
                  {...f.select('f')}
                />
                <Listbox
                  label={LL.STARTVALUE()}
                  options={[
                    { value: '0', label: LL.UNCHANGED() },
                    { value: '1', label: LL.ALWAYS_OFF() },
                    { value: '2', label: LL.ALWAYS_ON() }
                  ]}
                  disabled={sys}
                  {...f.select('u')}
                />
              </>
            )}
            {isPWM && (
              <>
                <TextField
                  label={LL.FREQ()}
                  unit="Hz"
                  min={1}
                  max={5000}
                  step={1}
                  disabled={sys}
                  {...f.number('f')}
                />
                <TextField
                  label={LL.DUTY_CYCLE()}
                  unit="%"
                  min={0}
                  max={100}
                  step={0.1}
                  disabled={sys}
                  {...f.number('o')}
                />
              </>
            )}
            {t === AnalogType.PULSE && (
              <>
                <Listbox
                  label={LL.POLARITY()}
                  options={polarityOptions(0, 1)}
                  disabled={sys}
                  {...f.select('o')}
                />
                <TextField
                  label={LL.PULSE()}
                  unit="s"
                  min={0}
                  max={10000}
                  step={0.1}
                  disabled={sys}
                  {...f.number('f')}
                />
              </>
            )}
            {sys && (
              <Message level="info" className="full">
                {LL.SYSTEM_SENSOR()}
              </Message>
            )}
          </>
        );
      }}
    </EntityDialog>
  );
};

export default SensorsAnalogDialog;
