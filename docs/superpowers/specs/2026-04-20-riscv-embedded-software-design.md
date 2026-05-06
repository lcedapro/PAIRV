# RISC-V 侧嵌入式软件技术方案设计

**日期：** 2026-04-20
**项目：** PaiCore 类脑芯片 RISC-V 控制器软件
**状态：** 待实现

---

## 1. 背景与约束

### 1.1 硬件环境

| 资源 | 规格 |
|------|------|
| RISC-V CPU | 800 MHz，单核 |
| 片上指令存储 | 128 KB ILMFLASH |
| 片上数据存储 | 128 KB DLMFLASH |
| 片外 FLASH | 2 × 512 Mb，QSPI 160 MHz |
| UART（host 接口） | 波特率 3,000,000 bps |
| NOC FIFO（下行） | FIFO_DOWN_ADDR = 0x10008000，64-bit 写 |
| NOC FIFO（上行） | FIFO_UP_ADDR = 0x10010000，64-bit 读 |
| SNN 中断 | SNN_IRQn，CLIC，优先级 1 |
| 加速外设 | BatchNorm 外设、Softmax 外设（riscv 独占） |

### 1.2 离线核帧格式（摘要）

帧为 64-bit，高 32 位（high）含帧头和地址字段，低 32 位（low）含 LOAD（24-bit）。

**工作帧（1型，普通数据帧）**：
- [63:62] = `0b10`（工作帧标识）
- [61] = `0b0`（1型）
- [60] = TS[7]
- [59:54] = Core_XY（6-bit）
- [53:48] = Core_X（6-bit）
- [47:42] = Core_Y（6-bit）
- [41:36] = Copy_XY（6-bit）
- [35:30] = Copy_X（6-bit，跨越 high/low 边界，实现时需分高低 32 位处理）
- [29:24] = Copy_Y（6-bit）
- LOAD[23:0]：[23:17] = TS[6:0]，[16:8] = AxonAddr（9-bit，默认 LCN=8:9），[7:0] = Data

> TS 与 AxonAddr 位宽按 LCN 参数动态分配，默认比例 8:9（TS 8-bit，Axon 9-bit）。

**控制帧**：
- [63:62] = `0b11`（控制帧标识）
- [61:60] = 类型：`0b00`=1型（同步帧），`0b01`=2型（初始化帧），`0b10`=3型（完成帧）
- [59:54] = Core_XY（6-bit）
- [53:48] = Core_X（6-bit）
- [47:42] = Core_Y（6-bit）
- [41:36] = Copy_XY（6-bit）
- [35:30] = Copy_X（6-bit，跨越 high/low 边界，实现时需分高低 32 位处理）
- [29:24] = Copy_Y（6-bit）
- LOAD[23:0]：同步帧=执行 TimeStep 数目；完成帧=线程序号

### 1.3 核心需求

1. host（Python 工具链）与 RISC-V 之间通过 **Protobuf（EmbeddedProto）** 通信，双端共享同一份 `.proto` 文件
2. RISC-V 支持**多线程推理**：独立控制多条子网络，异步处理各自的输入输出
3. 配置帧（权重/模式）烧录在片外 FLASH，运行时不重新下发
4. 神经网络输入输出以 `[T, C, H, W]` 张量形式与 host 交互，内部转换为 `[T, CoreAddr, AxonAddr]`

---

## 2. EmbeddedProto 编译可行性

### 2.1 结论：完全可行

EmbeddedProto `src/` 的依赖分析：

| 头文件 | 依赖 | newlib 支持 |
|--------|------|-------------|
| `<array>` | C++11 | ✓ |
| `<type_traits>` | C++11 | ✓ |
| `<cstdint>` | C++11 | ✓ |
| `<algorithm>` | C++11 | ✓ |
| `<cstring>` | C | ✓ |
| `<limits>` | C++11 | ✓ |
| `<math.h>` | C | ✓ |

**无异常、无 RTTI、无动态内存分配（new/delete/malloc）**，全部使用编译期固定大小的 `std::array`。

### 2.2 Makefile 集成方式

在 `application/freertos/embedded_proto/Makefile` 中添加：

```makefile
APP_CXXFLAGS    := -std=c++11
CXX_SRCDIRS     += $(NUCLEI_SDK_ROOT)/third_party/EmbeddedProto/src
INCDIRS         += $(NUCLEI_SDK_ROOT)/third_party/EmbeddedProto/src
INCDIRS         += $(NUCLEI_SDK_ROOT)/application/freertos/embedded_proto/inc/proto
```

生成流程（host 侧，一次性）：

```bash
# 在 host 上执行，生成 .pb.h 文件
cd third_party/EmbeddedProto
python -m pip install -e .
protoc --plugin=protoc-gen-eams=$(pwd)/protoc-gen-eams \
       --eams_out=../../application/freertos/embedded_proto/inc/proto \
       ../../application/freertos/embedded_proto/proto/paicore.proto
```

生成的 `.pb.h` 文件提交到仓库，RISC-V 侧只编译，不运行 protoc。

---

## 3. .proto 文件设计

### 3.1 文件路径

```
application/freertos/embedded_proto/proto/paicore.proto    ← 双端共享的 schema
application/freertos/embedded_proto/inc/proto/paicore.pb.h ← 由 protoc-gen-eams 生成，提交到仓库
```

### 3.2 proto 定义

```protobuf
syntax = "proto3";
package paicore;

// ── Host → RISC-V ─────────────────────────────────────────

// 初始化请求：要求指定线程对应的离线核组执行初始化
message InitRequest {
  uint32 thread_id = 1;
}

// 推理请求：下发一次推理任务
message InferRequest {
  uint32 thread_id = 1;   // 目标线程号
  uint32 timesteps = 2;   // T：推理步数（写入同步帧 LOAD 字段）
  bytes  data      = 3;   // [T,C,H,W] 展平的四维神经网络输入激活值（uint8，packed bytes）
}

// 顶层 host 消息，oneof 保证单帧只含一种消息
message HostMessage {
  oneof payload {
    InitRequest  init_req  = 1;
    InferRequest infer_req = 2;
  }
}

// ── RISC-V → Host ─────────────────────────────────────────

message InitResponse {
  uint32 thread_id = 1;
  bool   success   = 2;
}

message InferResponse {
  uint32 thread_id = 1;
  bool   success   = 2;
  bytes  data      = 3;   // [T,C,H,W] 展平的四维神经网络输出激活值（uint8，packed bytes）
  uint32 cycles    = 4;   // 推理耗时（CPU cycle 数，用于 profiling）
}

message DeviceMessage {
  oneof payload {
    InitResponse  init_resp  = 1;
    InferResponse infer_resp = 2;
  }
}
```

### 3.3 映射表设计

**映射表由工具链在编译期生成，存储在片外 FLASH。需要两张独立的表：**

**输入映射表**（发送方向：`[C,H,W] → [CoreAddr, AxonAddr]`）

```c
// 输入映射表：按 [c*H*W + h*W + w] 索引
typedef struct {
    uint8_t  core_xy;   // Core_XY [5:0]
    uint8_t  core_x;    // Core_X  [5:0]
    uint8_t  core_y;    // Core_Y  [5:0]
    uint16_t axon_addr; // Axon 地址（随 LCN 变化）
} InputNeuronMapping;

extern const InputNeuronMapping input_neuron_map[MAX_C * MAX_H * MAX_W]
    __attribute__((section(".large_const_data")));
```

**输出映射表**（接收方向：`[AxonAddr] → [C,H,W]`，每线程独立一张）

> **背景**：RISC-V 从 FIFO_UP 收到的工作帧，其 CoreAddr 字段已被 NOC 路由过程改写为 0，无法用于识别来源核。因此输出方向只能依赖 AxonAddr 字段，结合完成帧中的线程序号来还原 `[C,H,W]`。

```c
// 输出映射表：按 axon_addr 索引，每线程独立一张
typedef struct {
    uint32_t c, h, w;   // 对应的输出张量坐标
    uint8_t  valid;     // 该 axon_addr 是否有效
} OutputNeuronMapping;

// output_neuron_map[thread_id][axon_addr]
extern const OutputNeuronMapping output_neuron_map[MAX_THREADS][MAX_AXON_ADDR]
    __attribute__((section(".large_const_data")));
```

**RISC-V 侧映射逻辑：**

```
// 发送方向（InferRequest → 工作帧）
for t,c,h,w in enumerate(input):
    m = input_neuron_map[c*H*W + h*W + w]
    构造工作帧：Core_XY=m.core_xy, Core_X=m.core_x, Core_Y=m.core_y,
               AxonAddr=m.axon_addr, TimeStep=t, Data=input[t][c][h][w]
    写入 FIFO_DOWN
发送同步帧：类型=0x00, LOAD=T

// 接收方向（工作帧 → InferResponse）
// active_thread 由 worker_task 在发同步帧前设置，ISR 读取
收到工作帧：
    axon = frame.axon_addr
    m = output_neuron_map[active_thread][axon]
    if m.valid: output[m.c][m.h][m.w] = frame.data
收到完成帧：
    thread_id = frame.load[23:0]
    xSemaphoreGiveFromISR(done_sem[thread_id])
```

---

## 4. FreeRTOS 任务架构

### 4.1 任务清单

| 任务 | 优先级 | 栈大小 | 职责 |
|------|--------|--------|------|
| `uart_rx_task` | 3（最高） | 2 KB | 接收 host 字节流，帧边界检测，Protobuf 反序列化，投递到 dispatch_queue |
| `dispatch_task` | 2 | 1 KB | 按 thread_id 路由消息到对应 Worker 的 input_queue |
| `worker_task[0]` | 1 | 4 KB | 线程 0 推理全流程 |
| `worker_task[1]` | 1 | 4 KB | 线程 1 推理全流程 |
| `uart_tx_task` | 3（最高） | 2 KB | Protobuf 序列化，UART 发送 |

> 初始实现支持 2 个 Worker，`MAX_THREADS` 宏控制，后续可扩展。

### 4.2 同步原语

| 原语 | 类型 | 深度/初值 | 用途 |
|------|------|-----------|------|
| `dispatch_queue` | Queue | 深度 4 | uart_rx_task → dispatch_task |
| `input_queue[N]` | Queue | 深度 2 | dispatch_task → worker_task[N] |
| `done_sem[N]` | 二值信号量 | 0 | SNN_ISR → worker_task[N]，完成帧通知 |
| `tx_queue` | Queue | 深度 4 | worker_task[N] → uart_tx_task |
| `noc_mutex` | 互斥量 | — | 保护 FIFO_DOWN 写操作（多 Worker 共享） |
| `accel_mutex` | 互斥量 | — | 保护 BatchNorm/Softmax 外设（riscv 独占） |

### 4.3 Worker 任务流程

```
worker_task[n]:
  loop:
    xQueueReceive(input_queue[n], &req, portMAX_DELAY)

    // 1. 初始化请求
    if req.type == INIT:
        xSemaphoreTake(noc_mutex, portMAX_DELAY)
        发送初始化帧（控制帧2型）到 FIFO_DOWN
        xSemaphoreGive(noc_mutex)
        xSemaphoreTake(done_sem[n], pdMS_TO_TICKS(INIT_TIMEOUT_MS))
        构造 InitResponse，xQueueSend(tx_queue)
        continue

    // 2. 推理请求
    xSemaphoreTake(noc_mutex, portMAX_DELAY)
    active_thread = n                          // ISR 用此值路由输出工作帧
    for each (t,c,h,w) in req.data:
        m = input_neuron_map[c*H*W + h*W + w]  // 查输入映射表
        构造工作帧（Core_XY/X/Y=m, AxonAddr=m.axon_addr, TS=t, Data=val）
        写 FIFO_DOWN
    发送同步帧（T = req.timesteps），写 FIFO_DOWN
    xSemaphoreGive(noc_mutex)

    // 3. 等待完成帧（带超时）
    result = xSemaphoreTake(done_sem[n], pdMS_TO_TICKS(INFER_TIMEOUT_MS))
    if result == pdFALSE:
        构造 InferResponse(success=false)，xQueueSend(tx_queue)
        continue

    // 4. 读取输出工作帧（已由 ISR 写入 output_ringbuf[n]）
    // CoreAddr 已被 NOC 清零，只用 axon_addr 查输出映射表
    从 output_ringbuf[n] 读取帧：
        axon = frame.axon_addr
        m = output_neuron_map[n][axon]         // 查输出映射表（按线程索引）
        if m.valid: output[m.c][m.h][m.w] = frame.data

    // 5. 加速外设后处理
    xSemaphoreTake(accel_mutex, portMAX_DELAY)
    调用 BatchNorm 外设
    调用 Softmax 外设
    xSemaphoreGive(accel_mutex)

    // 6. 返回结果
    构造 InferResponse(success=true, data=output, cycles=elapsed)
    xQueueSend(tx_queue, &resp, portMAX_DELAY)
```

### 4.4 SNN 中断处理

```c
// active_thread 由 worker_task 在发同步帧前设置（受 noc_mutex 保护）
extern volatile uint32_t active_thread;

void SNN_IRQHandler(void) {
    while (fifo_has_data()) {
        uint64_t frame = read_fifo_up();
        uint8_t header = (frame >> 62) & 0x3;

        if (header == 0b11) {  // 控制帧
            uint8_t type = (frame >> 60) & 0x3;
            if (type == 0b10) {  // 完成帧，LOAD[23:0] = 线程序号
                uint32_t thread_id = frame & 0xFFFFFF;
                BaseType_t woken = pdFALSE;
                xSemaphoreGiveFromISR(done_sem[thread_id], &woken);
                portYIELD_FROM_ISR(woken);
            }
        } else if (header == 0b10) {  // 工作帧（输出数据）
            // CoreAddr 已被 NOC 清零，通过 active_thread 确定所属线程
            ringbuf_push_from_isr(&output_ringbuf[active_thread], frame);
        }
    }
}
```

---

## 5. UART 帧边界协议

Protobuf 本身无帧边界，外层加长度前缀帧：

```
┌──────────────┬────────────┬──────────────────────┐
│ Magic 4B     │ Length 2B  │ Protobuf Payload N B  │
│ 0x50 41 49 43│ uint16 LE  │ HostMessage / Device  │
│ ('P','A','I','C') │      │ Message               │
└──────────────┴────────────┴──────────────────────┘
```

`uart_rx_task` 内部状态机：

```
WAIT_MAGIC → WAIT_LENGTH → RECV_PAYLOAD → DESERIALIZE → DISPATCH
```

---

## 6. 内存布局估算

| 区域 | 大小 |
|------|------|
| FreeRTOS heap（`configTOTAL_HEAP_SIZE`） | 32 KB |
| Protobuf 收发缓冲区（各 2 个，每个 2 KB） | 8 KB |
| Worker 输出环形缓冲区（每线程 4 KB） | 8 KB |
| 任务栈（5 个任务，合计） | 13 KB |
| 全局变量 / BSS | ~4 KB |
| **合计** | **~65 KB**（128 KB 片上 RAM 剩余约 63 KB） |

---

## 7. 目录结构规划

```
application/freertos/embedded_proto/
├── proto/
│   └── paicore.proto              ← 双端共享 schema
├── inc/
│   └── proto/
│       └── paicore.pb.h           ← protoc-gen-eams 生成，提交到仓库
├── src/
│   ├── main.c                     ← FreeRTOS 初始化，任务创建
│   ├── uart_rx_task.c             ← UART 接收 + Protobuf 反序列化
│   ├── uart_tx_task.c             ← Protobuf 序列化 + UART 发送
│   ├── dispatch_task.c            ← 消息路由
│   ├── worker_task.c              ← 推理流程（参数化线程号）
│   ├── snn_isr.c                  ← SNN 中断处理
│   ├── noc.c / noc.h              ← NOC FIFO 读写封装
│   ├── frame_builder.c            ← 工作帧/控制帧构造
│   ├── neuron_map.c               ← 映射表查询接口
│   └── accel.c / accel.h          ← BatchNorm/Softmax 外设驱动
├── data/
│   ├── config_frame.c             ← 烧录在 FLASH 的配置帧
│   └── neuron_map_data.c          ← 烧录在 FLASH 的映射表
└── Makefile
```

---

## 8. 无法上板时的测试策略

当前只有编译环境，无法上板或使用 QEMU，测试分三层进行：

### 8.1 编译验证（RISC-V 交叉编译）

确认全部源码能通过 `riscv64-unknown-elf-g++ -std=c++11` 编译链接，无 undefined symbol。

```bash
cd application/freertos/embedded_proto
make CORE=n307fd DOWNLOAD=ilmflashxip all
```

重点检查：
- EmbeddedProto src/ 与生成的 `.pb.h` 能正常编译
- FreeRTOS + C++ 混合编译无冲突
- 链接后 `.map` 文件中 RAM 用量在 128 KB 以内

### 8.2 Host 侧单元测试（Python）

用标准 `protobuf` 库验证 `.proto` schema 和帧边界协议：

```bash
cd application/freertos/embedded_proto/tools
python -m pytest test_host_protocol.py
```

测试内容：
- `paicore_pb2.py` 序列化/反序列化 round-trip
- 帧边界协议（Magic + Length + Payload）编解码
- 边界条件：空 data、最大 data、非法 magic

### 8.3 x86 本地单元测试（纯逻辑模块）

将不依赖硬件寄存器的模块抽出来，用 GCC/x86 编译成本地可执行文件：

```
可在 x86 上测试的模块（占代码量 ~70%）：
├── frame_builder.c    ← 工作帧/控制帧构造逻辑
├── neuron_map.c       ← [C,H,W] ↔ [CoreAddr,AxonAddr] 查表
├── uart 帧解析状态机  ← WAIT_MAGIC → RECV_PAYLOAD 状态转换
└── Protobuf 编解码    ← EmbeddedProto serialize/deserialize（bytes 字段）

无法在 x86 上测试（需上板）：
├── noc.c              ← FIFO_DOWN/FIFO_UP 硬件寄存器读写
├── snn_isr.c          ← SNN 中断处理
├── accel.c            ← BatchNorm/Softmax 外设
└── FreeRTOS 任务调度  ← 需要真实中断和定时器
```

x86 测试用 Makefile 单独维护，不影响 RISC-V 交叉编译。

---

## 9. Host 侧配套程序

### 9.1 文件路径

```
application/freertos/embedded_proto/tools/
├── host_demo.py           ← Host 端通信示例
├── test_host_protocol.py  ← Host 端单元测试
└── requirements.txt       ← pyserial, protobuf
```

### 9.2 host_demo.py 核心接口

```python
import serial
import struct
from paicore_pb2 import HostMessage, DeviceMessage

MAGIC = b'PAIC'
BAUDRATE = 3_000_000

class PaiCoreClient:
    def __init__(self, port: str):
        self.ser = serial.Serial(port, BAUDRATE, timeout=5)

    def _send(self, msg: HostMessage):
        payload = msg.SerializeToString()
        header = MAGIC + struct.pack('<H', len(payload))
        self.ser.write(header + payload)

    def _recv(self) -> DeviceMessage:
        # 等待 Magic
        while self.ser.read(4) != MAGIC:
            pass
        length = struct.unpack('<H', self.ser.read(2))[0]
        payload = self.ser.read(length)
        resp = DeviceMessage()
        resp.ParseFromString(payload)
        return resp

    def send_init(self, thread_id: int) -> bool:
        msg = HostMessage()
        msg.init_req.thread_id = thread_id
        self._send(msg)
        resp = self._recv()
        return resp.init_resp.success

    def send_infer(self, thread_id: int, timesteps: int,
                   data: bytes) -> tuple[bool, bytes]:
        msg = HostMessage()
        msg.infer_req.thread_id = thread_id
        msg.infer_req.timesteps = timesteps
        msg.infer_req.data = data
        self._send(msg)
        resp = self._recv()
        return resp.infer_resp.success, resp.infer_resp.data
```

### 9.3 Proto 生成命令

```bash
# Host 侧（Python）
protoc --python_out=application/freertos/embedded_proto/tools \
       application/freertos/embedded_proto/proto/paicore.proto

# RISC-V 侧（EmbeddedProto C++）
cd third_party/EmbeddedProto
protoc --plugin=protoc-gen-eams=$(pwd)/protoc-gen-eams \
       --eams_out=../../application/freertos/embedded_proto/inc/proto \
       ../../application/freertos/embedded_proto/proto/paicore.proto
```

---

## 10. Worker 职责说明

两个 Worker 任务足以处理所有 host 请求。每个 Worker 通过 `oneof` 消息类型同时承担初始化和推理职责：

```
Host 发 InitRequest(thread_id=0)  → dispatch → worker[0] 发初始化帧 + 等完成帧
Host 发 InitRequest(thread_id=1)  → dispatch → worker[1] 发初始化帧 + 等完成帧
Host 发 InferRequest(thread_id=0) → dispatch → worker[0] 走推理流程
Host 发 InferRequest(thread_id=1) → dispatch → worker[1] 走推理流程
```

不需要额外的"控制线程"。如果 host 只用一个线程推理，另一个 Worker 空闲等待，无额外开销。

---

## 11. 实现顺序建议

1. **EmbeddedProto 编译验证**：先写一个最小 C++ 测试文件，确认 `-std=c++11` 下能编译 `paicore.pb.h`
2. **NOC 封装层**：`noc.c` 封装 FIFO 读写，`frame_builder.c` 封装帧构造，x86 单元测试
3. **裸机推理流程验证**：在现有 uart 工程基础上，先跑通单线程初始化→推理→完成帧的完整流程
4. **引入 FreeRTOS**：迁移到任务架构，先单 Worker，验证 ISR→信号量→任务的通路
5. **Protobuf 通信**：接入 UART 帧边界协议和 Protobuf 序列化/反序列化，配合 host_demo.py 联调
6. **多线程扩展**：增加第二个 Worker，验证 noc_mutex 和 done_sem 的正确性
7. **加速外设接入**：BatchNorm/Softmax 外设驱动和 accel_mutex 调度
