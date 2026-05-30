# STM32F407G Custom Bootloader

A custom bootloader for the STM32F407G-DISC1 (Discovery) board with a Python-based In-Application Programming (IAP) host application. The host communicates with the bootloader over USB CDC (virtual COM port) to flash, erase, and manage the application firmware without a hardware debugger.

## Repository Structure

```
stm32-custom-bootloader/
├── bootloader/          # Bootloader firmware (STM32CubeIDE/CMake project)
├── application/         # Example user application (STM32CubeIDE/CMake project)
└── host_app/
    └── host_app.py      # Python IAP host application
```

## Memory Layout

The STM32F407 has 1 MB of internal flash divided into 12 sectors. This project uses the following layout:

| Region      | Address                    | Size   | Flash Sectors |
|-------------|----------------------------|--------|---------------|
| Bootloader  | `0x0800_0000`              | 48 KB  | Sectors 0–2   |
| Application | `0x0800_C000` (Sector 3)   | 976 KB | Sectors 3–11  |

The bootloader is linked to start at `0x0800_0000`. The user application linker script places its code starting at `0x0800_C000`, which is where the bootloader expects to find the application's vector table.

## How It Works

### Boot Decision

On startup the bootloader checks the state of the user button (B1, PA0):

- **Button pressed** → enters bootloader mode and waits for USB commands from the host.
- **Button not pressed** → reads the MSP and reset handler address from the start of Sector 3 (`0x0800_C000`), tears down all peripherals, relocates the VTOR, and jumps to the user application.

### Communication Protocol

The bootloader presents itself as a USB CDC device. All commands follow this frame format:

```
[ length (1B) | command (1B) | payload (variable) | CRC32 (4B) ]
```

The `length` field counts everything except the 4-byte CRC. The bootloader verifies the CRC32 (polynomial `0x04C11DB7`) using the hardware CRC unit before executing any command, responding with `ACK (0xA5)` on success or `NACK (0x7F)` on failure.

Debug/trace output is sent over USART2 at 115200 baud.

### Supported Commands

| Command                    | Code   | Description                                              |
|----------------------------|--------|----------------------------------------------------------|
| `BL_GET_VER`               | `0x51` | Returns bootloader version                               |
| `BL_GET_HELP`              | `0x52` | Returns list of supported command codes                  |
| `BL_GET_CID`               | `0x53` | Returns the MCU chip ID from `DBGMCU->IDCODE`            |
| `BL_GET_RDP_STATUS`        | `0x54` | Returns the current Read Protection (RDP) level          |
| `BL_GO_TO_ADDR`            | `0x55` | Validates and jumps to an arbitrary address               |
| `BL_FLASH_ERASE`           | `0x56` | Erases one or more flash sectors; `0xFF` = mass erase    |
| `BL_MEM_WRITE`             | `0x57` | Writes a payload chunk to a validated memory address     |
| `BL_EN_RW_PROTECT`         | `0x58` | Enables write protection on selected sectors (nWRP bits) |
| `BL_MEM_READ`              | `0x59` | Memory read (stub — not yet implemented)                 |
| `BL_READ_SECTOR_P_STATUS`  | `0x5A` | Returns the 12-bit nWRP field from option bytes          |
| `BL_OTP_READ`              | `0x5B` | OTP read (stub — not yet implemented)                    |
| `BL_DIS_R_W_PROTECT`       | `0x5C` | Disables write protection on selected sectors            |

### Flash Write Flow (IAP)

The `BL_MEM_WRITE` command is used to flash application binaries. The Python host reads a `.bin` file and streams it in 48-byte chunks, incrementing the destination address with each chunk:

1. Erase target sectors with `BL_FLASH_ERASE`.
2. Send binary chunks with `BL_MEM_WRITE` starting at `0x0800_C000`.
3. Reset or use `BL_GO_TO_ADDR` to boot into the new application.

## Hardware Requirements

- STM32F407G-DISC1 Discovery board
- USB Micro-B cable (for CDC communication with the host)
- ST-Link or SWD programmer (for initial bootloader flashing only)
- Optionally: USART2 connection for debug trace output (PA2/PA3, 115200 8N1)

## Building the Firmware

Both projects use CMake with the `gcc-arm-none-eabi` toolchain.

**Bootloader:**
```bash
cd bootloader
cmake -B build/Debug -G Ninja
cmake --build build/Debug
```

**Application:**
```bash
cd application
cmake -B build/Debug -G Ninja
cmake --build build/Debug
```

The build system is configured to automatically generate a `.bin` file alongside the `.elf` after each build. Flash the bootloader to the board once using STM32CubeProgrammer or OpenOCD targeting `0x0800_0000`.

## Host Application (Python IAP)

### Requirements

```bash
pip install pyserial
```

### Usage

Update the serial port in `host_app.py` (default: `/dev/ttyACM1`) and run:

```bash
python3 host_app/host_app.py
```

The interactive menu drives all bootloader commands. To flash a new application binary:

1. Select **6** (mass erase) or **7** (sector erase) to clear Sectors 3+.
2. Select **8** (memory write), enter the path to `application.bin`, and enter address `0800C000`.
3. Reset the board without holding the button to boot into the application.

## Project Details

- **MCU:** STM32F407VGT6 (Cortex-M4, 168 MHz)
- **Bootloader interface:** USB Full-Speed OTG (CDC/ACM)
- **Debug output:** USART2, 115200 baud
- **Integrity check:** Hardware CRC32 (MPEG-2 polynomial `0x04C11DB7`)
- **HAL:** STM32Cube HAL / STM32CubeMX-generated project structure
- **Build system:** CMake + Ninja + `gcc-arm-none-eabi`
