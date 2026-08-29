<div align="center">
<img src="https://avatars.githubusercontent.com/u/245985800?s=200&v=4" style="width:100px;" width="100"/>
<h2>FasterEdge MCU - CH32V003</h2>
<h3>FasterEdge 框架的 CH32V003（RISC-V） 平台实现（Arduino / PlatformIO / MounRiver 版）</h3>
</div>

### 一、简介

本项目是 **[FasterEdge](https://github.com/FasterEdge/FasterEdge)** 框架在 ****CH32V003（RISC-V RV32EC）****平台上的实现。CH32V003 为 32 位 RISC-V（RV32EC）内核：48MHz、16KB Flash、2KB RAM、无硬件 EEPROM（用户区 DataFlash 模拟），无网络、无操作系统，因此按 [MCU-C51](../MCU-C51) 的无网络精简思路裁剪能力子集，并保留 **寄存器 / GPIO / 芯片信息** 三个 MCU 专有模块。

- ✅ **arduino/（C++，WCH Arduino core）** + **platformio_ide/（纯 C 寄存器级）** + **mounriver/（官方 MounRiver-Studio 工程）** 三版本
- ✅ 与主仓库**同名同命令**，云边协同对等编程
- ✅ HMAC-SHA256 纯 C 零依赖
- ✅ 配置/密钥持久化到 DataFlash（1KB 用户区）
- ✅ platformio_ide 版为 **RISC-V 寄存器级实现**（RCC / GPIO / USART1 / SysTick）

### 二、已实现能力（无网络合理子集）

**Ability（8 个）**

| 名称 | 类别 | 命令 |
|------|------|------|
| `BaseAbility` | 基础 | `list_data_names` / `list_ability_names` |
| `RoleAbility` | 角色 | `describe` / `set_role` / `get_role` |
| `TimeAbility` | 时间 | `sync_manual` / `sync_system` / `get_time` / `configure_run`（无 NTP）|
| `OneKeyAbility` | 令牌 | `issue_token` / `verify_token` / `revoke_all` / `list_tokens` / `status` / `rotate`（HMAC-SHA256）|
| `SerialAbility` | 串口 | `open` / `close` / `write` / `read` / `is_open` / `set_config` / `get_config` / `list_ports` |
| `ModbusAbility` | Modbus | `set_unit_id` / `get_unit_id` / `read_holding` / `read_input` / `read_coils` / `read_discrete` / `write_holding` / `write_coil`（RTU 从站）|
| `RegAbility` | 寄存器(专有) | `read <addr>` / `write <addr>,<value>` / `bit_set <addr>,<bit>` / `bit_clear <addr>,<bit>` / `info` |
| `GpioAbility` | GPIO(专有) | `mode <pin>,<input\|output\|input_pullup>` / `write <pin>,<0\|1>` / `read <pin>` / `info` |

**Data（3 个）**

| 名称 | 功能 | 命令 |
|------|------|------|
| `BaseData` | 框架元信息 | `logo` / `info` |
| `ConfigData` | KV 配置（EEPROM 持久化）| `get` / `set` / `delete` / `list` / `snapshot` |
| `ChipData` | 芯片信息(专有) | `info` |

### 三、排除项与理由

| 能力 | 排除原因 |
|------|---------|
| MQTTAbility / NetMapData | CH32V003 无网络协议栈 |
| EdgeRoleAbility | 依赖网络心跳上报 |
| ConfigFileAbility | 与 ConfigData 重复，且无文件系统概念 |
| KeyringData | 与 OneKeyAbility 合并（同一 EEPROM 密钥存储）|
| TimeAbility.sync_ntp | 无网络无法 SNTP 校时 |

### 四、目录结构

```
MCU-CH32V003/
├── arduino/                    # Arduino C++ 版（WCH Arduino core）
│   ├── include/                # fe.h / fe_ability.h / fe_data.h / fe_hmac_sha256.h
│   ├── src/                    # fe.cpp / main.cpp / register.cpp / ability_*.cpp / data_*.cpp
│   └── platformio.ini          # board = ch32v003f4p6（ch32v003 平台 + arduino framework）
├── platformio_ide/             # VS Code + PlatformIO IDE 工程（纯 C 寄存器级驱动）
│   ├── platformio.ini          # board = ch32v003f4p6（ch32v003 平台，framework = none）
│   ├── include/                # fe.h / fe_ability.h / fe_data.h / fe_port.h / fe_hmac_sha256.h
│   └── src/                    # 裸机 C + 寄存器级 fe_port（RCC/GPIO/USART1/SysTick）
└── mounriver/                  # 官方 MounRiver-Studio（Eclipse）工程
    ├── .project / .cproject    # MRS 工程文件
    ├── Src/                    # 共享平台io_ide 源码
    └── Inc/                    # 共享头文件
```

> 三个版本能力与命令完全一致：`arduino/` 快速上手、`platformio_ide/` 裸机寄存器驱动、`mounriver/` 官方 IDE 调试。

### 五、使用说明

1. **arduino 版**：VS Code + PlatformIO 打开 `arduino/`（或 Arduino IDE 装 WCH core），板卡选 CH32V003，烧录后串口 115200
2. **platformio_ide 版**：PlatformIO IDE 打开 `platformio_ide/`，Build / Upload / Serial Monitor
3. **mounriver 版**：MounRiver-Studio 导入 `mounriver/` 工程，Build / Download（WCH-Link）

**串口命令示例：**

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

### 六、平台适配要点

| 差异点 | ESP32/ESP8266 | CH32V003 |
|--------|--------------|---------------------|
| 架构 | Xtensa 32 位 | **RISC-V RV32EC 32 位** |
| RAM / Flash | KB~MB | **2KB SRAM / 16KB Flash** |
| 存储 | NVS / Flash | **1KB DataFlash 用户区（模拟 EEPROM）** |
| 网络 | 有 | **无**（能力子集剔除网络项）|
| 寄存器 | 32 位内存映射 | **32 位外设空间 0x40000000+（RegAbility 宽度 32）** |

### 七、platformio_ide 版实现说明（RISC-V 寄存器级）

`platformio_ide/` 版为纯 C 裸机，`fe_port.c` 直接操作 CH32V003 外设寄存器：

| 功能 | 实现 |
|------|------|
| UART | **USART1**（RCC 时钟 + BRR 波特率 + STATR/DATAR）|
| EEPROM | DataFlash 参考（TODO 已留接口）|
| 时间 | **SysTick**（内核定时器 1ms）|
| GPIO | **GPIOA-D**（CFGLR/CFGHR/INDR/OUTDR/BSHR/BCR）|
| 随机数 | LCG |

```bash
cd platformio_ide
pio run            # 编译
pio run -t upload  # 烧录
pio device monitor # 串口监视（115200）
```

### 八、MCU 专有模块

除主仓库对应能力外，本仓库提供 3 个 **MCU 专有** 模块（寄存器 / GPIO / 芯片信息）。CH32V003 的寄存器为 **32 位 RISC-V 外设空间**（0x40000000+），GPIO 为 Arduino 引脚号 0-19：

| 模块 | 类型 | 命令 | 说明 |
|------|------|------|------|
| RegAbility | Ability | `read <addr>` / `write <addr>,<value>` / `bit_set <addr>,<bit>` / `bit_clear <addr>,<bit>` / `info` | RISC-V 内存映射寄存器（32 位，volatile 指针） |
| GpioAbility | Ability | `mode <pin>,<input\|output\|input_pullup>` / `write <pin>,<0\|1>` / `read <pin>` / `info` | Arduino 引脚 GPIO（pin 0-19，寄存器级） |
| ChipData | Data | `info` | CH32V003 型号 / RAM / Flash / EEPROM / 频率 |

**示例：**

```
ability_RegAbility read 0x50000000      # 读 GPIOA 配置
ability_RegAbility write 0x50000018,0x01 # 写 GPIOA BSHR
ability_GpioAbility mode 13,output
ability_GpioAbility write 13,1
ability_GpioAbility read 2
data_ChipData info
```

> ⚠️ 寄存器操作直接访问硬件，误写可能导致系统异常，仅供调试/底层驱动使用。

### 九、与 FasterEdge 主仓库的对应关系

- 命令名与主仓库**完全一致**，与 MCU-C51 / MCU-ESP32 实现同构
- `Atom` 模型：单例全局 Atom，`data_` / `ability_` 前缀路由
- 令牌用 HMAC-SHA256（纯 C，无 mbedTLS），密钥 EEPROM 持久化
- Modbus 寄存器表存 RAM，RTU 帧服务入口 `modbus_slave_service()` 已预留

### 十、姊妹项目

- **[FasterEdge MCU - ESP32](https://github.com/FasterEdge/MCU-ESP32)**：双核、WiFi/BLE、更多外设
- **[FasterEdge MCU - ESP8266](https://github.com/FasterEdge/MCU-ESP8266)**：WiFi、低功耗
- **[FasterEdge MCU - C51](https://github.com/FasterEdge/MCU-C51)**：8 位 8051，最精简
- **[FasterEdge MCU - Arduino Uno R3](https://github.com/FasterEdge/MCU-Arduino-Uno-R3)**：8 位 AVR（ATmega328P）
- **[FasterEdge MCU - Arduino Uno R4](https://github.com/FasterEdge/MCU-Arduino-Uno-R4)**：32 位 Cortex-M4F（RA4M1）
- **[FasterEdge](https://github.com/FasterEdge/FasterEdge)**：框架主仓库
