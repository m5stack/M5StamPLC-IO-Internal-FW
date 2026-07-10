# M5StamPLC IO Internal FW

## Overview

This repository contains the internal IO-controller firmware for the M5Stack
[StamPLC](https://docs.m5stack.com/en/core/StamPLC) (SKU: `K141`). The firmware
runs on the STM32 coprocessor on the StamPLC IO board and handles low-level IO,
power switching, relay control, current and voltage monitoring, and
communication with the main controller.

The main controller communicates with the IO controller over I2C. It can read
the voltage and current of two INA226 monitoring channels, control two external
outputs and two channel power-enable signals, switch the relay outputs between
GPIO and PWM modes, configure INA226 sampling parameters, and query device
status and firmware information.

This repository only covers the STM32 IO-controller firmware and its I2C
bootloader. It does not contain the ESP32-S3 application firmware, UIFlow2
firmware, or user applications for StamPLC.

## Supported Hardware

| Product | SKU | Official Documentation |
|:--|:--|:--|
| M5Stack StamPLC | `K141` | [docs.m5stack.com/en/core/StamPLC](https://docs.m5stack.com/en/core/StamPLC) |

The application project targets `STM32G030F6Px`. The bootloader project is
configured for `STM32G031G8Ux`; use the project settings supplied in this
repository when rebuilding each image.

## Features

- I2C slave communication with DIP-switch-selectable 7-bit addresses from
  `0x20` to `0x2F`
- Two INA226 channels for bus-voltage and current monitoring
- Runtime configuration of INA226 averaging and conversion-time parameters
- Two external control outputs and two channel power-enable outputs
- Relay trigger output
- GPIO and PWM relay-control modes
- Configurable PWM frequency from `1 Hz` to `100 Hz`
- Independent PWM duty-cycle control for two outputs from `0` to `1000`
  per mille
- INA226 peripheral initialization error reporting
- Application and bootloader version readback
- I2C in-application upgrade support with application CRC verification
- Independent watchdog and I2C bus timeout recovery

## Repository Structure

```text
.
|-- code/
|   |-- app/StamPLCIO/                 # Main STM32 IO application
|   `-- bootloader/                    # I2C bootloader
|-- firmware/
|   `-- 2026-06-17-0x01/              # Released firmware V0x01
|-- protocol/
|   |-- StamPLC_IO-I2C-Protocol-CN-V1.0.pdf
|   `-- StamPLC_IO-I2C-Protocol-EN-V1.0.pdf
|-- LICENSE
`-- README.md
```

## Development Environment

[STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) is
recommended for opening, building, and debugging both projects.

| Component | Project Directory | MCU Configuration | CubeMX File |
|:--|:--|:--|:--|
| Application | `code/app/StamPLCIO` | `STM32G030F6Px` | `StamPLCIO.ioc` |
| Bootloader | `code/bootloader` | `STM32G031G8Ux` | `StamPLCIO_Bootloader.ioc` |

The STM32 HAL, LL, and CMSIS sources required by the projects are included in
the repository. No additional application library installation is required.

### Import and Build

1. Open STM32CubeIDE.
2. Select `File` -> `Import` -> `General` -> `Existing Projects into Workspace`.
3. Select either `code/app/StamPLCIO` or `code/bootloader` as the root directory.
4. Import the detected project.
5. Select the `Debug` or `Release` configuration and build the project.
6. Repeat the process for the other project if both images are required.

## Flash Layout

| Region | Start Address | Description |
|:--|:--|:--|
| Bootloader | `0x08000000` | I2C bootloader and startup code |
| Application | `0x08002000` | StamPLC IO application |
| Application CRC | `0x080077FC` | CRC value checked by the bootloader |

The application linker script is already configured to start at
`0x08002000`. Do not flash an application-only image at `0x08000000`, because
that would overwrite the bootloader.

## Released Firmware

The current release is application firmware version `0x01`, dated
`2026-06-17`:

```text
firmware/2026-06-17-0x01/
```

| File | Purpose |
|:--|:--|
| `A176-StampPLC-IO-STM32G030F6P6-TAO-V01-20260617-b03ab866_0x8000000.hex` | Complete production image starting at `0x08000000` |
| `bootloader_app_merge.bin` | Merged bootloader and application binary |
| `bootloader.h` | Bootloader image represented as a C byte array (`0x08000000` - `0x08001FFF`) |
| `app_without_flash.h` | Application byte array covering `0x08002000` - `0x080077FF` |
| `app_with_flash.h` | Application byte array padded through `0x08007FFF` |

### Flashing Notes

1. For full programming, erase the target Flash and program the release HEX at
   its encoded addresses, or program `bootloader_app_merge.bin` at
   `0x08000000`.
2. Reset or power-cycle the device after programming.
3. Confirm that the application responds at the I2C address selected by the
   four address switches.
4. Read register `0xFE` to verify application firmware version `0x01`.
5. Read register `0xFB` and confirm that the INA226 initialization error bits
   are clear.

## I2C Communication

The runtime I2C address is calculated as `0x20 + DIP switch value`. With all
four address switches OFF the address is `0x20`; with all four switches ON the
address is `0x2F`.

| Register Range | Access | Description |
|:--|:--|:--|
| `0x00` - `0x0B` | Read | Channel 1 and channel 2 voltage (mV) and current (uA) |
| `0x10` | Read/Write | External outputs, channel power enables, relay trigger, and relay mode |
| `0x20` - `0x23` | Read/Write | INA226 channel configuration |
| `0x30` - `0x34` | Read/Write | PWM frequency and two duty-cycle values |
| `0xFB` | Read | Peripheral initialization status |
| `0xFC` | Read | Bootloader version |
| `0xFD` | Write | Jump-to-bootloader command (`0x01`) |
| `0xFE` | Read | Application firmware version |
| `0xFF` | Read/Write | Active I2C address and address-refresh control |

Multi-byte measurement and configuration values use little-endian byte order.
After writing INA226 configuration registers, allow time for the setting to be
applied and read the registers back to verify the effective value.

For the complete register definitions, bit fields, valid ranges, and DIP-switch
address table, see:

- [StamPLC IO I2C Protocol - English](protocol/StamPLC_IO-I2C-Protocol-EN-V1.0.pdf)
- [StamPLC IO I2C Protocol - Chinese](protocol/StamPLC_IO-I2C-Protocol-CN-V1.0.pdf)

## Related Resources

- [StamPLC official documentation](https://docs.m5stack.com/en/core/StamPLC)
- [StamPLC IO board schematic](https://m5stack-doc.oss-cn-shenzhen.aliyuncs.com/1129/K141_sch_StamPLC_V10_IO.pdf)
- [M5StamPLC Arduino library](https://github.com/m5stack/M5StamPLC)
- [StamPLC factory firmware](https://github.com/m5stack/M5StamPLC-UserDemo)

## License

This project is licensed under the [MIT License](LICENSE).
