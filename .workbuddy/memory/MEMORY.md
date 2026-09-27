# MCU 工程长期笔记（GD32F427 / Keil MDK）

## 工程基本信息
- 工程文件：`project/template.uvprojx`（单 Target："Target 1"）
- 工具链：Keil MDK-ARM Plus V5.29，**AC5 = ARMCC V5.06 update 6**（不是 AC6），路径 `D:\keil\ARM\ARMCC\Bin`
- 器件：GD32F427VE，Pack `GigaDevice.GD32F4xx_DFP.3.5.0`
- 预定义宏：`USE_STDPERIPH_DRIVER,GD32F427`（`GD32F427` 必需，`gd32f4xx.h` 会 `#error` 检查）
- 底层宏：C99 + GNU 模式（`uC99=1`, `uGnu=1`）
- 输出：`..\Output\`，生成 `template.axf` + `template.hex`

## 目录职责（分层）
| 目录 | 职责 |
|---|---|
| `App/` | `main.c`、`app_cmd.c`（shell 命令注册）；main.c 自带 fputc / `__use_no_semihosting` |
| `Components/` | 第三方组件：`lettershell`（shell 解析）、`circular_buffer`（环形缓冲） |
| `HAL/` | 硬件抽象层：`hal_*.h/.c`（芯片无关接口 + 分发器）+ `port/gd32f4xx/hal_*_port.c`（芯片相关实现） |
| `Driver/` | （已弃用）旧的 `drv_*`/`drvp_*` 驱动，已从工程移除（文件仍留在磁盘上） |
| `Bsp/gd32f4/` | GD32F4xx 标准外设库（`Include/` + `Source/`）与 CMSIS core 头 |
| `Startup/` | 启动文件、`system_gd32f4xx.c`、`gd32f4xx_it.c`、`systick.c` |

## IncludePath 约定（重要）
必须包含（相对 `project/`）：
`..\App; ..\Bsp\gd32f4; ..\Bsp\gd32f4\Include; ..\Startup; ..\Startup\Include;
 ..\Components\circular_buffer\src; ..\Components\circular_buffer\port;
 ..\Components\lettershell\src; ..\Components\lettershell\port;
 ..\Driver\drv_dma|drv_gpio|drv_uart; ..\Driver\drvp_dma|drvp_gpio|drvp_uart`

- `..\Startup` 必须在列表里：`systick.h` 在该目录，且 `core_cm4.h` 用
  `#include <core_cmInstr.h>`（尖括号只搜 -I 路径）也要靠它命中。
- **DFP 包里没有 `core_cm4.h`**，所以 `gd32f4xx.h` 绝不能解析到
  `D:\keil\GigaDevice\GD32F4xx_DFP\3.5.0\Device\F4XX\Include`（这是器件自动加的路径）。
  实测 Keil 用户 IncludePath 优先级高于该自动路径，本地副本会先命中。

## 已知陷阱
- **两套重复库副本**：`Startup/` 下的 `Include/`、`Source/`、`core_cm*.h`、
  `system_gd32f4xx.h`、`gd32f450i_eval.h` 与 `Bsp/gd32f4/` 完全相同（仅 `gd32f4xx.h` 的
  `HXTAL_VALUE` 不同：Bsp=25000000，Startup=8000000）。`Startup/Source/` 的 .c 未参与编译。
  建议只保留一份、统一 HXTAL。
- **晶振配置**：实际跑的是 **8MHz HXTAL / 200MHz SYSCLK**
  （`Startup/system_gd32f4xx.c` 启用 `__SYSTEM_CLOCK_200M_PLL_8M_HXTAL`）。改晶振要同时改
  `gd32f4xx.h` 的 `HXTAL_VALUE` 和该宏。
- `Driver/drv_i2c/`、`Driver/drvp_i2c/` 目前是空文件，未加入工程。
- `App/app_cmd/` 是旧版残留目录，正式版是 `App/app_cmd.c`（`SHELL_TYPE_CMD_MAIN`）。

## 命令行构建
```bash
"/d/keil/UV4/UV4.exe" -j0 -b template.uvprojx    # 在 project/ 目录下执行
```
退出码：0=无警告无错误，1=有警告，2=有错误。构建日志看 `Output/template.build_log.htm`。

## 分层架构方向（已实现，用户目标）
- 用户是嵌入式小白，想实现「硬件层 / 抽象层分离」，换芯片只改底层。**已删除 drv/drvp，改用 HAL 注册-ops 模式**。
- 实际结构（GD32F4xx 已跑通 0 error）：
  - `HAL/hal_gpio.h`：芯片无关接口 + 抽象类型。`hal_gpio_t` = `port*16+pin`（宏 `HAL_GPIO_(port,pin)`）；
    `hal_gpio_ops_t` 函数指针表；公共 API `hal_gpio_init/write/read/toggle` + `hal_gpio_register_ops`。
  - `HAL/hal_gpio.c`：分发器，把公共 API 转发给已注册的 ops（未注册时安全返回）。
  - `HAL/port/gd32f4xx/hal_gpio_port.c`：唯一芯片相关文件，直接调 GD32 库（gpio_mode_set 等）；
    底层函数全部 static（**名字不能和 hal_gpio.h 公共 API 重名，否则 duplicate symbol**），
    通过 `hal_gd32f4_gpio_register()` 注册。
  - `HAL/port/gd32f4xx/hal_port.h/.c`：`hal_port_init()` 统一注册入口；`hal_port_internal.h` 声明各 register。
- 换芯片：只重写 `port/` 下对应 `*_port.c`（+ 换库），`hal_gpio.h/.c` 和 App 层不动。
- 尚待实现：hal_uart / hal_dma / hal_i2c 的 port（目前是空桩）。

## 环境注意
- 本机 Git Bash 的 PATH 是空的，跑 bash 命令前需先
  `export PATH="/c/Users/Lenovo/.workbuddy/binaries/PortableGit/versions/1.2.0/usr/bin:/usr/bin:/bin"`。
- PowerShell 工具在本机捕获不到 stdout，取信息优先用 Bash / Read / Grep。
