# Decoding crash dumps

When EMS-ESP crashes (panic, `abort()`, stack overflow or a watchdog panic), the ESP32 writes a core dump to the `coredump` flash partition before it restarts. The dump survives the restart and is kept until the next crash overwrites it, or until it is erased.

Only boards whose partition table has a `coredump` partition store a dump. That's the 16MB and 32MB layouts, not the 4MB ones. A brownout, a power cut or a normal restart doesn't write a dump.

Every decode needs the `firmware.elf` from the exact build that crashed. Each build keeps a gzipped copy as `build/elf/<variant>-<sha>.elf.gz`, where `<sha>` is the first 9 hex characters of the ELF's SHA256. The crash summary reports the same `sha`, so pick the file with that name and unpack it with `gunzip -k`. They are about 20 MB each, so delete old ones now and then.

## 1. Quick: the summary from the API

After the restart, EMS-ESP reads the dump and logs `Crash dump found: ...` to the System Log. It also adds a `coredump` field to the `system` section of `/api/system/info`:

```text
task=async_tcp pc=0x400d1234 cause=28 vaddr=0x00000000 bt=0x400d1234 0x400d5678 0x400e9abc sha=1a2b3c4d5 reason=LoadProhibited
```

| Field    | Meaning                                                                    |
| -------- | -------------------------------------------------------------------------- |
| `task`   | FreeRTOS task that crashed                                                 |
| `pc`     | program counter where it crashed                                           |
| `cause`  | Xtensa exception cause, e.g. 28 = LoadProhibited, 29 = StoreProhibited     |
| `vaddr`  | memory address that was accessed, near 0 usually means a null pointer      |
| `bt`     | backtrace, the crashing function first and its callers after it            |
| `sha`    | start of the crashed firmware's ELF SHA256                                 |
| `reason` | panic reason text, e.g. the `abort()` or watchdog message                  |

On RISC-V chips (C3, C6) there is no backtrace on the device, only `ra` (return address), `mcause` and `mtval`.

Turn the addresses into function, file and line:

```bash
# ESP32, use xtensa-esp32s3-elf-addr2line for the S3 and xtensa-esp32s2-elf-addr2line for the S2
~/.platformio/packages/toolchain-xtensa-esp-elf/bin/xtensa-esp32-elf-addr2line -pfiaC \
  -e .pio/build/s_16M_P/firmware.elf 0x400d1234 0x400d5678 0x400e9abc

# C3/C6, pass the pc and ra
~/.platformio/packages/toolchain-riscv32-esp/bin/riscv32-esp-elf-addr2line -pfiaC \
  -e .pio/build/c3_mini_4M/firmware.elf 0x42001234 0x42005678
```

Erase the dump once you've noted it, so the next crash is clearly a new one. Use `call system coredump` in the console, or `POST /api/system/coredump` as an admin.

## 2. Full: every task's stack and registers

With the board on USB, `esp-coredump` reads the partition and decodes it in one step:

```bash
~/.platformio/penv/bin/esp-coredump --port /dev/ttyUSB0 info_corefile .pio/build/s_16M_P/firmware.elf
```

You can also save the partition first and decode it later:

```bash
# 16MB layout: 0xFF0000, 32MB layout: 0x1FF0000
~/.platformio/penv/bin/esptool.py --port /dev/ttyUSB0 read_flash 0xFF0000 0x10000 coredump.bin
~/.platformio/penv/bin/esp-coredump info_corefile -t raw -c coredump.bin .pio/build/s_16M_P/firmware.elf
```

This shows the crashed task's decoded backtrace with source lines, the registers, and the stacks of all the other tasks. Use `dbg_corefile` instead of `info_corefile` to open the dump in GDB.

Find the partition offset of other layouts with `python gen_esp32part.py .pio/build/<env>/partitions.bin`.

## Alternative: the serial monitor

The panic handler also prints the backtrace to the serial port. For a crash that's hard to reproduce, leave a monitor logging to a file. The log also shows what happened just before the crash:

```bash
pio device monitor -e s_16M_P -f esp32_exception_decoder -f log2file
```
