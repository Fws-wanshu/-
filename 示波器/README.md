# STM32F103VET6 双通道示波器

基于STM32F103VET6的双通道ADC数据采集系统，支持定时器触发、DMA传输和环形缓冲区管理。

## 功能特性

- **双通道ADC采集**：ADC1_IN1 (PA1), ADC1_IN2 (PA2)
- **采样率**：10kHz
- **分辨率**：10位
- **触发方式**：定时器触发 (TIM2)
- **实时波形缓存**：每通道512点环形缓冲区
- **按键控制**：PA0切换运行/停止状态
- **串口控制**：USART1 (115200bps)
- **命令协议**：START/STOP/CH1/CH2

## 硬件平台

| 部件 | 规格 |
|------|------|
| 主控 | STM32F103VET6 |
| 系统时钟 | 72MHz (HSE 8MHz + PLL×9) |
| ADC时钟 | 12MHz (PCLK2/6) |

## 文件结构

```
.
├── Oscilloscope.c          # 主程序（寄存器级实现，无HAL依赖）
├── Oscilloscope.h          # 头文件
├── stm32f10x.h             # STM32寄存器定义
├── system_stm32f10x.c      # 系统初始化（含SysTick配置）
├── startup_stm32f10x_hd.s  # 启动文件
├── stm32f103vet6_flash.ld  # 链接脚本
├── main.c                  # 备用主程序文件（HAL版本）
├── oscope_receiver.py      # PC端Python接收程序
└── README.md               # 项目说明文档
```

## 硬件连接

| STM32引脚 | 功能 | 连接说明 |
|-----------|------|----------|
| PA0 | 按键输入 | 接按键到GND（内部上拉） |
| PA1 | ADC1_IN1 | 通道1模拟输入 |
| PA2 | ADC1_IN2 | 通道2模拟输入 |
| PA9 | USART1_TX | 串口发送（连接PC RX） |
| PA10 | USART1_RX | 串口接收（连接PC TX） |
| VCC | 电源 | 3.3V |
| GND | 地 | 电源地 |

## 软件配置

### Keil MDK配置（详细步骤）

**1. 创建新工程**
- 打开Keil MDK，点击 `Project -> New μVision Project`
- 选择保存路径，输入工程名
- 在 `Select Device for Target` 中选择 `STMicroelectronics -> STM32F1 Series -> STM32F103 -> STM32F103VE`
- 选择 `No` 跳过添加标准库

**2. 添加源文件**
- 右键 `Source Group 1` -> `Add Existing Files to Group 'Source Group 1'`
- 添加以下文件：
  - `Oscilloscope.c`
  - `system_stm32f10x.c`
  - `startup_stm32f10x_hd.s`

**3. 添加头文件路径**
- 点击 `Options for Target` (魔术棒图标)
- 切换到 `C/C++` 选项卡
- 在 `Include Paths` 中添加项目目录路径

**4. 配置链接脚本**
- 切换到 `Linker` 选项卡
- 在 `Use Memory Layout from Target Dialog` 取消勾选
- 在 `Scatter File` 中选择 `stm32f103vet6_flash.ld`

**5. 设置编译选项**
- 切换到 `C/C++` 选项卡
- `Compiler`: 选择 `ARM Compiler v6` (AC6)
- `Optimization`: `-O2`
- `Language`: `C99`
- 勾选 `Enable MISRA C:2012` (可选)

**6. 配置目标选项**
- 切换到 `Target` 选项卡
- `Code Generation`:
  - `Thumb Mode`: 勾选
  - `Use MicroLIB`: 勾选

**7. 编译工程**
- 点击 `Build` 或 `Rebuild` 按钮
- 确保没有编译错误和警告

### 编译检查清单

确保以下文件已正确添加：
| 文件 | 类型 | 必须添加 |
|------|------|----------|
| Oscilloscope.c | C源文件 | ✓ |
| system_stm32f10x.c | C源文件 | ✓ |
| startup_stm32f10x_hd.s | 汇编启动文件 | ✓ |
| stm32f10x.h | 头文件 | 自动包含 |
| Oscilloscope.h | 头文件 | 自动包含 |
| stm32f103vet6_flash.ld | 链接脚本 | ✓ (在Linker配置中指定) |

### 时钟配置

```
HSE = 8MHz
PLL = HSE × 9 = 72MHz (系统时钟)
APB1 = 36MHz (TIM2时钟 = 72MHz)
APB2 = 72MHz (ADC/USART1)
ADC预分频 = 6 → 12MHz
```

### 定时器配置 (TIM2)

- 预分频器：0
- 自动重装载：7200 - 1 = 7199
- 触发输出：更新事件触发ADC

### 采样率计算

```
TIM2频率 = 72MHz
ARR = 7199
采样率 = 72MHz / (7199 + 1) = 10kHz
```

## 串口命令协议

### 命令列表

| 命令 | 功能 | 响应 |
|------|------|------|
| `START` | 开始采集 | `START\r\n` |
| `STOP` | 停止采集 | `STOP\r\n` |
| `CH1` | 获取通道1数据 | 二进制帧 |
| `CH2` | 获取通道2数据 | 二进制帧 |

### 数据帧格式

```
┌──────┬────────┬─────────┬───────────┬──────────┬─────┐
│ 0xAA │ Channel│ Length  │ Data[N]   │ Checksum │0x55│
│  (1B)│  (1B)  │  (2B)   │ (N×2B)    │   (1B)   │(1B)│
└──────┴────────┴─────────┴───────────┴──────────┴─────┘
```

- **Header**: 0xAA（帧起始标志）
- **Channel**: 通道号 (1或2)
- **Length**: 数据长度（512，小端序）
- **Data**: int16_t数组（512点，小端序）
- **Checksum**: 数据部分所有字节之和（低8位）
- **Tail**: 0x55（帧结束标志）

## PC端接收程序

### 环境要求

- Python 3.x
- pyserial
- matplotlib
- numpy

### 安装依赖

```bash
pip install pyserial matplotlib numpy
```

### 运行程序

```bash
python oscope_receiver.py [串口名]
```

示例：
```bash
python oscope_receiver.py COM3
```

### 使用说明

1. 运行Python程序，选择串口
2. 串口助手发送 `START` 命令开始采集
3. 发送 `CH1` 或 `CH2` 获取波形数据
4. 发送 `STOP` 停止采集（保持最后一帧）
5. 或使用PA0按键切换运行/停止状态

## 工作原理

### 采集流程

```
TIM2 (10kHz) ──▶ ADC触发 ──▶ 双通道扫描 ──▶ DMA传输 ──▶ 环形缓冲区
                                                     │
                                                     ▼
                                              串口命令读取
```

### 环形缓冲区机制

- DMA循环模式自动填充缓冲区
- 双通道交替存储：CH1[0], CH2[0], CH1[1], CH2[1], ...
- 读取时按通道分离数据

## 代码说明

### 关键函数

| 函数 | 功能 |
|------|------|
| `SystemClock_Config()` | 配置系统时钟为72MHz |
| `GPIO_Config()` | 配置GPIO引脚 |
| `ADC_Config()` | 配置ADC扫描模式和定时器触发 |
| `DMA_Config()` | 配置DMA循环传输 |
| `Timer_Config()` | 配置TIM2为10kHz触发源 |
| `USART_Config()` | 配置USART1为115200bps |
| `Send_Channel_Data()` | 发送通道数据（二进制格式） |
| `Parse_Command()` | 解析串口命令 |

### 中断处理

- **USART1_IRQHandler**: 串口接收中断，解析命令
- **按键检测**: 主循环轮询，带50ms消抖

## 注意事项

1. ADC输入范围：0-3.3V（10位分辨率，0-1023）
2. 串口波特率：115200bps，8N1
3. 按键为低电平有效（内部上拉）
4. 停止采集后缓冲区保持最后一帧数据

## 许可证

MIT License
