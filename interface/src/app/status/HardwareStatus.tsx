import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { boardProfileName } from 'app/main/types';
import { KeyValueList, Loader, Meter, Section } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import './Status.css';
import BBQKeesIcon from './bbqkees.svg';
import { formatDuration, formatTemperature, nf } from './status';

const HardwareStatus = () => {
  const { LL } = useI18nContext();
  const { data, send: loadData, error } = useRequest(readSystemStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const maxBlock = LL.MAX_BLOCK({ num: nf.format(data.max_alloc_heap) });

  return (
    <div className="grid2">
      <Section icon="developer_board" title={LL.DEVICE()}>
        <KeyValueList
          rows={[
            {
              label: LL.HARDWARE(),
              value: data.model ? (
                <>
                  <img className="st-bbq" alt="" src={BBQKeesIcon} />
                  {data.model}
                </>
              ) : (
                data.cpu_type
              )
            },
            !!data.board_profile && {
              label: LL.BOARD_PROFILE(),
              value:
                data.board_profile === 'CUSTOM'
                  ? LL.CUSTOM()
                  : boardProfileName(data.board_profile)
            },
            {
              label: 'CPU',
              value: `${data.esp_platform} / ${data.cpu_type} rev ${data.cpu_rev}`,
              extra: `${data.cpu_cores} × ${data.cpu_freq_mhz} MHz`
            },
            data.temperature !== undefined && {
              label: LL.TEMPERATURE(),
              value: formatTemperature(data.temperature)
            },
            {
              label: 'SDK',
              value: `${data.arduino_version} / ESP-IDF ${data.sdk_version}`
            }
          ]}
        />
      </Section>

      <Section icon="memory" title={LL.FREE_MEMORY()}>
        {data.heap_size !== undefined ? (
          <Meter
            label={LL.HEAP()}
            used={data.heap_size - data.free_heap}
            total={data.heap_size}
            unit="KB"
            note={maxBlock}
          />
        ) : (
          <KeyValueList
            rows={[
              {
                label: LL.HEAP(),
                value: `${nf.format(data.free_heap)} KB`,
                extra: maxBlock
              }
            ]}
          />
        )}
        {data.psram_size !== undefined && data.free_psram !== undefined && (
          <Meter
            label="PSRAM"
            used={data.psram_size - data.free_psram}
            total={data.psram_size}
            unit="KB"
          />
        )}
        <p className="cap st-help">{LL.MAX_BLOCK_HELP()}</p>
      </Section>

      <Section icon="sd_storage" title="Flash">
        <KeyValueList
          rows={[
            {
              label: LL.FLASH_CHIP(),
              value: `${nf.format(data.flash_chip_size)} KB`,
              extra: `${(data.flash_chip_speed / 1000000).toFixed(0)} MHz`
            }
          ]}
        />
        <Meter
          label={`${LL.EMSESP_APP()} (${data.partition})`}
          used={data.app_used}
          total={data.app_used + data.app_free}
          unit="KB"
        />
        <Meter
          label={LL.FILE_SYSTEM()}
          used={data.fs_used}
          total={data.fs_used + data.fs_free}
          unit="KB"
        />
      </Section>

      <Section icon="battery_charging_full" title={LL.POWER_AND_RESET()}>
        <KeyValueList
          rows={[
            !!data.reset_reason && {
              label: LL.LAST_RESTART(),
              value: data.reset_reason
            },
            { label: LL.UPTIME(), value: formatDuration(data.uptime, LL) }
          ]}
        />
      </Section>
    </div>
  );
};

export default HardwareStatus;
