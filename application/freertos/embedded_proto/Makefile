# application/freertos/embedded_proto/Makefile
TARGET = paicore_firmware

# To regenerate inc/proto/paicore.h (requires protoc + protoc-gen-eams):
#   cd $(NUCLEI_SDK_ROOT)/third_party/EmbeddedProto && pip install -e .
#   protoc --plugin=protoc-gen-eams=$(pwd)/protoc-gen-eams \
#          --eams_out=../../application/freertos/embedded_proto/inc/proto \
#          ../../application/freertos/embedded_proto/proto/paicore.proto
# Note: generated classes are templated, e.g. paicore::HostMessage<PROTO_BUF_SIZE>

RTOS = FreeRTOS

NUCLEI_SDK_ROOT = ../../..

EMBEDDED_PROTO_DIR = $(NUCLEI_SDK_ROOT)/third_party/EmbeddedProto

SRCDIRS = . src data $(EMBEDDED_PROTO_DIR)/src

INCDIRS = . inc inc/proto $(EMBEDDED_PROTO_DIR)/src

CORE ?= n307fd
DOWNLOAD ?= ilmflashxip

COMMON_FLAGS := -O2
APP_CXXFLAGS := -std=c++11 -fno-exceptions -fno-rtti

include $(NUCLEI_SDK_ROOT)/Build/Makefile.base
