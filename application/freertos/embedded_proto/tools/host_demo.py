"""PaiCore host-side communication client.

NOTE: paicore_pb2.py must be generated before using this module.
Generate it with:
    protoc --python_out=. ../proto/paicore.proto
Run from this tools/ directory.
"""
import struct
import serial
from paicore_pb2 import HostMessage, DeviceMessage  # noqa: E402 — generated file

MAGIC = b'PAIC'
BAUDRATE = 3_000_000


class PaiCoreClient:
    def __init__(self, port: str, timeout: float = 5.0):
        self.ser = serial.Serial(port, BAUDRATE, timeout=timeout)

    def close(self):
        self.ser.close()

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()

    def _send(self, msg: HostMessage) -> None:
        payload = msg.SerializeToString()
        header = MAGIC + struct.pack('<H', len(payload))
        self.ser.write(header + payload)

    def _recv(self) -> DeviceMessage:
        # Scan for MAGIC using a fixed 4-byte window (no unbounded buffer growth)
        window = bytearray(4)
        for i in range(4):
            b = self.ser.read(1)
            if not b:
                raise TimeoutError("UART read timeout waiting for magic")
            window[i] = b[0]
        while bytes(window) != MAGIC:
            b = self.ser.read(1)
            if not b:
                raise TimeoutError("UART read timeout waiting for magic")
            window = window[1:] + bytearray(b)
        # Read length
        raw_len = self.ser.read(2)
        if len(raw_len) < 2:
            raise TimeoutError("UART read timeout waiting for length")
        length = struct.unpack('<H', raw_len)[0]
        # Read payload
        payload = self.ser.read(length)
        if len(payload) < length:
            raise TimeoutError("UART read timeout waiting for payload")
        resp = DeviceMessage()
        resp.ParseFromString(payload)
        return resp

    def send_init(self, thread_id: int) -> bool:
        """Send InitRequest and return success flag from InitResponse."""
        msg = HostMessage()
        msg.init_req.thread_id = thread_id
        self._send(msg)
        resp = self._recv()
        return resp.init_resp.success

    def send_infer(self, thread_id: int, timesteps: int, data: bytes) -> tuple:
        """Send InferRequest and return (success, data, cycles) from InferResponse."""
        msg = HostMessage()
        msg.infer_req.thread_id = thread_id
        msg.infer_req.timesteps = timesteps
        msg.infer_req.data = data
        self._send(msg)
        resp = self._recv()
        return resp.infer_resp.success, resp.infer_resp.data, resp.infer_resp.cycles
