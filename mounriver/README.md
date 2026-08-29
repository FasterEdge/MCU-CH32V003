# FasterEdge MCU - CH32V003（MounRiver-Studio 工程）

WCH 官方 IDE **MounRiver Studio（MRS）** 基于 Eclipse，工程文件为 `.project` / `.cproject`。

## 使用方法

1. 从 [WCH 官网](http://www.mounriver.com/) 下载并安装 MounRiver Studio（内置 RISC-V GCC 工具链）
2. `File → Import → General → Existing Projects into Workspace`，选择本目录（`mounriver/`）
3. 打开工程后自动识别为 RISC-V 工程，右键项目 → `Build Project`
4. 烧录：菜单栏 **Download**（需接入 WCH-Link），或直接生成 HEX 后用 WCH-LinkUtility 烧录

## 目录

```
mounriver/
├── .project / .cproject   # MounRiver-Studio 工程文件（Eclipse CDT）
├── Src/                   # fe.c / ability_*.c / data_*.c / fe_port.c / main.c
└── Inc/                   # fe.h / fe_ability.h / fe_data.h / fe_port.h / fe_hmac_sha256.h
```

> 说明：`Src/` 与 `Inc/` 与 `platformio_ide/` 共享同一套代码；MounRiver 版未捆绑 WCH 标准外设库，
> 如需使用 `ch32v00x.h` 寄存器定义可在工程设置中引用官方例程的 `Peripheral/inc` 或在 `fe_port.c` 中
> 保持当前寄存器地址宏定义（本工程已内置最小寄存器宏，可直接编译）。
