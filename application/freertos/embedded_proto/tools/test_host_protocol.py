"""Tests for PaiCore host-side protocol (no hardware required).

These tests exercise protobuf message encoding/decoding and UART frame
construction entirely in Python — no serial port or device needed.

If paicore_pb2 is missing, conftest.py will skip all tests automatically.
Generate it with:
    protoc --python_out=. ../proto/paicore.proto
"""
import struct
import pytest
from paicore_pb2 import HostMessage, DeviceMessage

MAGIC = b'PAIC'


# ---------------------------------------------------------------------------
# Frame helpers (mirror the firmware framing logic)
# ---------------------------------------------------------------------------

def encode_frame(payload: bytes) -> bytes:
    """Wrap a serialised protobuf payload in a PAIC frame."""
    return MAGIC + struct.pack('<H', len(payload)) + payload


def decode_frame(frame: bytes) -> bytes:
    """Strip the PAIC frame header and return the raw payload."""
    if frame[:4] != MAGIC:
        raise ValueError(f"Invalid magic: {frame[:4]!r}")
    length = struct.unpack('<H', frame[4:6])[0]
    return frame[6:6 + length]


def scan_and_decode(stream: bytes) -> bytes:
    """Scan a byte stream for MAGIC, then decode the frame that follows."""
    idx = stream.find(MAGIC)
    if idx == -1:
        raise ValueError("MAGIC not found in stream")
    return decode_frame(stream[idx:])


# ---------------------------------------------------------------------------
# Test 1 — InitRequest round-trip
# ---------------------------------------------------------------------------

def test_init_request_roundtrip():
    msg = HostMessage()
    msg.init_req.thread_id = 42
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("init_req")
    assert decoded.init_req.thread_id == 42


# ---------------------------------------------------------------------------
# Test 2 — InferRequest round-trip
# ---------------------------------------------------------------------------

def test_infer_request_roundtrip():
    msg = HostMessage()
    msg.infer_req.thread_id = 7
    msg.infer_req.timesteps = 16
    msg.infer_req.data = b'\x01\x02\x03\x04'
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("infer_req")
    assert decoded.infer_req.thread_id == 7
    assert decoded.infer_req.timesteps == 16
    assert decoded.infer_req.data == b'\x01\x02\x03\x04'


# ---------------------------------------------------------------------------
# Test 3 — InitResponse round-trip
# ---------------------------------------------------------------------------

def test_init_response_roundtrip():
    msg = DeviceMessage()
    msg.init_resp.thread_id = 1
    msg.init_resp.success = True
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("init_resp")
    assert decoded.init_resp.thread_id == 1
    assert decoded.init_resp.success is True


# ---------------------------------------------------------------------------
# Test 4 — InferResponse round-trip
# ---------------------------------------------------------------------------

def test_infer_response_roundtrip():
    msg = DeviceMessage()
    msg.infer_resp.thread_id = 3
    msg.infer_resp.success = True
    msg.infer_resp.data = b'\xde\xad\xbe\xef'
    msg.infer_resp.cycles = 123456
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("infer_resp")
    assert decoded.infer_resp.thread_id == 3
    assert decoded.infer_resp.success is True
    assert decoded.infer_resp.data == b'\xde\xad\xbe\xef'
    assert decoded.infer_resp.cycles == 123456


# ---------------------------------------------------------------------------
# Test 5 — Frame encoding: magic, LE length, payload
# ---------------------------------------------------------------------------

def test_frame_encoding_structure():
    payload = b'\xAA\xBB\xCC'
    frame = encode_frame(payload)

    assert frame[:4] == MAGIC
    length_field = struct.unpack('<H', frame[4:6])[0]
    assert length_field == len(payload)
    assert frame[6:] == payload


# ---------------------------------------------------------------------------
# Test 6 — Empty data InferRequest round-trips correctly
# ---------------------------------------------------------------------------

def test_infer_request_empty_data():
    msg = HostMessage()
    msg.infer_req.thread_id = 0
    msg.infer_req.timesteps = 1
    msg.infer_req.data = b''
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.infer_req.data == b''


# ---------------------------------------------------------------------------
# Test 7 — Large data (2048 bytes) round-trips correctly
# ---------------------------------------------------------------------------

def test_infer_request_max_data():
    big = bytes(range(256)) * 8  # 2048 bytes
    msg = HostMessage()
    msg.infer_req.thread_id = 1
    msg.infer_req.timesteps = 4
    msg.infer_req.data = big
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.infer_req.data == big


# ---------------------------------------------------------------------------
# Test 8 — Invalid magic raises ValueError
# ---------------------------------------------------------------------------

def test_decode_frame_bad_magic():
    bad_frame = b'XXXX' + struct.pack('<H', 3) + b'\x01\x02\x03'
    with pytest.raises(ValueError):
        decode_frame(bad_frame)


# ---------------------------------------------------------------------------
# Test 9 — Scanning past garbage bytes before magic
# ---------------------------------------------------------------------------

def test_scan_skips_garbage_before_magic():
    msg = HostMessage()
    msg.init_req.thread_id = 99
    payload = msg.SerializeToString()
    frame = encode_frame(payload)

    # Prepend some garbage bytes
    stream = b'\x00\xFF\x12\x34\x56' + frame

    raw = scan_and_decode(stream)
    decoded = HostMessage()
    decoded.ParseFromString(raw)

    assert decoded.init_req.thread_id == 99


# ---------------------------------------------------------------------------
# Test 10 — HostMessage oneof: setting infer_req clears init_req
# ---------------------------------------------------------------------------

def test_host_message_oneof_exclusivity():
    msg = HostMessage()
    msg.init_req.thread_id = 5
    assert msg.HasField("init_req")

    # Assigning the other oneof field should clear the first
    msg.infer_req.thread_id = 10
    assert msg.HasField("infer_req")
    assert not msg.HasField("init_req")


# ---------------------------------------------------------------------------
# Test 11 — Frame with InitRequest encodes/decodes end-to-end
# ---------------------------------------------------------------------------

def test_full_frame_init_request():
    msg = HostMessage()
    msg.init_req.thread_id = 55
    payload = msg.SerializeToString()
    frame = encode_frame(payload)

    raw = decode_frame(frame)
    decoded = HostMessage()
    decoded.ParseFromString(raw)

    assert decoded.init_req.thread_id == 55


# ---------------------------------------------------------------------------
# Test 12 — Frame length field matches actual payload length
# ---------------------------------------------------------------------------

def test_frame_length_field_accuracy():
    for size in (0, 1, 255, 256, 1000):
        payload = bytes(size)
        frame = encode_frame(payload)
        length_field = struct.unpack('<H', frame[4:6])[0]
        assert length_field == size


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
    msg.test_req.block_index = 5
    payload = msg.SerializeToString()

    decoded = HostMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_req")
    assert decoded.test_req.block_index == 5


# ---------------------------------------------------------------------------
# Test 15 — ConfigResponse round-trip (success)
# ---------------------------------------------------------------------------

def test_config_response_success():
    msg = DeviceMessage()
    msg.config_resp.block_index = 2
    msg.config_resp.success = True
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("config_resp")
    assert decoded.config_resp.block_index == 2
    assert decoded.config_resp.success is True


# ---------------------------------------------------------------------------
# Test 16 — ConfigResponse round-trip (failure)
# ---------------------------------------------------------------------------

def test_config_response_failure():
    msg = DeviceMessage()
    msg.config_resp.block_index = 0
    msg.config_resp.success = False
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("config_resp")
    assert decoded.config_resp.success is False


# ---------------------------------------------------------------------------
# Test 17 — TestResponse round-trip (success)
# ---------------------------------------------------------------------------

def test_test_response_success():
    msg = DeviceMessage()
    msg.test_resp.block_index = 1
    msg.test_resp.success = True
    msg.test_resp.mismatch_frame = 0
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_resp")
    assert decoded.test_resp.block_index == 1
    assert decoded.test_resp.success is True
    assert decoded.test_resp.mismatch_frame == 0


# ---------------------------------------------------------------------------
# Test 18 — TestResponse round-trip (failure with mismatch_frame)
# ---------------------------------------------------------------------------

def test_test_response_failure_mismatch():
    msg = DeviceMessage()
    msg.test_resp.block_index = 3
    msg.test_resp.success = False
    msg.test_resp.mismatch_frame = 42
    payload = msg.SerializeToString()

    decoded = DeviceMessage()
    decoded.ParseFromString(payload)

    assert decoded.HasField("test_resp")
    assert decoded.test_resp.success is False
    assert decoded.test_resp.mismatch_frame == 42


# ---------------------------------------------------------------------------
# Test 19 — HostMessage oneof: config_req clears infer_req
# ---------------------------------------------------------------------------

def test_host_message_oneof_config_clears_infer():
    msg = HostMessage()
    msg.infer_req.thread_id = 7
    assert msg.HasField("infer_req")

    msg.config_req.block_index = 1
    assert msg.HasField("config_req")
    assert not msg.HasField("infer_req")
