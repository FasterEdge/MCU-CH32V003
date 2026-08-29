<div align="center">
<img src="https://avatars.githubusercontent.com/u/245985800?s=200&v=4" style="width:100px;" width="100"/>
<h2>FasterEdge MCU - CH32V003</h2>
<h3>FasterEdge framework on CH32V003 (RISC-V) (Arduino / PlatformIO / MounRiver editions)</h3>
</div>

### 1. Introduction

This repo implements the **[FasterEdge](https://github.com/FasterEdge/FasterEdge)** framework on the **CH32V003 (RISC-V RV32EC)**. The CH32V003 is a 32-bit RISC-V (RV32EC): 48MHz, 16KB Flash, 2KB RAM, no HW EEPROM (DataFlash emulation) — no network, no OS. Following the [MCU-C51](../MCU-C51) no-network design, the capability set is trimmed and 3 **MCU-specific** modules (registers / GPIO / chip info) are kept.

- ✅ **arduino/ (C++, WCH Arduino core)** + **platformio_ide/ (pure C register-level)** + **mounriver/ (official MounRiver-Studio project)** triple editions
- ✅ Same names & commands as the main repo — peer programming for edge/cloud
- ✅ HMAC-SHA256 in pure C (zero dependencies)
- ✅ Config/keys persisted to DataFlash (1KB user area)
- ✅ platformio_ide edition ships **RISC-V register-level drivers** (RCC / GPIO / USART1 / SysTick)

### 2. Implemented Capabilities (no-network subset)

**Abilities (8)**

| Name | Type | Commands |
|------|------|----------|
| `BaseAbility` | Base | `list_data_names` / `list_ability_names` |
| `RoleAbility` | Role | `describe` / `set_role` / `get_role` |
| `TimeAbility` | Time | `sync_manual` / `sync_system` / `get_time` / `configure_run` (no NTP) |
| `OneKeyAbility` | Token | `issue_token` / `verify_token` / `revoke_all` / `list_tokens` / `status` / `rotate` (HMAC-SHA256) |
| `SerialAbility` | Serial | `open` / `close` / `write` / `read` / `is_open` / `set_config` / `get_config` / `list_ports` |
| `ModbusAbility` | Modbus | `set_unit_id` / `get_unit_id` / `read_holding` / `read_input` / `read_coils` / `read_discrete` / `write_holding` / `write_coil` (RTU slave) |
| `RegAbility` | Reg (own) | `read <addr>` / `write <addr>,<value>` / `bit_set <addr>,<bit>` / `bit_clear <addr>,<bit>` / `info` |
| `GpioAbility` | GPIO (own) | `mode <pin>,<input|output|input_pullup>` / `write <pin>,<0|1>` / `read <pin>` / `info` |

**Data (3)**

| Name | Type | Commands |
|------|------|----------|
| `BaseData` | Meta | `logo` / `info` |
| `ConfigData` | KV config (EEPROM) | `get` / `set` / `delete` / `list` / `snapshot` |
| `ChipData` | Chip info (own) | `info` |

### 3. Excluded Capabilities

| Capability | Reason |
|------------|--------|
| MQTTAbility / NetMapData | No network stack on CH32V003 |
| EdgeRoleAbility | Needs network heartbeat |
| ConfigFileAbility | Redundant with ConfigData; no filesystem concept |
| KeyringData | Merged into OneKeyAbility (same EEPROM key) |
| TimeAbility.sync_ntp | No network for SNTP |

### 4. Directory Layout

```
MCU-CH32V003/
├── arduino/                    # Arduino C++ edition (WCH Arduino core)
│   ├── include/                # fe.h / fe_ability.h / fe_data.h / fe_hmac_sha256.h
│   ├── src/                    # fe.cpp / main.cpp / register.cpp / ability_*.cpp / data_*.cpp
│   └── platformio.ini          # board = ch32v003f4p6 (ch32v003 platform + arduino framework)
├── platformio_ide/             # VS Code + PlatformIO IDE project (pure C register-level)
│   ├── platformio.ini          # board = ch32v003f4p6 (ch32v003 platform, framework = none)
│   ├── include/                # fe.h / fe_ability.h / fe_data.h / fe_port.h / fe_hmac_sha256.h
│   └── src/                    # bare-metal C + register-level fe_port (RCC/GPIO/USART1/SysTick)
└── mounriver/                  # official MounRiver-Studio (Eclipse) project
    ├── .project / .cproject    # MRS project files
    ├── Src/                    # shares platformio_ide sources
    └── Inc/                    # shared headers
```

> All three editions expose identical capabilities & commands: `arduino/` quick start, `platformio_ide/` bare-metal register drivers, `mounriver/` official IDE debugging.

### 5. Usage

1. **arduino edition**: VS Code + PlatformIO open `arduino/` (or Arduino IDE with WCH core), board CH32V003, flash and monitor at 115200
2. **platformio_ide edition**: PlatformIO IDE open `platformio_ide/`, Build / Upload / Serial Monitor
3. **mounriver edition**: import `mounriver/` into MounRiver-Studio, Build / Download (WCH-Link)

**Serial command examples:**

```
help
ability_BaseAbility list_ability_names
ability_RoleAbility set_role edge
ability_TimeAbility sync_manual 1700000000
ability_OneKeyAbility issue_token sensor01
ability_ModbusAbility set_unit_id 3
ability_ModbusAbility write_holding 0,42
ability_ModbusAbility read_holding 0,4
ability_SerialAbility set_config 0,9600
ability_SerialAbility write hello
data_ConfigData set wifi.ssid=MyNet
data_ConfigData get wifi.ssid
data_BaseData info
```

### 6. Platform Differences

| Aspect | ESP32/ESP8266 | CH32V003 |
|--------|---------------|---------------------|
| Architecture | Xtensa 32-bit | **RISC-V RV32EC 32-bit** |
| RAM / Flash | KB~MB | **2KB SRAM / 16KB Flash** |
| Storage | NVS / Flash | **1KB DataFlash user area (emulated EEPROM)** |
| Network | Yes | **No** (network items trimmed) |
| Registers | 32-bit MMIO | **32-bit peripheral space 0x40000000+ (RegAbility width 32)** |

### 7. platformio_ide Notes (RISC-V register-level)

The `platformio_ide/` edition is pure-C bare metal; `fe_port.c` drives the CH32V003 peripherals directly:

| Function | Implementation |
|----------|----------------|
| UART | **USART1** (RCC clock + BRR baud + STATR/DATAR) |
| EEPROM | DataFlash reference (TODO interface reserved) |
| Time | **SysTick** (core timer 1ms) |
| GPIO | **GPIOA-D** (CFGLR/CFGHR/INDR/OUTDR/BSHR/BCR) |
| Random | LCG |

```bash
cd platformio_ide
pio run            # build
pio run -t upload  # flash
pio device monitor # serial monitor (115200)
```

### 8. MCU-Specific Modules

Beyond main-repo capabilities, 3 **MCU-specific** modules (registers / GPIO / chip info) are provided. The CH32V003 registers are **32-bit RISC-V peripheral space** (0x40000000+); GPIO uses Arduino pin numbers 0-19:

| Module | Type | Commands | Description |
|--------|------|----------|-------------|
| RegAbility | Ability | `read <addr>` / `write <addr>,<value>` / `bit_set <addr>,<bit>` / `bit_clear <addr>,<bit>` / `info` | RISC-V MMIO registers (32-bit, volatile pointer) |
| GpioAbility | Ability | `mode <pin>,<input|output|input_pullup>` / `write <pin>,<0|1>` / `read <pin>` / `info` | Arduino pin GPIO (pin 0-19, register-level) |
| ChipData | Data | `info` | CH32V003 model / RAM / Flash / EEPROM / freq |

**Examples:**

```
ability_RegAbility read 0x50000000      # read GPIOA config
ability_RegAbility write 0x50000018,0x01 # write GPIOA BSHR
ability_GpioAbility mode 13,output
ability_GpioAbility write 13,1
ability_GpioAbility read 2
data_ChipData info
```

> ⚠️ Register access touches hardware directly; a wrong write may crash the system. Debug/low-level use only.

### 9. Correspondence with the Main Repo

- Commands match the main repo exactly, and the implementation is isomorphic with MCU-C51 / MCU-ESP32.
- `Atom` model: singleton global Atom, `data_` / `ability_` prefix routing.
- Tokens via HMAC-SHA256 (pure C, no mbedTLS), key persisted in EEPROM.
- Modbus register tables live in RAM; RTU entry `modbus_slave_service()` is reserved.

### 10. Sibling Projects

- **[FasterEdge MCU - ESP32](https://github.com/FasterEdge/MCU-ESP32)**: dual-core, WiFi/BLE, more peripherals
- **[FasterEdge MCU - ESP8266](https://github.com/FasterEdge/MCU-ESP8266)**: WiFi, low power
- **[FasterEdge MCU - C51](https://github.com/FasterEdge/MCU-C51)**: 8-bit 8051, most minimal
- **[FasterEdge MCU - Arduino Uno R3](https://github.com/FasterEdge/MCU-Arduino-Uno-R3)**: 8-bit AVR (ATmega328P)
- **[FasterEdge MCU - Arduino Uno R4](https://github.com/FasterEdge/MCU-Arduino-Uno-R4)**: 32-bit Cortex-M4F (RA4M1)
- **[FasterEdge](https://github.com/FasterEdge/FasterEdge)**: framework main repo
