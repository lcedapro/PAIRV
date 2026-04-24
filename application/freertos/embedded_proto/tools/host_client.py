"""PaiCore host client — USB-UART 3 MHz, protobuf framing.

Usage:
    python3 host_client.py [--port /dev/ttyUSB0] [--baud 3000000]

Requires:
    pip install pyserial
    protoc --python_out=. ../proto/paicore.proto  (or grpc_tools equivalent)
"""
import argparse
import struct
import time

import numpy as np
import serial

from paicore_pb2 import DeviceMessage, HostMessage

MAGIC = b"PAIC"
DEFAULT_PORT = "/dev/ttyUSB0"
DEFAULT_BAUD = 3_000_000


# ---------------------------------------------------------------------------
# Framing
# ---------------------------------------------------------------------------

def encode_frame(payload: bytes) -> bytes:
    return MAGIC + struct.pack("<H", len(payload)) + payload


def recv_frame(ser: serial.Serial, timeout: float = 5.0) -> bytes:
    buf = b""
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        chunk = ser.read(ser.in_waiting or 1)
        if chunk:
            buf += chunk
        idx = buf.find(MAGIC)
        if idx == -1:
            buf = buf[-3:]
            continue
        buf = buf[idx:]
        if len(buf) < 6:
            continue
        length = struct.unpack("<H", buf[4:6])[0]
        if len(buf) >= 6 + length:
            return buf[6 : 6 + length]
    raise TimeoutError("No response within timeout")


def send_msg(ser: serial.Serial, msg: HostMessage, timeout: float = 5.0) -> DeviceMessage:
    payload = msg.SerializeToString()
    ser.write(encode_frame(payload))
    raw = recv_frame(ser, timeout=timeout)
    resp = DeviceMessage()
    resp.ParseFromString(raw)
    return resp


# ---------------------------------------------------------------------------
# High-level operations
# ---------------------------------------------------------------------------

def do_config(ser: serial.Serial, block_index: int) -> bool:
    msg = HostMessage()
    msg.config_req.block_index = block_index
    resp = send_msg(ser, msg, timeout=15.0)
    assert resp.HasField("config_resp"), f"Unexpected response: {resp}"
    ok = resp.config_resp.success
    status = "OK" if ok else "FAIL"
    print(f"  config block {block_index}: {status}")
    return ok


def do_test(ser: serial.Serial, block_index: int) -> bool:
    msg = HostMessage()
    msg.test_req.block_index = block_index
    resp = send_msg(ser, msg, timeout=15.0)
    assert resp.HasField("test_resp"), f"Unexpected response: {resp}"
    ok = resp.test_resp.success
    if ok:
        print(f"  test  block {block_index}: OK")
    else:
        print(f"  test  block {block_index}: FAIL at frame {resp.test_resp.mismatch_frame}")
    return ok


def do_init(ser: serial.Serial, thread_id: int = 0) -> bool:
    msg = HostMessage()
    msg.init_req.thread_id = thread_id
    resp = send_msg(ser, msg, timeout=3.0)
    assert resp.HasField("init_resp"), f"Unexpected response: {resp}"
    ok = resp.init_resp.success
    print(f"  init  thread {thread_id}: {'OK' if ok else 'FAIL'}")
    return ok


def do_infer(ser: serial.Serial, thread_id: int,
             data: np.ndarray, timesteps: int) -> np.ndarray | None:
    msg = HostMessage()
    msg.infer_req.thread_id = thread_id
    msg.infer_req.timesteps = timesteps
    msg.infer_req.data = data.astype(np.uint8).tobytes()
    resp = send_msg(ser, msg, timeout=8.0)
    assert resp.HasField("infer_resp"), f"Unexpected response: {resp}"
    if not resp.infer_resp.success:
        print(f"  infer thread {thread_id}: FAIL")
        return None
    output = np.frombuffer(resp.infer_resp.data, dtype=np.uint8)
    print(f"  infer thread {thread_id}: OK  cycles={resp.infer_resp.cycles}  "
          f"output_len={len(output)}")
    return output


# ---------------------------------------------------------------------------
# Main test sequence
# ---------------------------------------------------------------------------

def run_board_test(port: str, baud: int, num_blocks: int,
                   C: int, H: int, W: int, T: int = 1) -> None:
    print(f"Opening {port} @ {baud} baud")
    with serial.Serial(port, baud, timeout=0.05) as ser:
        time.sleep(0.2)
        ser.reset_input_buffer()

        print("\n[1] Chip configuration")
        for i in range(num_blocks):
            assert do_config(ser, i), f"Config block {i} failed — aborting"

        print("\n[2] Configuration verification (test frames)")
        for i in range(num_blocks):
            do_test(ser, i)

        print("\n[3] Init inference thread 0")
        assert do_init(ser, thread_id=0), "Init failed — aborting"

        print("\n[4] Zero-input inference (sanity check)")
        data = np.zeros(T * C * H * W, dtype=np.uint8)
        do_infer(ser, thread_id=0, data=data, timesteps=T)

        print("\n[5] Stress test (10 consecutive inferences)")
        for i in range(10):
            data = np.random.randint(0, 4, size=T * C * H * W, dtype=np.uint8)
            out = do_infer(ser, thread_id=0, data=data, timesteps=T)
            if out is None:
                print(f"  iteration {i}: FAIL")

        print("\nDone.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="PaiCore board test client")
    parser.add_argument("--port",       default=DEFAULT_PORT)
    parser.add_argument("--baud",       type=int, default=DEFAULT_BAUD)
    parser.add_argument("--num-blocks", type=int, default=4,
                        help="Number of config blocks in g_config_blocks[]")
    parser.add_argument("--C",  type=int, default=2)
    parser.add_argument("--H",  type=int, default=2)
    parser.add_argument("--W",  type=int, default=2)
    parser.add_argument("--T",  type=int, default=1, help="Timesteps")
    args = parser.parse_args()

    run_board_test(args.port, args.baud, args.num_blocks,
                   args.C, args.H, args.W, args.T)
