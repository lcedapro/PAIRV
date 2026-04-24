"""Offline smoke tests for host_client.py — no hardware required.

Tests encode_frame / recv_frame / send_msg using an in-memory mock serial port.
Run with: python3 -m pytest test_host_client.py -v
"""
import io
import struct
import threading

import pytest

from paicore_pb2 import DeviceMessage, HostMessage
from host_client import encode_frame, recv_frame, send_msg

MAGIC = b"PAIC"


# ---------------------------------------------------------------------------
# Minimal mock serial that satisfies host_client's serial.Serial interface
# ---------------------------------------------------------------------------

class MockSerial:
    """Bidirectional in-memory serial port."""

    def __init__(self, response_bytes: bytes = b""):
        self._rx = io.BytesIO(response_bytes)  # data the "device" sends back
        self._tx = io.BytesIO()                # data the host wrote
        self.in_waiting = 0
        self._lock = threading.Lock()

    def read(self, n: int = 1) -> bytes:
        data = self._rx.read(n)
        return data

    def write(self, data: bytes) -> int:
        self._tx.write(data)
        return len(data)

    def reset_input_buffer(self) -> None:
        self._rx.seek(0, 2)  # seek to end

    @property
    def written(self) -> bytes:
        return self._tx.getvalue()

    def _inject(self, data: bytes) -> None:
        """Append bytes to the RX buffer (simulates device sending data)."""
        pos = self._rx.tell()
        self._rx.seek(0, 2)
        self._rx.write(data)
        self._rx.seek(pos)


# ---------------------------------------------------------------------------
# encode_frame tests
# ---------------------------------------------------------------------------

def test_encode_frame_magic():
    frame = encode_frame(b"\x01\x02")
    assert frame[:4] == MAGIC


def test_encode_frame_length():
    payload = b"\xAA" * 10
    frame = encode_frame(payload)
    length = struct.unpack("<H", frame[4:6])[0]
    assert length == 10


def test_encode_frame_payload():
    payload = b"\x01\x02\x03"
    frame = encode_frame(payload)
    assert frame[6:] == payload


def test_encode_frame_empty_payload():
    frame = encode_frame(b"")
    assert struct.unpack("<H", frame[4:6])[0] == 0
    assert len(frame) == 6


# ---------------------------------------------------------------------------
# recv_frame tests
# ---------------------------------------------------------------------------

def _make_rx(payload: bytes) -> bytes:
    return encode_frame(payload)


def test_recv_frame_basic():
    payload = b"\x11\x22\x33"
    ser = MockSerial(_make_rx(payload))
    result = recv_frame(ser, timeout=1.0)
    assert result == payload


def test_recv_frame_with_leading_garbage():
    payload = b"\xAB\xCD"
    garbage = b"\x00\xFF\x12\x34"
    ser = MockSerial(garbage + _make_rx(payload))
    result = recv_frame(ser, timeout=1.0)
    assert result == payload


def test_recv_frame_timeout():
    ser = MockSerial(b"")
    with pytest.raises(TimeoutError):
        recv_frame(ser, timeout=0.05)


def test_recv_frame_partial_magic_then_full():
    payload = b"\x55"
    # Inject b"PAI" (partial magic) then the full frame
    data = b"PAI" + _make_rx(payload)
    ser = MockSerial(data)
    result = recv_frame(ser, timeout=1.0)
    assert result == payload


# ---------------------------------------------------------------------------
# send_msg / round-trip tests
# ---------------------------------------------------------------------------

def _device_frame(dmsg: DeviceMessage) -> bytes:
    return encode_frame(dmsg.SerializeToString())


def test_send_msg_init_request():
    dmsg = DeviceMessage()
    dmsg.init_resp.thread_id = 0
    dmsg.init_resp.success = True

    ser = MockSerial(_device_frame(dmsg))

    hmsg = HostMessage()
    hmsg.init_req.thread_id = 0
    resp = send_msg(ser, hmsg, timeout=1.0)

    assert resp.HasField("init_resp")
    assert resp.init_resp.success is True
    assert resp.init_resp.thread_id == 0


def test_send_msg_infer_request():
    dmsg = DeviceMessage()
    dmsg.infer_resp.thread_id = 0
    dmsg.infer_resp.success = True
    dmsg.infer_resp.cycles = 42
    dmsg.infer_resp.data = bytes([1, 0, 0, 0])

    ser = MockSerial(_device_frame(dmsg))

    hmsg = HostMessage()
    hmsg.infer_req.thread_id = 0
    hmsg.infer_req.timesteps = 1
    hmsg.infer_req.data = bytes(8)
    resp = send_msg(ser, hmsg, timeout=1.0)

    assert resp.HasField("infer_resp")
    assert resp.infer_resp.success is True
    assert resp.infer_resp.cycles == 42
    assert resp.infer_resp.data == bytes([1, 0, 0, 0])


def test_send_msg_config_request():
    dmsg = DeviceMessage()
    dmsg.config_resp.block_index = 2
    dmsg.config_resp.success = True

    ser = MockSerial(_device_frame(dmsg))

    hmsg = HostMessage()
    hmsg.config_req.block_index = 2
    resp = send_msg(ser, hmsg, timeout=1.0)

    assert resp.HasField("config_resp")
    assert resp.config_resp.block_index == 2
    assert resp.config_resp.success is True


def test_send_msg_test_request_failure():
    dmsg = DeviceMessage()
    dmsg.test_resp.block_index = 1
    dmsg.test_resp.success = False
    dmsg.test_resp.mismatch_frame = 17

    ser = MockSerial(_device_frame(dmsg))

    hmsg = HostMessage()
    hmsg.test_req.block_index = 1
    resp = send_msg(ser, hmsg, timeout=1.0)

    assert resp.HasField("test_resp")
    assert resp.test_resp.success is False
    assert resp.test_resp.mismatch_frame == 17


def test_host_writes_correct_frame():
    """Verify the bytes written to serial match the expected UART frame."""
    dmsg = DeviceMessage()
    dmsg.init_resp.thread_id = 0
    dmsg.init_resp.success = True

    ser = MockSerial(_device_frame(dmsg))

    hmsg = HostMessage()
    hmsg.init_req.thread_id = 0
    send_msg(ser, hmsg, timeout=1.0)

    written = ser.written
    assert written[:4] == MAGIC
    length = struct.unpack("<H", written[4:6])[0]
    assert length == len(written) - 6

    decoded = HostMessage()
    decoded.ParseFromString(written[6:])
    assert decoded.HasField("init_req")
    assert decoded.init_req.thread_id == 0
