# PaiCore Config/Test Frame Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add config_task to the FreeRTOS embedded_proto app so host can request RV to send configuration frames to PaiCore and verify them via test frames, with full FIFO exclusivity during config/test.

**Architecture:** A new `config_task` (prio=3, same as uart_rx/tx) receives `ConfigRequest`/`TestRequest` protobuf messages via a dedicated `config_queue`. It takes `accel_mutex` to block all workers, disables SNN IRQ, then directly drives FIFO_DOWN (config) and polls FIFO_UP (test response). Config metadata (Core address, subtype, SRAM addr, data pointer, data count) lives in `data/config_meta.c`. Test frame verification is full-quantity: every response data frame is compared byte-for-byte against the flash data.

**Tech Stack:** C (FreeRTOS), C++ (EmbeddedProto), protobuf3, x86 gcc for unit tests

---

## File Map

| File | Action | Responsibility |
|------|--------|----------------|
| `proto/paicore.proto` | Modify | Add ConfigRequest, TestRequest, ConfigResponse, TestResponse messages |
| `inc/proto/paicore.h` | Modify | Regenerated from proto (manual edit to add new message classes) |
| `data/config_meta.c` | Create | Config block descriptors: subtype, core coords, sram_addr, data pointer, data_words |
| `inc/config_meta.h` | Create | `config_block_t` struct and `g_config_blocks[]` extern declaration |
| `inc/worker_task.h` | Modify | Add `config_queue` extern, `config_msg_t` struct, `config_task` declaration |
| `src/config_task.c` | Create | config_task: send config frames, send test request, verify test response |
| `src/main.c` | Modify | Create config_queue, create config_task |
| `src/uart_rx_task.cpp` | Modify | Decode ConfigRequest/TestRequest and route to config_queue |
| `src/uart_tx_task.cpp` | Modify | Encode ConfigResponse/TestResponse from tx_queue |
| `test/test_config_frame.c` | Create | x86 unit tests for config frame header construction and test response parsing |
| `test/Makefile` | Modify | Add test_config_frame target |
| `tools/test_host_protocol.py` | Modify | Add pytest tests for ConfigRequest/TestRequest/ConfigResponse/TestResponse round-trips |

---

## Task 1: Extend protobuf schema and regenerate header

**Files:**
- Modify: `proto/paicore.proto`
- Modify: `inc/proto/paicore.h` (manual addition — no protoc toolchain required)

The EmbeddedProto generated header `inc/proto/paicore.h` is large and templated. Rather than running protoc (which requires the EmbeddedProto plugin), we manually add the new message classes following the exact same pattern as the existing ones.

- [ ] **Step 1: Add new messages to paicore.proto**

Edit `proto/paicore.proto` to append:

```protobuf
message ConfigRequest {
  uint32 block_index = 1;   // index into g_config_blocks[]
}

message TestRequest {
  uint32 block_index = 1;   // index into g_config_blocks[]
}

message ConfigResponse {
  uint32 block_index = 1;
  bool   success     = 2;
}

message TestResponse {
  uint32 block_index = 1;
  bool   success     = 2;   // false = mismatch or timeout
  uint32 mismatch_frame = 3; // index of first mismatched frame (0 if success)
}
```

Also extend `HostMessage` oneof to add `config_req` and `test_req`, and `DeviceMessage` oneof to add `config_resp` and `test_resp`:

```protobuf
message HostMessage {
  oneof payload {
    InitRequest   init_req   = 1;
    InferRequest  infer_req  = 2;
    ConfigRequest config_req = 3;
    TestRequest   test_req   = 4;
  }
}

message DeviceMessage {
  oneof payload {
    InitResponse   init_resp   = 1;
    InferResponse  infer_resp  = 2;
    ConfigResponse config_resp = 3;
    TestResponse   test_resp   = 4;
  }
}
```

- [ ] **Step 2: Add ConfigRequest class to inc/proto/paicore.h**

Open `inc/proto/paicore.h`. Find the `InferRequest` class as a template. Add `ConfigRequest` and `TestRequest` classes immediately after `InferRequest`. They follow the same pattern but only have a single `uint32` field `block_index`:

```cpp
template<uint32_t block_index_LENGTH = 0>
class ConfigRequest final : public EmbeddedProto::MessageInterface {
  public:
    ConfigRequest() = default;
    ~ConfigRequest() override = default;

    enum class id : uint32_t { NOT_SET = 0, BLOCK_INDEX = 1 };

    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const EmbeddedProto::uint32& v) { block_index_ = v; }
    inline const EmbeddedProto::uint32& get_block_index() const { return block_index_; }
    inline EmbeddedProto::uint32::FIELD_TYPE block_index() const { return block_index_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(id::BLOCK_INDEX), buffer, false);
      return return_value;
    }

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type;
      uint32_t id_number = 0;
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (0 < buffer.get_size())) {
        return_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        if(::EmbeddedProto::Error::NO_ERRORS == return_value) {
          switch(id_number) {
            case static_cast<uint32_t>(id::BLOCK_INDEX):
              return_value = block_index_.deserialize_check_type(buffer, wire_type); break;
            default: return_value = skip_unknown_field(buffer, wire_type); break;
          }
        }
      }
      return return_value;
    }

    void clear() override { clear_block_index(); }

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
};

template<uint32_t block_index_LENGTH = 0>
class TestRequest final : public EmbeddedProto::MessageInterface {
  public:
    TestRequest() = default;
    ~TestRequest() override = default;

    enum class id : uint32_t { NOT_SET = 0, BLOCK_INDEX = 1 };

    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const EmbeddedProto::uint32& v) { block_index_ = v; }
    inline const EmbeddedProto::uint32& get_block_index() const { return block_index_; }
    inline EmbeddedProto::uint32::FIELD_TYPE block_index() const { return block_index_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(id::BLOCK_INDEX), buffer, false);
      return return_value;
    }

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type;
      uint32_t id_number = 0;
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (0 < buffer.get_size())) {
        return_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        if(::EmbeddedProto::Error::NO_ERRORS == return_value) {
          switch(id_number) {
            case static_cast<uint32_t>(id::BLOCK_INDEX):
              return_value = block_index_.deserialize_check_type(buffer, wire_type); break;
            default: return_value = skip_unknown_field(buffer, wire_type); break;
          }
        }
      }
      return return_value;
    }

    void clear() override { clear_block_index(); }

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
};
```

- [ ] **Step 3: Add ConfigResponse and TestResponse classes to inc/proto/paicore.h**

Add after `InferResponse`. `ConfigResponse` has `block_index` (uint32) and `success` (bool). `TestResponse` has `block_index` (uint32), `success` (bool), `mismatch_frame` (uint32). Follow the exact same serialize/deserialize pattern as `InitResponse`.

- [ ] **Step 4: Extend HostMessage and DeviceMessage in inc/proto/paicore.h**

In the `HostMessage` class, add field IDs 3 and 4 to the `id` enum, add `has_config_req()`, `get_config_req()`, `has_test_req()`, `get_test_req()` accessors, and handle field IDs 3 and 4 in `deserialize`. In `DeviceMessage`, add `mutable_config_resp()`, `mutable_test_resp()` and handle serialization of fields 3 and 4.

- [ ] **Step 5: Commit**

```bash
git add proto/paicore.proto inc/proto/paicore.h
git commit -m "feat(proto): add ConfigRequest/TestRequest/ConfigResponse/TestResponse messages"
```

---

## Task 2: config_meta header and stub data

**Files:**
- Create: `inc/config_meta.h`
- Create: `data/config_meta.c`

- [ ] **Step 1: Write failing x86 test**

Create `test/test_config_frame.c`:

```c
// test/test_config_frame.c
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "config_meta.h"

static void test_config_meta_count(void) {
    assert(CONFIG_BLOCK_COUNT > 0);
    printf("test_config_meta_count PASSED\n");
}

static void test_config_meta_fields(void) {
    const config_block_t *b = &g_config_blocks[0];
    // subtype must be 0-3
    assert(b->subtype <= 3);
    // data must not be NULL
    assert(b->data != NULL);
    // data_words must be even (pairs of 32-bit = 64-bit frames)
    assert(b->data_words % 2 == 0);
    // data_words / 2 must match expected frame count for subtype
    uint32_t n = b->data_words / 2;
    if (b->subtype == 0) assert(n == 3);       // Core params: 3 frames
    if (b->subtype == 1) assert(n == 256);     // LUT: 256 frames
    if (b->subtype == 2) assert(n == 8192);    // Neuron: 8192 frames
    if (b->subtype == 3) assert(n == 2048);    // Input: 2048 frames
    printf("test_config_meta_fields PASSED\n");
}

int main(void) {
    test_config_meta_count();
    test_config_meta_fields();
    printf("ALL config_frame tests PASSED\n");
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails**

```bash
cd test && make test_config_frame 2>&1 | head -5
```
Expected: compile error — `config_meta.h` not found.

- [ ] **Step 3: Create inc/config_meta.h**

```c
// inc/config_meta.h
#ifndef CONFIG_META_H
#define CONFIG_META_H

#include <stdint.h>

// Describes one configuration block stored in flash.
// RV constructs the 64-bit header frame from these fields,
// then streams data[0..data_words-1] as pairs of 32-bit words.
typedef struct {
    uint8_t         subtype;     // 0=Core(1型) 1=LUT(2型) 2=Neuron(3型) 3=Input(4型)
    uint8_t         core_xy;     // [5:0] Core_XY
    uint8_t         core_x;     // [5:0] Core_X
    uint8_t         core_y;     // [5:0] Core_Y
    uint16_t        sram_addr;  // raw value for header [22:14] (pre-divided for subtype 2)
    const uint32_t *data;       // pointer to 32-bit word pairs in flash
    uint32_t        data_words; // number of 32-bit words (must be even; frames = data_words/2)
} config_block_t;

extern const config_block_t g_config_blocks[];
extern const uint32_t CONFIG_BLOCK_COUNT;

#endif // CONFIG_META_H
```

- [ ] **Step 4: Create data/config_meta.c with one stub block**

```c
// data/config_meta.c
// Config block metadata — generated by host toolchain, one entry per Core configuration.
// Replace stub data with actual toolchain output before hardware bring-up.
#include "config_meta.h"

// Stub: 3-frame Core params block for Core (0,0,0), all-zero payload
static const uint32_t _stub_core_data[6] = {
    0, 0,  // frame 0: low word, high word
    0, 0,  // frame 1
    0, 0,  // frame 2
};

const config_block_t g_config_blocks[] = {
    {
        .subtype    = 0,          // Core params (1型)
        .core_xy    = 0,
        .core_x     = 0,
        .core_y     = 0,
        .sram_addr  = 0,          // reserved for subtype 0
        .data       = _stub_core_data,
        .data_words = 6,          // 3 frames × 2 words
    },
};

const uint32_t CONFIG_BLOCK_COUNT = sizeof(g_config_blocks) / sizeof(g_config_blocks[0]);
```

- [ ] **Step 5: Add test_config_frame to test/Makefile**

In `test/Makefile`, add to `TESTS`:
```makefile
TESTS = test_ringbuf test_frame_builder test_neuron_map test_uart_frame test_config_frame
```
Add build rule:
```makefile
test_config_frame: test_config_frame.c ../data/config_meta.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
```

- [ ] **Step 6: Run test to verify it passes**

```bash
cd test && make test_config_frame && ./test_config_frame
```
Expected:
```
test_config_meta_count PASSED
test_config_meta_fields PASSED
ALL config_frame tests PASSED
```

- [ ] **Step 7: Commit**

```bash
git add inc/config_meta.h data/config_meta.c test/test_config_frame.c test/Makefile
git commit -m "feat(config): add config_meta header, stub data block, and x86 unit test"
```

---

## Task 3: config_msg_t, config_queue, and config_task skeleton

**Files:**
- Modify: `inc/worker_task.h`
- Create: `src/config_task.c`
- Modify: `src/main.c`

- [ ] **Step 1: Add config_msg_t and config_queue to worker_task.h**

In `inc/worker_task.h`, after the `tx_msg_t` definition, add:

```c
// Message types for config_queue
typedef enum {
    MSG_CONFIG_REQ = 0,
    MSG_TEST_REQ   = 1,
} config_msg_type_t;

// Message passed from uart_rx_task to config_task
typedef struct {
    config_msg_type_t type;
    uint32_t          block_index;
} config_msg_t;

// Response passed from config_task to uart_tx_task
typedef struct {
    config_msg_type_t type;
    uint32_t          block_index;
    uint8_t           success;
    uint32_t          mismatch_frame; // first mismatched frame index (0 if success)
} config_resp_t;

extern QueueHandle_t config_queue;
```

Also add `config_task` to the task entry point declarations:
```c
void config_task(void *param);
```

- [ ] **Step 2: Create src/config_task.c skeleton**

```c
// src/config_task.c
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "worker_task.h"
#include "config_meta.h"
#include "noc.h"
#include "paicore_config.h"

#define CONFIG_TEST_TIMEOUT_MS  10000

// Build the 64-bit config/test header frame.
// frame_type: 0=config, 1=test_request
// Returns frame64_t with [63:62]=frame_type, [61:60]=subtype,
// [59:54]=core_xy, [53:48]=core_x, [47:42]=core_y,
// Copy fields all zero (test requests never multicast),
// [23]=is_test_req, [22:14]=sram_addr, [13:0]=n_frames
static frame64_t build_cfg_header(uint8_t frame_type, const config_block_t *b,
                                   uint8_t is_test_req, uint32_t n_frames)
{
    frame64_t f;
    uint32_t n = n_frames & 0x3FFF;
    uint32_t sram = (uint32_t)(b->sram_addr & 0x1FF);

    // high word: [31:30]=frame_type [29:28]=subtype [27:22]=core_xy
    //            [21:16]=core_x [15:10]=core_y [9:4]=copy_xy=0 [3:0]=copy_x[5:2]=0
    f.high = ((uint32_t)frame_type  << 30)
           | ((uint32_t)b->subtype  << 28)
           | ((uint32_t)(b->core_xy & 0x3F) << 22)
           | ((uint32_t)(b->core_x  & 0x3F) << 16)
           | ((uint32_t)(b->core_y  & 0x3F) << 10);
    // copy_xy, copy_x, copy_y all zero

    // low word: [31:30]=copy_x[1:0]=0 [29:24]=copy_y=0
    //           [23]=is_test_req [22:14]=sram_addr [13:0]=n_frames
    f.low = ((uint32_t)is_test_req << 23)
          | (sram << 14)
          | n;
    return f;
}

static void send_config(const config_block_t *b, config_resp_t *resp)
{
    uint32_t n_frames = b->data_words / 2;
    frame64_t hdr = build_cfg_header(0, b, 0, n_frames);
    noc_write_down(hdr);
    for (uint32_t i = 0; i < b->data_words; i += 2) {
        frame64_t f;
        f.high = b->data[i + 1];
        f.low  = b->data[i];
        noc_write_down(f);
    }
    resp->success = 1;
    resp->mismatch_frame = 0;
}

static void send_test_and_verify(const config_block_t *b, config_resp_t *resp)
{
    uint32_t n_frames = b->data_words / 2;

    // Send test request frame (Copy fields forced to 0)
    frame64_t req_hdr = build_cfg_header(1, b, 1, n_frames);
    noc_write_down(req_hdr);

    // Poll FIFO_UP for test response header
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(CONFIG_TEST_TIMEOUT_MS);
    while (!noc_up_has_data()) {
        if (xTaskGetTickCount() >= deadline) {
            resp->success = 0;
            resp->mismatch_frame = 0;
            return;
        }
        vTaskDelay(1);
    }

    // Read response header frame
    frame64_t resp_hdr;
    if (noc_read_up(&resp_hdr) != 0) {
        resp->success = 0;
        resp->mismatch_frame = 0;
        return;
    }

    // Validate: [63:62] must be 0b01 (test frame), [23] must be 0 (response)
    uint8_t ftype = (uint8_t)((resp_hdr.high >> 30) & 0x3);
    uint8_t is_resp = (uint8_t)((resp_hdr.low >> 23) & 0x1);
    uint32_t resp_n = resp_hdr.low & 0x3FFF;
    if (ftype != 1 || is_resp != 0 || resp_n != n_frames) {
        resp->success = 0;
        resp->mismatch_frame = 0;
        return;
    }

    // Read and compare N data frames
    for (uint32_t i = 0; i < n_frames; i++) {
        deadline = xTaskGetTickCount() + pdMS_TO_TICKS(CONFIG_TEST_TIMEOUT_MS);
        while (!noc_up_has_data()) {
            if (xTaskGetTickCount() >= deadline) {
                resp->success = 0;
                resp->mismatch_frame = i;
                return;
            }
            vTaskDelay(1);
        }
        frame64_t df;
        if (noc_read_up(&df) != 0) {
            resp->success = 0;
            resp->mismatch_frame = i;
            return;
        }
        uint32_t exp_low  = b->data[i * 2];
        uint32_t exp_high = b->data[i * 2 + 1];
        if (df.low != exp_low || df.high != exp_high) {
            resp->success = 0;
            resp->mismatch_frame = i;
            return;
        }
    }
    resp->success = 1;
    resp->mismatch_frame = 0;
}

void config_task(void *param)
{
    (void)param;
    config_msg_t msg;

    for (;;) {
        if (xQueueReceive(config_queue, &msg, portMAX_DELAY) != pdTRUE)
            continue;

        if (msg.block_index >= CONFIG_BLOCK_COUNT) {
            // Invalid block index — send failure response
            config_resp_t resp = {
                .type           = msg.type,
                .block_index    = msg.block_index,
                .success        = 0,
                .mismatch_frame = 0,
            };
            xQueueSend(tx_queue, &resp, portMAX_DELAY);
            continue;
        }

        const config_block_t *b = &g_config_blocks[msg.block_index];

        // Take accel_mutex to block all workers and serialize FIFO access
        xSemaphoreTake(accel_mutex, portMAX_DELAY);
        // Disable SNN IRQ so ISR doesn't consume FIFO_UP frames
        noc_irq_disable();

        config_resp_t resp;
        resp.type        = msg.type;
        resp.block_index = msg.block_index;

        if (msg.type == MSG_CONFIG_REQ) {
            send_config(b, &resp);
        } else {
            send_test_and_verify(b, &resp);
        }

        noc_irq_enable();
        xSemaphoreGive(accel_mutex);

        xQueueSend(tx_queue, &resp, portMAX_DELAY);
    }
}

- [ ] **Step 3: Add config_queue to main.c**

In `src/main.c`, add `QueueHandle_t config_queue;` to the global FreeRTOS objects section (alongside `dispatch_queue`, `tx_queue`, etc.).

In `main()`, after creating `tx_queue`, add:
```c
config_queue = xQueueCreate(2, sizeof(config_msg_t));
```

Add task creation after the worker tasks:
```c
xTaskCreate(config_task, "config", 768, NULL, 3, NULL);
```

- [ ] **Step 4: Commit**

```bash
git add inc/worker_task.h src/config_task.c src/main.c
git commit -m "feat(config): add config_task skeleton with config/test frame send and verify"
```

---

## Task 4: Wire uart_rx_task and uart_tx_task for config/test messages

**Files:**
- Modify: `src/uart_rx_task.cpp`
- Modify: `src/uart_tx_task.cpp`

- [ ] **Step 1: Route ConfigRequest/TestRequest in uart_rx_task.cpp**

In `uart_rx_task.cpp`, after the `else if (hmsg.has_infer_req())` block, add:

```cpp
} else if (hmsg.has_config_req()) {
    config_msg_t cmsg;
    cmsg.type        = MSG_CONFIG_REQ;
    cmsg.block_index = hmsg.get_config_req().get_block_index();
    xQueueSend(config_queue, &cmsg, portMAX_DELAY);
    continue;
} else if (hmsg.has_test_req()) {
    config_msg_t cmsg;
    cmsg.type        = MSG_TEST_REQ;
    cmsg.block_index = hmsg.get_test_req().get_block_index();
    xQueueSend(config_queue, &cmsg, portMAX_DELAY);
    continue;
```

Also add `#include "config_meta.h"` at the top of the file (needed for `config_msg_t` via `worker_task.h`).

Note: `config_msg_t` is defined in `worker_task.h` which is already included. `config_queue` is declared there too.

- [ ] **Step 2: Encode ConfigResponse/TestResponse in uart_tx_task.cpp**

In `uart_tx_task.cpp`, the `tx_queue` currently carries `tx_msg_t`. We need to extend it to also carry `config_resp_t`. The cleanest approach is to use a tagged union in a new `tx_envelope_t` type.

Add to `inc/worker_task.h` (after `config_resp_t`):

```c
typedef enum {
    TX_INFER_RESP  = 0,
    TX_CONFIG_RESP = 1,
    TX_TEST_RESP   = 2,
} tx_envelope_type_t;

typedef struct {
    tx_envelope_type_t env_type;
    union {
        tx_msg_t      infer;
        config_resp_t config;
    } u;
} tx_envelope_t;
```

Change `tx_queue` element size in `main.c` from `sizeof(tx_msg_t)` to `sizeof(tx_envelope_t)`.

Update `worker_task.c` to wrap `tx_msg_t` in `tx_envelope_t` before sending:
```c
tx_envelope_t env;
env.env_type = TX_INFER_RESP;
env.u.infer  = resp;
xQueueSend(tx_queue, &env, portMAX_DELAY);
```
(Do this for both `handle_init` and `handle_infer`.)

Update `config_task.c` to wrap `config_resp_t`:
```c
tx_envelope_t env;
env.env_type = (msg.type == MSG_CONFIG_REQ) ? TX_CONFIG_RESP : TX_TEST_RESP;
env.u.config = resp;
xQueueSend(tx_queue, &env, portMAX_DELAY);
```

Update `uart_tx_task.cpp` to receive `tx_envelope_t` and dispatch:
```cpp
tx_envelope_t env;
if (xQueueReceive(tx_queue, &env, portMAX_DELAY) != pdTRUE) continue;

dmsg.clear();
wbuf.clear();

if (env.env_type == TX_INFER_RESP) {
    const tx_msg_t &tmsg = env.u.infer;
    if (tmsg.is_init_resp) {
        dmsg.mutable_init_resp().set_thread_id(tmsg.thread_id);
        dmsg.mutable_init_resp().set_success(tmsg.success != 0);
    } else {
        dmsg.mutable_infer_resp().set_thread_id(tmsg.thread_id);
        dmsg.mutable_infer_resp().set_success(tmsg.success != 0);
        dmsg.mutable_infer_resp().set_cycles(tmsg.cycles);
        if (tmsg.data_len > 0)
            dmsg.mutable_infer_resp().mutable_data().set(tmsg.data, tmsg.data_len);
    }
} else if (env.env_type == TX_CONFIG_RESP) {
    const config_resp_t &cr = env.u.config;
    dmsg.mutable_config_resp().set_block_index(cr.block_index);
    dmsg.mutable_config_resp().set_success(cr.success != 0);
} else { // TX_TEST_RESP
    const config_resp_t &cr = env.u.config;
    dmsg.mutable_test_resp().set_block_index(cr.block_index);
    dmsg.mutable_test_resp().set_success(cr.success != 0);
    dmsg.mutable_test_resp().set_mismatch_frame(cr.mismatch_frame);
}
```

- [ ] **Step 3: Commit**

```bash
git add inc/worker_task.h src/uart_rx_task.cpp src/uart_tx_task.cpp src/worker_task.c src/config_task.c src/main.c
git commit -m "feat(config): wire config/test messages through uart_rx, tx_envelope, uart_tx"
```

---

## Task 5: x86 unit tests for config frame header and test response parsing

**Files:**
- Modify: `test/test_config_frame.c`

Add tests that exercise `build_cfg_header` logic (extracted to a testable form) and the test response header validation logic.

Since `build_cfg_header` is a static function in `config_task.c`, extract it to a new inline helper in `inc/config_meta.h` so it can be tested without FreeRTOS.

- [ ] **Step 1: Move build_cfg_header to inc/config_meta.h as inline**

Add to `inc/config_meta.h` (after the struct definition):

```c
#include "noc.h"  // for frame64_t

// Build the 64-bit config/test header frame.
// frame_type: 0=config, 1=test_request
// is_test_req: 1 for RV request, 0 for config
// Copy fields are always zero (test requests never multicast).
static inline frame64_t config_build_header(uint8_t frame_type,
                                             const config_block_t *b,
                                             uint8_t is_test_req,
                                             uint32_t n_frames)
{
    frame64_t f;
    uint32_t n    = n_frames & 0x3FFF;
    uint32_t sram = (uint32_t)(b->sram_addr & 0x1FF);
    f.high = ((uint32_t)frame_type        << 30)
           | ((uint32_t)(b->subtype & 0x3) << 28)
           | ((uint32_t)(b->core_xy & 0x3F) << 22)
           | ((uint32_t)(b->core_x  & 0x3F) << 16)
           | ((uint32_t)(b->core_y  & 0x3F) << 10);
    f.low  = ((uint32_t)is_test_req << 23)
           | (sram << 14)
           | n;
    return f;
}
```

Remove the static `build_cfg_header` from `src/config_task.c` and replace its call with `config_build_header(...)`.

- [ ] **Step 2: Write failing tests**

Replace the content of `test/test_config_frame.c` with:

```c
// test/test_config_frame.c
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "config_meta.h"

static void test_config_meta_count(void) {
    assert(CONFIG_BLOCK_COUNT > 0);
    printf("test_config_meta_count PASSED\n");
}

static void test_config_meta_fields(void) {
    const config_block_t *b = &g_config_blocks[0];
    assert(b->subtype <= 3);
    assert(b->data != NULL);
    assert(b->data_words % 2 == 0);
    uint32_t n = b->data_words / 2;
    if (b->subtype == 0) assert(n == 3);
    if (b->subtype == 1) assert(n == 256);
    if (b->subtype == 2) assert(n == 8192);
    if (b->subtype == 3) assert(n == 2048);
    printf("test_config_meta_fields PASSED\n");
}

static void test_config_header_frame_type(void) {
    config_block_t b = { .subtype=0, .core_xy=0, .core_x=0, .core_y=0,
                         .sram_addr=0, .data=NULL, .data_words=6 };
    frame64_t f = config_build_header(0, &b, 0, 3);
    // [63:62] = 0b00 (config frame)
    assert(((f.high >> 30) & 0x3) == 0);
    // [23] = 0 (not test request)
    assert(((f.low >> 23) & 0x1) == 0);
    // [13:0] = 3
    assert((f.low & 0x3FFF) == 3);
    printf("test_config_header_frame_type PASSED\n");
}

static void test_test_request_header(void) {
    config_block_t b = { .subtype=2, .core_xy=0x3F, .core_x=0x1, .core_y=0x2,
                         .sram_addr=0x10, .data=NULL, .data_words=16384 };
    frame64_t f = config_build_header(1, &b, 1, 8192);
    // [63:62] = 0b01 (test frame)
    assert(((f.high >> 30) & 0x3) == 1);
    // [61:60] = 0b10 (subtype 2 = Neuron)
    assert(((f.high >> 28) & 0x3) == 2);
    // [59:54] = core_xy = 0x3F
    assert(((f.high >> 22) & 0x3F) == 0x3F);
    // [53:48] = core_x = 0x1
    assert(((f.high >> 16) & 0x3F) == 0x1);
    // [47:42] = core_y = 0x2
    assert(((f.high >> 10) & 0x3F) == 0x2);
    // Copy fields all zero
    assert((f.high & 0x3FF) == 0);
    assert(((f.low >> 30) & 0x3) == 0);
    assert(((f.low >> 24) & 0x3F) == 0);
    // [23] = 1 (test request)
    assert(((f.low >> 23) & 0x1) == 1);
    // [22:14] = sram_addr = 0x10
    assert(((f.low >> 14) & 0x1FF) == 0x10);
    // [13:0] = 8192
    assert((f.low & 0x3FFF) == 8192);
    printf("test_test_request_header PASSED\n");
}

static void test_config_header_copy_fields_zero(void) {
    // Even if we had copy fields, test requests must zero them
    config_block_t b = { .subtype=1, .core_xy=5, .core_x=3, .core_y=7,
                         .sram_addr=0, .data=NULL, .data_words=512 };
    frame64_t f = config_build_header(1, &b, 1, 256);
    // copy_xy [41:36] = high[9:4] = 0
    assert(((f.high >> 4) & 0x3F) == 0);
    // copy_x [35:32] = high[3:0] = 0
    assert((f.high & 0xF) == 0);
    // copy_x [31:30] = low[31:30] = 0
    assert(((f.low >> 30) & 0x3) == 0);
    // copy_y [29:24] = low[29:24] = 0
    assert(((f.low >> 24) & 0x3F) == 0);
    printf("test_config_header_copy_fields_zero PASSED\n");
}

int main(void) {
    test_config_meta_count();
    test_config_meta_fields();
    test_config_header_frame_type();
    test_test_request_header();
    test_config_header_copy_fields_zero();
    printf("ALL config_frame tests PASSED\n");
    return 0;
}
```

- [ ] **Step 3: Run test to verify it passes**

```bash
cd test && make test_config_frame && ./test_config_frame
```
Expected:
```
test_config_meta_count PASSED
test_config_meta_fields PASSED
test_config_header_frame_type PASSED
test_test_request_header PASSED
test_config_header_copy_fields_zero PASSED
ALL config_frame tests PASSED
```

- [ ] **Step 4: Run full test suite**

```bash
cd test && make run
```
Expected: ALL TESTS PASSED

- [ ] **Step 5: Commit**

```bash
git add inc/config_meta.h src/config_task.c test/test_config_frame.c
git commit -m "test(config): add x86 unit tests for config/test header frame construction"
```

---

## Task 6: Python host-side tests for new protobuf messages

**Files:**
- Modify: `tools/test_host_protocol.py`

Add pytest tests for ConfigRequest, TestRequest, ConfigResponse, TestResponse round-trips and UART frame encoding. These tests run without hardware (same pattern as existing tests 1-12).

- [ ] **Step 1: Add tests to test_host_protocol.py**

Append to `tools/test_host_protocol.py`:

```python
# ---------------------------------------------------------------------------
# Test 13 — ConfigRequest round-trip
# ---------------------------------------------------------------------------

def test_config_request_roundtrip():
    msg = HostMessage()
    msg.config_req.block_index = 2
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("config_req")
    assert decoded.config_req.block_index == 2


# ---------------------------------------------------------------------------
# Test 14 — TestRequest round-trip
# ---------------------------------------------------------------------------

def test_test_request_roundtrip():
    msg = HostMessage()
    msg.test_req.block_index = 0
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_req")
    assert decoded.test_req.block_index == 0


# ---------------------------------------------------------------------------
# Test 15 — ConfigResponse round-trip
# ---------------------------------------------------------------------------

def test_config_response_roundtrip():
    msg = DeviceMessage()
    msg.config_resp.block_index = 1
    msg.config_resp.success = True
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("config_resp")
    assert decoded.config_resp.block_index == 1
    assert decoded.config_resp.success is True


# ---------------------------------------------------------------------------
# Test 16 — TestResponse success round-trip
# ---------------------------------------------------------------------------

def test_test_response_success_roundtrip():
    msg = DeviceMessage()
    msg.test_resp.block_index = 0
    msg.test_resp.success = True
    msg.test_resp.mismatch_frame = 0
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_resp")
    assert decoded.test_resp.success is True
    assert decoded.test_resp.mismatch_frame == 0


# ---------------------------------------------------------------------------
# Test 17 — TestResponse failure carries mismatch_frame
# ---------------------------------------------------------------------------

def test_test_response_failure_roundtrip():
    msg = DeviceMessage()
    msg.test_resp.block_index = 3
    msg.test_resp.success = False
    msg.test_resp.mismatch_frame = 512
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_resp")
    assert decoded.test_resp.success is False
    assert decoded.test_resp.mismatch_frame == 512


# ---------------------------------------------------------------------------
# Test 18 — HostMessage oneof: config_req clears init_req
# ---------------------------------------------------------------------------

def test_host_message_oneof_config_clears_init():
    msg = HostMessage()
    msg.init_req.thread_id = 1
    assert msg.HasField("init_req")

    msg.config_req.block_index = 0
    assert msg.HasField("config_req")
    assert not msg.HasField("init_req")


# ---------------------------------------------------------------------------
# Test 19 — ConfigRequest framed end-to-end
# ---------------------------------------------------------------------------

def test_full_frame_config_request():
    msg = HostMessage()
    msg.config_req.block_index = 7
    payload = msg.SerializeToString()
    frame = encode_frame(payload)

    raw = decode_frame(frame)
    decoded = HostMessage()
    decoded.ParseFromString(raw)

    assert decoded.config_req.block_index == 7
```

- [ ] **Step 2: Regenerate paicore_pb2.py**

The Python protobuf file must be regenerated to include the new messages:
```bash
cd tools && protoc --python_out=. ../proto/paicore.proto
```
If protoc is not available, skip this step and the tests will be auto-skipped by conftest.py.

- [ ] **Step 3: Run tests**

```bash
cd tools && python -m pytest test_host_protocol.py -v
```
Expected: all 19 tests pass (or tests 13-19 skipped if paicore_pb2 not regenerated yet).

- [ ] **Step 4: Commit**

```bash
git add tools/test_host_protocol.py tools/paicore_pb2.py
git commit -m "test(host): add pytest tests for ConfigRequest/TestRequest/ConfigResponse/TestResponse"
```

---

## Self-Review

**Spec coverage check:**

| Requirement | Task |
|-------------|------|
| host requests RV send config frames | Task 3 (config_task MSG_CONFIG_REQ), Task 4 (uart_rx routing) |
| RV constructs header frame from metadata | Task 3 (build_cfg_header / config_build_header) |
| RV streams flash data frames | Task 3 (send_config loop) |
| host requests RV test | Task 3 (MSG_TEST_REQ), Task 4 |
| RV sends test request frame with Copy=0 | Task 3 (send_test_and_verify, copy fields zero) |
| RV polls FIFO_UP for test response | Task 3 (send_test_and_verify poll loop) |
| Full-quantity verification | Task 3 (compare every frame) |
| 10s timeout | Task 3 (#define CONFIG_TEST_TIMEOUT_MS 10000) |
| accel_mutex exclusivity | Task 3 (xSemaphoreTake(accel_mutex)) |
| SNN IRQ disabled during config/test | Task 3 (noc_irq_disable/enable) |
| config_resp/test_resp to host | Task 4 (uart_tx_task TX_CONFIG_RESP/TX_TEST_RESP) |
| config_meta.h struct | Task 2 |
| stub data block | Task 2 |
| x86 unit tests for header construction | Task 5 |
| Python host tests for new messages | Task 6 |
| tx_envelope_t to unify tx_queue | Task 4 |

**Placeholder scan:** No TBD/TODO/placeholder steps found.

**Type consistency:**
- `config_msg_t` defined in Task 3, used in Tasks 3, 4 ✓
- `config_resp_t` defined in Task 3, used in Tasks 3, 4 ✓
- `tx_envelope_t` defined in Task 4, used in Tasks 3, 4 ✓
- `config_build_header` defined in Task 5 (moved from Task 3), used in Task 5 tests ✓
- Note: Task 3 introduces `build_cfg_header` as static; Task 5 moves it to `config_build_header` inline in header. The implementer must apply Task 5's refactor when implementing Task 3, or implement Task 3 with the inline version from the start.

**Ordering note:** Tasks 3 and 5 are coupled — the implementer should implement `config_build_header` as the inline version from the start (Task 5 Step 1 describes the final form). The static version in Task 3 is superseded by Task 5.
