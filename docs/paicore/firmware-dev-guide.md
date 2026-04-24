# PaiCore RISC-V 固件开发与上板测试指南

本文档覆盖两个开发阶段的完整记录：

1. **推理工作流程**（FreeRTOS + EmbeddedProto 系统构建）
2. **芯片配置流程**（配置帧 / 测试帧）

以及工具链对接规范、x86 测试运行方法、上板测试完整流程和测试建议。

---

## 一、系统架构概览

```
PC (Python host)
    │  USB-UART 桥 (3 MHz)
    ▼
RISC-V (N307FD, FreeRTOS)
    ├── uart_rx_task   ← 接收 HostMessage (protobuf)
    ├── dispatch_task  ← 路由 init/infer 到对应 worker
    ├── worker_task[0] ← 推理线程 0
    ├── worker_task[1] ← 推理线程 1（前期暂不使用）
    ├── config_task    ← 芯片配置 / 测试帧
    └── uart_tx_task   ← 发送 DeviceMessage (protobuf)
         │
         ▼
    PaiCore 离线核 (NoC FIFO)
```

所有 PC ↔ RV 通信使用同一套 UART 帧格式：

```
[PAIC][len_lo][len_hi][protobuf payload...]
```

magic = `0x50 0x41 0x49 0x43`，长度字段为 little-endian uint16。

---

## 二、Protobuf 消息定义

proto 文件位于 `application/freertos/embedded_proto/proto/paicore.proto`。

### HostMessage（PC → RV）

| 字段 | 类型 | 说明 |
|------|------|------|
| `init_req.thread_id` | uint32 | 初始化指定线程 |
| `infer_req.thread_id` | uint32 | 推理目标线程 |
| `infer_req.timesteps` | uint32 | 时间步数 T |
| `infer_req.data` | bytes | 输入张量，[T×C×H×W] 展平，uint8 |
| `config_req.block_index` | uint32 | 配置指定 block |
| `test_req.block_index` | uint32 | 测试指定 block |

### DeviceMessage（RV → PC）

| 字段 | 类型 | 说明 |
|------|------|------|
| `init_resp.thread_id` | uint32 | 对应线程 |
| `init_resp.success` | bool | 是否成功 |
| `infer_resp.thread_id` | uint32 | 对应线程 |
| `infer_resp.success` | bool | 是否成功 |
| `infer_resp.data` | bytes | 输出张量，展平 uint8 |
| `infer_resp.cycles` | uint32 | 推理耗时（FreeRTOS ticks） |
| `config_resp.block_index` | uint32 | 对应 block |
| `config_resp.success` | bool | 是否成功 |
| `test_resp.block_index` | uint32 | 对应 block |
| `test_resp.success` | bool | 是否成功 |
| `test_resp.mismatch_frame` | uint32 | 首个不匹配帧索引（失败时有效） |

---

## 三、工具链对接规范

工具链负责将神经网络编译结果输出为两个 C 数据文件，直接替换固件中的 stub 文件后重新编译烧录。

### 3.1 neuron_map_data.c — 神经元映射表

**文件路径**：`application/freertos/embedded_proto/data/neuron_map_data.c`

工具链需要输出以下内容（替换现有 stub）：

```c
#include "neuron_map.h"

// 输入映射：[C,H,W] → [CoreAddr, AxonAddr]
// 数组大小固定为 MAX_NEURONS = MAX_C * MAX_H * MAX_W
// 按 c*H*W + h*W + w 线性索引
static const input_neuron_mapping_t input_neuron_map_table[MAX_NEURONS]
    __attribute__((section(".large_const_data"))) = {
    // 每个条目：{core_xy, core_x, core_y, copy_xy, copy_x, copy_y, axon_addr}
    // copy_* 字段用于多播，单播时填 0
    {0x01, 0x01, 0x00, 0, 0, 0, 0x000},  // neuron [c=0,h=0,w=0]
    {0x01, 0x01, 0x00, 0, 0, 0, 0x001},  // neuron [c=0,h=0,w=1]
    // ... 共 MAX_NEURONS 条，未使用的神经元填 {0,0,0,0,0,0,0}
};

// 输出映射：[axon_addr] → [c,h,w]
// 数组大小固定为 [MAX_THREADS][MAX_AXON_ADDR]
// 每个线程一张表（前期两张表内容相同）
const output_neuron_mapping_t output_neuron_map_table[MAX_THREADS][MAX_AXON_ADDR]
    __attribute__((section(".large_const_data"))) = {
    // thread 0
    {
        // {c, h, w, valid}
        {0, 0, 0, 1},  // axon 0 → output [c=0,h=0,w=0]
        {0, 0, 1, 1},  // axon 1 → output [c=0,h=0,w=1]
        // ... 未使用的 axon 填 {0, 0, 0, 0}（valid=0）
    },
    // thread 1（前期与 thread 0 相同）
    { ... },
};

void neuron_map_init(void)
{
    // 填写实际网络的 C/H/W 维度
    g_map_ctx[0].input_map  = input_neuron_map_table;
    g_map_ctx[0].output_map = output_neuron_map_table[0];
    g_map_ctx[0].C = <实际C>; g_map_ctx[0].H = <实际H>; g_map_ctx[0].W = <实际W>;
    g_map_ctx[0].max_axon = MAX_AXON_ADDR;

    g_map_ctx[1].input_map  = input_neuron_map_table;
    g_map_ctx[1].output_map = output_neuron_map_table[1];
    g_map_ctx[1].C = <实际C>; g_map_ctx[1].H = <实际H>; g_map_ctx[1].W = <实际W>;
    g_map_ctx[1].max_axon = MAX_AXON_ADDR;
}
```

**关键约束**：

- `MAX_NEURONS`、`MAX_AXON_ADDR` 在 `paicore_config.h` 中定义，当前值为 `64×8×8=4096` 和 `512`。如果实际网络超出这些上限，需要同步修改 `paicore_config.h`。
- `input_neuron_map_table` 中，值为 0 的 axon_addr 不代表"无效"——固件通过 `val == 0` 跳过零值输入（稀疏编码），而不是通过 axon_addr 判断有效性。
- `output_neuron_map_table` 中，`valid=0` 的条目会被固件忽略。
- 两个数组必须放在 `.large_const_data` section（链接到 flash）。

### 3.2 config_meta.c — 芯片配置数据块

**文件路径**：`application/freertos/embedded_proto/data/config_meta.c`

工具链需要输出以下内容（替换现有 stub）：

```c
/* 工具链自动生成 — 请勿手动修改 */
#include "config_meta.h"

// 每个 block 的原始数据（64-bit 帧对，按 low/high 顺序存储）
// data[i]   = 第 i/2 帧的 low  word
// data[i+1] = 第 i/2 帧的 high word
static const uint32_t _core0_data[6] = {
    /* frame 0 low  */ 0xXXXXXXXX,
    /* frame 0 high */ 0xXXXXXXXX,
    /* frame 1 low  */ 0xXXXXXXXX,
    /* frame 1 high */ 0xXXXXXXXX,
    /* frame 2 low  */ 0xXXXXXXXX,
    /* frame 2 high */ 0xXXXXXXXX,
};

static const uint32_t _lut0_data[N*2] = { ... };    // N 帧
static const uint32_t _neuron0_data[N*2] = { ... }; // N 帧
static const uint32_t _input0_data[N*2] = { ... };  // N 帧

const config_block_t g_config_blocks[] = {
    // subtype 0 = Core(1型)，data_words 必须 = 6（固定 3 帧）
    {
        .subtype    = 0,
        .core_xy    = 0x01,
        .core_x     = 0x01,
        .core_y     = 0x00,
        .sram_addr  = 0,       // Core 类型保留，填 0
        .data       = _core0_data,
        .data_words = 6,       // 必须为 6，不可更改
    },
    // subtype 1 = LUT(2型)，data_words = 帧数 × 2，帧数可变
    {
        .subtype    = 1,
        .core_xy    = 0x01,
        .core_x     = 0x01,
        .core_y     = 0x00,
        .sram_addr  = 0,       // SRAM 起始地址（原始值，9-bit）
        .data       = _lut0_data,
        .data_words = 512,     // 256 帧 × 2 words/帧（示例，可按需调整）
    },
    // subtype 2 = Neuron(3型)，data_words = 帧数 × 2
    {
        .subtype    = 2,
        .core_xy    = 0x01,
        .core_x     = 0x01,
        .core_y     = 0x00,
        .sram_addr  = 0,       // SRAM 起始地址 ÷ 8（硬件要求对齐）
        .data       = _neuron0_data,
        .data_words = 16384,   // 8192 帧 × 2（示例，可按需调整）
    },
    // subtype 3 = Input(4型)，data_words = 帧数 × 2
    {
        .subtype    = 3,
        .core_xy    = 0x01,
        .core_x     = 0x01,
        .core_y     = 0x00,
        .sram_addr  = 0,       // SRAM 起始地址（原始值）
        .data       = _input0_data,
        .data_words = 4096,    // 2048 帧 × 2（示例，可按需调整）
    },
};
```

**关键约束**：

| subtype | 类型 | data_words | 帧数 | sram_addr 含义 |
|---------|------|-----------|------|----------------|
| 0 | Core(1型) | **必须 = 6** | **固定 3** | 保留，填 0 |
| 1 | LUT(2型) | 偶数，≥2 | 可变 | SRAM 起始地址（原始值） |
| 2 | Neuron(3型) | 偶数，≥2 | 可变 | SRAM 起始地址 ÷ 8 |
| 3 | Input(4型) | 偶数，≥2 | 可变 | SRAM 起始地址（原始值） |

- `data_words` 必须为偶数（每帧 = 2 个 uint32）。
- `data[i]` 存 low word，`data[i+1]` 存 high word（与 `frame64_t.low/high` 对应）。
- 数组顺序即发送顺序，固件按 `i += 2` 遍历，直接写入 NoC FIFO。
- `g_config_blocks[]` 的顺序决定 `block_index`（0-based），PC 端按此索引发送请求。

### 3.3 paicore_config.h 中需要工具链确认的参数

```c
#define MAX_C    64   // 输入张量通道数上限
#define MAX_H    8    // 输入张量高度上限
#define MAX_W    8    // 输入张量宽度上限
#define MAX_AXON_ADDR  512  // 输出 axon 地址空间（由 LCN 参数决定）
```

如果实际网络的 C/H/W 或 axon 地址超出这些值，修改 `paicore_config.h` 后重新编译。

---

## 四、x86 测试运行方法

所有测试不需要硬件，在开发机上直接运行。

```bash
cd application/freertos/embedded_proto/test
make clean && make run
```

预期输出：

```
=== test_ringbuf ===
ALL ringbuf tests PASSED
=== test_frame_builder ===
ALL frame_builder tests PASSED
=== test_neuron_map ===
ALL neuron_map tests PASSED
=== test_uart_frame ===
ALL uart_frame tests PASSED
=== test_config_frame ===
test_config_meta_count PASSED
test_config_meta_fields PASSED
test_build_cfg_header_config PASSED
test_build_cfg_header_test_req PASSED
test_build_cfg_header_masks PASSED
ALL config_frame tests PASSED
ALL TESTS PASSED
```

各测试覆盖范围：

| 测试二进制 | 覆盖内容 |
|-----------|---------|
| `test_ringbuf` | 环形缓冲区 push/pop/wrap/clear |
| `test_frame_builder` | 工作帧/控制帧位域构造和解析 |
| `test_neuron_map` | 输入/输出神经元映射查找 |
| `test_uart_frame` | UART 帧编解码、magic 扫描 |
| `test_config_frame` | config_meta 数据完整性、配置帧头位域 |

Python 端测试（需要 `grpc_tools` 或 `protoc`）：

```bash
cd application/freertos/embedded_proto/tools
python3 -m pytest test_host_protocol.py -v
```

覆盖 19 个 protobuf 消息编解码和 UART 帧格式测试。

---

## 五、上板测试完整流程

### 5.1 硬件连接

- USB-UART 桥接到 PC，波特率 **3 MHz**
- 确认串口设备（Linux: `/dev/ttyUSB0`，Windows: `COMx`）

### 5.2 固件编译与烧录

```bash
# 替换 stub 数据文件后编译
make CORE=n307fd DOWNLOAD=ilmflashxip \
     PROGRAM=application/freertos/embedded_proto all

# 烧录（具体命令取决于调试器）
make CORE=n307fd DOWNLOAD=ilmflashxip \
     PROGRAM=application/freertos/embedded_proto upload
```

### 5.3 主机端 Python 发送脚本

源文件：`application/freertos/embedded_proto/tools/host_client.py`

```bash
pip install pyserial numpy
cd application/freertos/embedded_proto/tools
python3 host_client.py --port /dev/ttyUSB0 --C 2 --H 2 --W 2 --num-blocks 4
```

参数说明：

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--port` | 串口设备 | `/dev/ttyUSB0` |
| `--baud` | 波特率 | `3000000` |
| `--num-blocks` | `g_config_blocks[]` 的长度 | `4` |
| `--C/H/W` | 网络输入维度 | `2/2/2` |
| `--T` | 时间步数 | `1` |

脚本按顺序执行：配置所有 block → 测试帧验证 → init → 零输入推理 → 10 次随机输入压力测试。

### 5.4 离线验证 host_client.py（无硬件）

在没有板子的情况下，可以用 `test_host_client.py` 验证帧编解码逻辑是否正确：

```bash
cd application/freertos/embedded_proto/tools
python3 -m pytest test_host_client.py -v
```

---

## 六、推理数据格式说明

### 输入张量 [T, C, H, W]

- 展平顺序：`data[t * C*H*W + c * H*W + h * W + w]`
- 数据类型：uint8（0 表示无脉冲，非零表示脉冲强度）
- 固件对零值做稀疏跳过（`if (val == 0) continue`），零值不产生工作帧
- 最大尺寸：`PROTO_BUF_SIZE = 2048` 字节（T × C × H × W ≤ 2048）

### 输出张量

- 展平顺序：`data[c * H*W + h * W + w]`（无时间维度，固件取最后一个时间步的输出）
- 数据类型：uint8
- 长度由 `infer_resp.data_len` 决定（`= max_valid_index + 1`）

---

## 七、上板测试建议

### 7.1 测试顺序

按以下顺序进行，每步通过后再进行下一步：

```
1. UART 通信基础测试
2. 芯片配置（config）
3. 配置验证（test）
4. 推理初始化（init）
5. 推理功能（infer）
6. 压力测试
```

### 7.2 各测试项目详细说明

#### 测试 1：UART 通信基础

**目的**：确认串口连通、波特率、帧格式正确。

**方法**：发送一个 `InitRequest(thread_id=0)`，观察是否收到 `InitResponse`。

**通过标准**：收到合法的 `InitResponse`（无论 success 是否为 true）。

**失败排查**：
- 无响应 → 检查波特率（3 MHz）、串口设备名、USB-UART 驱动
- 收到乱码 → 检查 magic 字节对齐，尝试 `ser.reset_input_buffer()` 后重试

#### 测试 2：芯片配置

**目的**：验证 RV 能正确将配置数据通过 NoC 写入离线核 SRAM。

**方法**：按 block_index 顺序发送所有 `ConfigRequest`，检查每个 `ConfigResponse.success`。

**通过标准**：所有 block 返回 `success=True`。

**注意**：配置顺序建议 Core(0) → LUT(1) → Neuron(2) → Input(3)，与硬件初始化顺序一致。

#### 测试 3：配置验证（测试帧）

**目的**：验证写入 SRAM 的数据与工具链生成的数据完全一致。

**方法**：对每个 block 发送 `TestRequest`，检查 `TestResponse.success`。

**通过标准**：所有 block 返回 `success=True`。

**失败排查**：
- `success=False`，`mismatch_frame=N` → 第 N 帧数据不匹配，检查 `config_meta.c` 中对应 block 的数据是否正确
- 超时（10 秒）→ 检查 NoC 连接，确认离线核已上电

#### 测试 4：推理初始化

**目的**：验证初始化帧能正确重置离线核状态。

**方法**：发送 `InitRequest(thread_id=0)`，检查 `InitResponse.success`。

**通过标准**：`success=True`，响应时间 < 2 秒（`INIT_TIMEOUT_MS=2000`）。

#### 测试 5：推理功能

**目的**：端到端验证推理流程。

**5a. 零输入测试**

发送全零输入（T=1），期望输出全零（无脉冲输入 → 无输出）。

```python
data = np.zeros(T * C * H * W, dtype=np.uint8)
```

**5b. 已知输入测试**

使用工具链提供的测试向量（已知输入 → 已知输出），验证推理结果正确性。

**5c. 多时间步测试**

发送 T>1 的输入，验证时间步计数正确。

**通过标准**：
- `success=True`
- 输出与工具链参考输出一致
- `cycles` 值合理（不为 0，不超时）

#### 测试 6：压力测试

**目的**：验证系统在连续工作下的稳定性。

**方法**：
1. 连续发送 100 次推理请求，检查每次响应
2. 交替发送 init + infer，验证状态重置正确
3. 发送边界尺寸输入（最大 2048 字节）

**通过标准**：100 次全部成功，无超时，无数据错误。

### 7.3 不需要测试的项目（前期）

- thread_id=1（第二个推理线程）：前期只使用单线程
- 多播（Copy_XY/Copy_X/Copy_Y 非零）：config_task 强制置 0
- 膜电平帧（工作帧 2 型）：当前固件只实现普通数据帧

### 7.4 调试建议

固件在 `main()` 启动时会通过 UART 打印：

```
[PAICORE] PaiCore RISC-V Firmware
[PAICORE] CPU: XXXXXXXX Hz
```

如果看不到这两行，说明固件未正常启动。

超时参数（`paicore_config.h`）：

```c
#define INFER_TIMEOUT_MS   5000   // 推理超时 5 秒
#define INIT_TIMEOUT_MS    2000   // 初始化超时 2 秒
// config_task.c:
#define CONFIG_TEST_TIMEOUT_MS  10000  // 配置/测试超时 10 秒
```

如果硬件响应较慢，可适当增大这些值后重新编译。
