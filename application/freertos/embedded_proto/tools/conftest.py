"""conftest.py — skip all tests if paicore_pb2 is not available.

Generate paicore_pb2.py with:
    protoc --python_out=. ../proto/paicore.proto
Run from this tools/ directory.
"""
import pytest

try:
    import paicore_pb2  # noqa: F401
    _PROTO_AVAILABLE = True
except (ImportError, RuntimeError):
    _PROTO_AVAILABLE = False


def pytest_collection_modifyitems(items):
    if not _PROTO_AVAILABLE:
        skip_marker = pytest.mark.skip(
            reason="paicore_pb2 not found — run: "
                   "protoc --python_out=. ../proto/paicore.proto"
        )
        for item in items:
            item.add_marker(skip_marker)
