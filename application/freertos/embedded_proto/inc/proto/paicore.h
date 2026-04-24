/*
 *  This file is generated with Embedded Proto, PLEASE DO NOT EDIT!
 *  source: paicore.proto
 */

// This file is generated. Please do not edit!
#ifndef PAICORE_H
#define PAICORE_H

#include <cstdint>
#include <MessageInterface.h>
#include <WireFormatter.h>
#include <Fields.h>
#include <MessageSizeCalculator.h>
#include <ReadBufferSection.h>
#include <RepeatedFieldFixedSize.h>
#include <FieldStringBytes.h>
#include <Errors.h>
#include <Defines.h>
#include <limits>

// Include external proto definitions

namespace paicore {

// ============================================================
// InitRequest
// ============================================================

class InitRequest final: public ::EmbeddedProto::MessageInterface
{
  public:
    InitRequest() = default;
    InitRequest(const InitRequest& rhs)
    {
      set_thread_id(rhs.get_thread_id());
    }
    InitRequest(const InitRequest&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
    }
    ~InitRequest() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      THREAD_ID = 1
    };

    InitRequest& operator=(const InitRequest& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      return *this;
    }
    InitRequest& operator=(const InitRequest&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      return *this;
    }

    static constexpr char const* THREAD_ID_NAME = "thread_id";
    inline void clear_thread_id() { thread_id_.clear(); }
    inline void set_thread_id(const uint32_t& value) { thread_id_ = value; }
    inline void set_thread_id(const uint32_t&& value) { thread_id_ = value; }
    inline uint32_t& mutable_thread_id() { return thread_id_.get(); }
    inline const uint32_t& get_thread_id() const { return thread_id_.get(); }
    inline uint32_t thread_id() const { return thread_id_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != thread_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = thread_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::THREAD_ID), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::THREAD_ID:
            return_value = thread_id_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_thread_id();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::THREAD_ID: name = THREAD_ID_NAME; break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 thread_id_ = 0U;
};

// ============================================================
// InferRequest
// ============================================================

template<
    uint32_t InferRequest_data_LENGTH
>
class InferRequest final: public ::EmbeddedProto::MessageInterface
{
  public:
    InferRequest() = default;
    InferRequest(const InferRequest& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_timesteps(rhs.get_timesteps());
      set_data(rhs.get_data());
    }
    InferRequest(const InferRequest&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_timesteps(rhs.get_timesteps());
      set_data(rhs.get_data());
    }
    ~InferRequest() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      THREAD_ID = 1,
      TIMESTEPS = 2,
      DATA = 3
    };

    InferRequest& operator=(const InferRequest& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_timesteps(rhs.get_timesteps());
      set_data(rhs.get_data());
      return *this;
    }
    InferRequest& operator=(const InferRequest&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_timesteps(rhs.get_timesteps());
      set_data(rhs.get_data());
      return *this;
    }

    static constexpr char const* THREAD_ID_NAME = "thread_id";
    inline void clear_thread_id() { thread_id_.clear(); }
    inline void set_thread_id(const uint32_t& value) { thread_id_ = value; }
    inline void set_thread_id(const uint32_t&& value) { thread_id_ = value; }
    inline uint32_t& mutable_thread_id() { return thread_id_.get(); }
    inline const uint32_t& get_thread_id() const { return thread_id_.get(); }
    inline uint32_t thread_id() const { return thread_id_.get(); }

    static constexpr char const* TIMESTEPS_NAME = "timesteps";
    inline void clear_timesteps() { timesteps_.clear(); }
    inline void set_timesteps(const uint32_t& value) { timesteps_ = value; }
    inline void set_timesteps(const uint32_t&& value) { timesteps_ = value; }
    inline uint32_t& mutable_timesteps() { return timesteps_.get(); }
    inline const uint32_t& get_timesteps() const { return timesteps_.get(); }
    inline uint32_t timesteps() const { return timesteps_.get(); }

    static constexpr char const* DATA_NAME = "data";
    inline void clear_data() { data_.clear(); }
    inline ::EmbeddedProto::FieldBytes<InferRequest_data_LENGTH>& mutable_data() { return data_; }
    inline void set_data(const ::EmbeddedProto::FieldBytes<InferRequest_data_LENGTH>& rhs) { data_.set(rhs); }
    inline const ::EmbeddedProto::FieldBytes<InferRequest_data_LENGTH>& get_data() const { return data_; }
    inline const uint8_t* data() const { return data_.get_const(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != thread_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = thread_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::THREAD_ID), buffer, false);
      }
      if((0U != timesteps_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = timesteps_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TIMESTEPS), buffer, false);
      }
      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = data_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DATA), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::THREAD_ID:
            return_value = thread_id_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::TIMESTEPS:
            return_value = timesteps_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::DATA:
            return_value = data_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_thread_id();
      clear_timesteps();
      clear_data();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::THREAD_ID:  name = THREAD_ID_NAME;  break;
        case FieldNumber::TIMESTEPS:  name = TIMESTEPS_NAME;  break;
        case FieldNumber::DATA:       name = DATA_NAME;        break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 thread_id_ = 0U;
    EmbeddedProto::uint32 timesteps_ = 0U;
    ::EmbeddedProto::FieldBytes<InferRequest_data_LENGTH> data_;
};

// ============================================================
// ConfigRequest
// ============================================================

class ConfigRequest final: public ::EmbeddedProto::MessageInterface
{
  public:
    ConfigRequest() = default;
    ConfigRequest(const ConfigRequest& rhs)
    {
      set_block_index(rhs.get_block_index());
    }
    ConfigRequest(const ConfigRequest&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
    }
    ~ConfigRequest() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      BLOCK_INDEX = 1
    };

    ConfigRequest& operator=(const ConfigRequest& rhs)
    {
      set_block_index(rhs.get_block_index());
      return *this;
    }
    ConfigRequest& operator=(const ConfigRequest&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      return *this;
    }

    static constexpr char const* BLOCK_INDEX_NAME = "block_index";
    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const uint32_t& value) { block_index_ = value; }
    inline void set_block_index(const uint32_t&& value) { block_index_ = value; }
    inline uint32_t& mutable_block_index() { return block_index_.get(); }
    inline const uint32_t& get_block_index() const { return block_index_.get(); }
    inline uint32_t block_index() const { return block_index_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BLOCK_INDEX), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::BLOCK_INDEX:
            return_value = block_index_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_block_index();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::BLOCK_INDEX: name = BLOCK_INDEX_NAME; break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
};

// ============================================================
// TestRequest
// ============================================================

class TestRequest final: public ::EmbeddedProto::MessageInterface
{
  public:
    TestRequest() = default;
    TestRequest(const TestRequest& rhs)
    {
      set_block_index(rhs.get_block_index());
    }
    TestRequest(const TestRequest&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
    }
    ~TestRequest() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      BLOCK_INDEX = 1
    };

    TestRequest& operator=(const TestRequest& rhs)
    {
      set_block_index(rhs.get_block_index());
      return *this;
    }
    TestRequest& operator=(const TestRequest&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      return *this;
    }

    static constexpr char const* BLOCK_INDEX_NAME = "block_index";
    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const uint32_t& value) { block_index_ = value; }
    inline void set_block_index(const uint32_t&& value) { block_index_ = value; }
    inline uint32_t& mutable_block_index() { return block_index_.get(); }
    inline const uint32_t& get_block_index() const { return block_index_.get(); }
    inline uint32_t block_index() const { return block_index_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BLOCK_INDEX), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::BLOCK_INDEX:
            return_value = block_index_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_block_index();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::BLOCK_INDEX: name = BLOCK_INDEX_NAME; break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
};

// ============================================================
// HostMessage (oneof: init_req | infer_req | config_req | test_req)
// ============================================================

template<
    uint32_t HostMessage_infer_req_InferRequest_data_LENGTH
>
class HostMessage final: public ::EmbeddedProto::MessageInterface
{
  public:
    HostMessage() = default;
    HostMessage(const HostMessage& rhs)
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_REQ:
          set_init_req(rhs.get_init_req());
          break;
        case FieldNumber::INFER_REQ:
          set_infer_req(rhs.get_infer_req());
          break;
        case FieldNumber::CONFIG_REQ:
          set_config_req(rhs.get_config_req());
          break;
        case FieldNumber::TEST_REQ:
          set_test_req(rhs.get_test_req());
          break;
        default:
          break;
      }
    }
    HostMessage(const HostMessage&& rhs) noexcept
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_REQ:
          set_init_req(rhs.get_init_req());
          break;
        case FieldNumber::INFER_REQ:
          set_infer_req(rhs.get_infer_req());
          break;
        case FieldNumber::CONFIG_REQ:
          set_config_req(rhs.get_config_req());
          break;
        case FieldNumber::TEST_REQ:
          set_test_req(rhs.get_test_req());
          break;
        default:
          break;
      }
    }
    ~HostMessage() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      INIT_REQ = 1,
      INFER_REQ = 2,
      CONFIG_REQ = 3,
      TEST_REQ = 4
    };

    HostMessage& operator=(const HostMessage& rhs)
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_REQ:
          set_init_req(rhs.get_init_req());
          break;
        case FieldNumber::INFER_REQ:
          set_infer_req(rhs.get_infer_req());
          break;
        case FieldNumber::CONFIG_REQ:
          set_config_req(rhs.get_config_req());
          break;
        case FieldNumber::TEST_REQ:
          set_test_req(rhs.get_test_req());
          break;
        default:
          break;
      }
      return *this;
    }
    HostMessage& operator=(const HostMessage&& rhs) noexcept
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_REQ:
          set_init_req(rhs.get_init_req());
          break;
        case FieldNumber::INFER_REQ:
          set_infer_req(rhs.get_infer_req());
          break;
        case FieldNumber::CONFIG_REQ:
          set_config_req(rhs.get_config_req());
          break;
        case FieldNumber::TEST_REQ:
          set_test_req(rhs.get_test_req());
          break;
        default:
          break;
      }
      return *this;
    }

    FieldNumber get_which_payload() const { return which_payload_; }

    static constexpr char const* INIT_REQ_NAME = "init_req";
    inline bool has_init_req() const { return FieldNumber::INIT_REQ == which_payload_; }
    inline void clear_init_req()
    {
      if(FieldNumber::INIT_REQ == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.init_req_.~InitRequest();
      }
    }
    inline void set_init_req(const InitRequest& value)
    {
      if(FieldNumber::INIT_REQ != which_payload_)
      {
        init_payload(FieldNumber::INIT_REQ);
      }
      payload_.init_req_ = value;
    }
    inline void set_init_req(const InitRequest&& value)
    {
      if(FieldNumber::INIT_REQ != which_payload_)
      {
        init_payload(FieldNumber::INIT_REQ);
      }
      payload_.init_req_ = value;
    }
    inline InitRequest& mutable_init_req()
    {
      if(FieldNumber::INIT_REQ != which_payload_)
      {
        init_payload(FieldNumber::INIT_REQ);
      }
      return payload_.init_req_;
    }
    inline const InitRequest& get_init_req() const { return payload_.init_req_; }
    inline const InitRequest& init_req() const { return payload_.init_req_; }

    static constexpr char const* INFER_REQ_NAME = "infer_req";
    inline bool has_infer_req() const { return FieldNumber::INFER_REQ == which_payload_; }
    inline void clear_infer_req()
    {
      if(FieldNumber::INFER_REQ == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.infer_req_.~InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>();
      }
    }
    inline void set_infer_req(const InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>& value)
    {
      if(FieldNumber::INFER_REQ != which_payload_)
      {
        init_payload(FieldNumber::INFER_REQ);
      }
      payload_.infer_req_ = value;
    }
    inline void set_infer_req(const InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>&& value)
    {
      if(FieldNumber::INFER_REQ != which_payload_)
      {
        init_payload(FieldNumber::INFER_REQ);
      }
      payload_.infer_req_ = value;
    }
    inline InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>& mutable_infer_req()
    {
      if(FieldNumber::INFER_REQ != which_payload_)
      {
        init_payload(FieldNumber::INFER_REQ);
      }
      return payload_.infer_req_;
    }
    inline const InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>& get_infer_req() const { return payload_.infer_req_; }
    inline const InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>& infer_req() const { return payload_.infer_req_; }

    static constexpr char const* CONFIG_REQ_NAME = "config_req";
    inline bool has_config_req() const { return FieldNumber::CONFIG_REQ == which_payload_; }
    inline void clear_config_req()
    {
      if(FieldNumber::CONFIG_REQ == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.config_req_.~ConfigRequest();
      }
    }
    inline void set_config_req(const ConfigRequest& value)
    {
      if(FieldNumber::CONFIG_REQ != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_REQ);
      }
      payload_.config_req_ = value;
    }
    inline void set_config_req(const ConfigRequest&& value)
    {
      if(FieldNumber::CONFIG_REQ != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_REQ);
      }
      payload_.config_req_ = value;
    }
    inline ConfigRequest& mutable_config_req()
    {
      if(FieldNumber::CONFIG_REQ != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_REQ);
      }
      return payload_.config_req_;
    }
    inline const ConfigRequest& get_config_req() const { return payload_.config_req_; }
    inline const ConfigRequest& config_req() const { return payload_.config_req_; }

    static constexpr char const* TEST_REQ_NAME = "test_req";
    inline bool has_test_req() const { return FieldNumber::TEST_REQ == which_payload_; }
    inline void clear_test_req()
    {
      if(FieldNumber::TEST_REQ == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.test_req_.~TestRequest();
      }
    }
    inline void set_test_req(const TestRequest& value)
    {
      if(FieldNumber::TEST_REQ != which_payload_)
      {
        init_payload(FieldNumber::TEST_REQ);
      }
      payload_.test_req_ = value;
    }
    inline void set_test_req(const TestRequest&& value)
    {
      if(FieldNumber::TEST_REQ != which_payload_)
      {
        init_payload(FieldNumber::TEST_REQ);
      }
      payload_.test_req_ = value;
    }
    inline TestRequest& mutable_test_req()
    {
      if(FieldNumber::TEST_REQ != which_payload_)
      {
        init_payload(FieldNumber::TEST_REQ);
      }
      return payload_.test_req_;
    }
    inline const TestRequest& get_test_req() const { return payload_.test_req_; }
    inline const TestRequest& test_req() const { return payload_.test_req_; }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      switch(which_payload_)
      {
        case FieldNumber::INIT_REQ:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.init_req_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INIT_REQ), buffer, true);
          }
          break;
        case FieldNumber::INFER_REQ:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.infer_req_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INFER_REQ), buffer, true);
          }
          break;
        case FieldNumber::CONFIG_REQ:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.config_req_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CONFIG_REQ), buffer, true);
          }
          break;
        case FieldNumber::TEST_REQ:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.test_req_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEST_REQ), buffer, true);
          }
          break;
        default:
          break;
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::INIT_REQ:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::INFER_REQ:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::CONFIG_REQ:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::TEST_REQ:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_payload();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::INIT_REQ:  name = INIT_REQ_NAME;  break;
        case FieldNumber::INFER_REQ: name = INFER_REQ_NAME; break;
        case FieldNumber::CONFIG_REQ: name = CONFIG_REQ_NAME; break;
        case FieldNumber::TEST_REQ:   name = TEST_REQ_NAME;   break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    FieldNumber which_payload_ = FieldNumber::NOT_SET;
    union payload_union
    {
      payload_union() {}
      ~payload_union() {}
      InitRequest init_req_;
      InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH> infer_req_;
      ConfigRequest config_req_;
      TestRequest test_req_;
    } payload_;

    void init_payload(const FieldNumber field_id)
    {
      if(FieldNumber::NOT_SET != which_payload_)
      {
        clear_payload();
      }
      switch(field_id)
      {
        case FieldNumber::INIT_REQ:
          new(&payload_.init_req_) InitRequest;
          break;
        case FieldNumber::INFER_REQ:
          new(&payload_.infer_req_) InferRequest<HostMessage_infer_req_InferRequest_data_LENGTH>;
          break;
        case FieldNumber::CONFIG_REQ:
          new(&payload_.config_req_) ConfigRequest;
          break;
        case FieldNumber::TEST_REQ:
          new(&payload_.test_req_) TestRequest;
          break;
        default:
          break;
      }
      which_payload_ = field_id;
    }

    void clear_payload()
    {
      switch(which_payload_)
      {
        case FieldNumber::INIT_REQ:
          ::EmbeddedProto::destroy_at(&payload_.init_req_);
          break;
        case FieldNumber::INFER_REQ:
          ::EmbeddedProto::destroy_at(&payload_.infer_req_);
          break;
        case FieldNumber::CONFIG_REQ:
          ::EmbeddedProto::destroy_at(&payload_.config_req_);
          break;
        case FieldNumber::TEST_REQ:
          ::EmbeddedProto::destroy_at(&payload_.test_req_);
          break;
        default:
          break;
      }
      which_payload_ = FieldNumber::NOT_SET;
    }

    ::EmbeddedProto::Error deserialize_payload(const FieldNumber field_id,
                                               ::EmbeddedProto::ReadBufferInterface& buffer,
                                               const ::EmbeddedProto::WireFormatter::WireType wire_type)
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if(field_id != which_payload_)
      {
        init_payload(field_id);
      }
      switch(which_payload_)
      {
        case FieldNumber::INIT_REQ:
          return_value = payload_.init_req_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::INFER_REQ:
          return_value = payload_.infer_req_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::CONFIG_REQ:
          return_value = payload_.config_req_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::TEST_REQ:
          return_value = payload_.test_req_.deserialize_check_type(buffer, wire_type);
          break;
        default:
          break;
      }
      if(::EmbeddedProto::Error::NO_ERRORS != return_value)
      {
        clear_payload();
      }
      return return_value;
    }
};

// ============================================================
// InitResponse
// ============================================================

class InitResponse final: public ::EmbeddedProto::MessageInterface
{
  public:
    InitResponse() = default;
    InitResponse(const InitResponse& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
    }
    InitResponse(const InitResponse&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
    }
    ~InitResponse() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      THREAD_ID = 1,
      SUCCESS = 2
    };

    InitResponse& operator=(const InitResponse& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      return *this;
    }
    InitResponse& operator=(const InitResponse&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      return *this;
    }

    static constexpr char const* THREAD_ID_NAME = "thread_id";
    inline void clear_thread_id() { thread_id_.clear(); }
    inline void set_thread_id(const uint32_t& value) { thread_id_ = value; }
    inline void set_thread_id(const uint32_t&& value) { thread_id_ = value; }
    inline uint32_t& mutable_thread_id() { return thread_id_.get(); }
    inline const uint32_t& get_thread_id() const { return thread_id_.get(); }
    inline uint32_t thread_id() const { return thread_id_.get(); }

    static constexpr char const* SUCCESS_NAME = "success";
    inline void clear_success() { success_.clear(); }
    inline void set_success(const bool& value) { success_ = value; }
    inline void set_success(const bool&& value) { success_ = value; }
    inline bool& mutable_success() { return success_.get(); }
    inline const bool& get_success() const { return success_.get(); }
    inline bool success() const { return success_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != thread_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = thread_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::THREAD_ID), buffer, false);
      }
      if((false != success_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = success_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUCCESS), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::THREAD_ID:
            return_value = thread_id_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::SUCCESS:
            return_value = success_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_thread_id();
      clear_success();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::THREAD_ID: name = THREAD_ID_NAME; break;
        case FieldNumber::SUCCESS:   name = SUCCESS_NAME;   break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 thread_id_ = 0U;
    EmbeddedProto::boolean success_ = false;
};

// ============================================================
// InferResponse
// ============================================================

template<
    uint32_t InferResponse_data_LENGTH
>
class InferResponse final: public ::EmbeddedProto::MessageInterface
{
  public:
    InferResponse() = default;
    InferResponse(const InferResponse& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      set_data(rhs.get_data());
      set_cycles(rhs.get_cycles());
    }
    InferResponse(const InferResponse&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      set_data(rhs.get_data());
      set_cycles(rhs.get_cycles());
    }
    ~InferResponse() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      THREAD_ID = 1,
      SUCCESS = 2,
      DATA = 3,
      CYCLES = 4
    };

    InferResponse& operator=(const InferResponse& rhs)
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      set_data(rhs.get_data());
      set_cycles(rhs.get_cycles());
      return *this;
    }
    InferResponse& operator=(const InferResponse&& rhs) noexcept
    {
      set_thread_id(rhs.get_thread_id());
      set_success(rhs.get_success());
      set_data(rhs.get_data());
      set_cycles(rhs.get_cycles());
      return *this;
    }

    static constexpr char const* THREAD_ID_NAME = "thread_id";
    inline void clear_thread_id() { thread_id_.clear(); }
    inline void set_thread_id(const uint32_t& value) { thread_id_ = value; }
    inline void set_thread_id(const uint32_t&& value) { thread_id_ = value; }
    inline uint32_t& mutable_thread_id() { return thread_id_.get(); }
    inline const uint32_t& get_thread_id() const { return thread_id_.get(); }
    inline uint32_t thread_id() const { return thread_id_.get(); }

    static constexpr char const* SUCCESS_NAME = "success";
    inline void clear_success() { success_.clear(); }
    inline void set_success(const bool& value) { success_ = value; }
    inline void set_success(const bool&& value) { success_ = value; }
    inline bool& mutable_success() { return success_.get(); }
    inline const bool& get_success() const { return success_.get(); }
    inline bool success() const { return success_.get(); }

    static constexpr char const* DATA_NAME = "data";
    inline void clear_data() { data_.clear(); }
    inline ::EmbeddedProto::FieldBytes<InferResponse_data_LENGTH>& mutable_data() { return data_; }
    inline void set_data(const ::EmbeddedProto::FieldBytes<InferResponse_data_LENGTH>& rhs) { data_.set(rhs); }
    inline const ::EmbeddedProto::FieldBytes<InferResponse_data_LENGTH>& get_data() const { return data_; }
    inline const uint8_t* data() const { return data_.get_const(); }

    static constexpr char const* CYCLES_NAME = "cycles";
    inline void clear_cycles() { cycles_.clear(); }
    inline void set_cycles(const uint32_t& value) { cycles_ = value; }
    inline void set_cycles(const uint32_t&& value) { cycles_ = value; }
    inline uint32_t& mutable_cycles() { return cycles_.get(); }
    inline const uint32_t& get_cycles() const { return cycles_.get(); }
    inline uint32_t cycles() const { return cycles_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != thread_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = thread_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::THREAD_ID), buffer, false);
      }
      if((false != success_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = success_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUCCESS), buffer, false);
      }
      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = data_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DATA), buffer, false);
      }
      if((0U != cycles_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = cycles_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CYCLES), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::THREAD_ID:
            return_value = thread_id_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::SUCCESS:
            return_value = success_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::DATA:
            return_value = data_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::CYCLES:
            return_value = cycles_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_thread_id();
      clear_success();
      clear_data();
      clear_cycles();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::THREAD_ID: name = THREAD_ID_NAME; break;
        case FieldNumber::SUCCESS:   name = SUCCESS_NAME;   break;
        case FieldNumber::DATA:      name = DATA_NAME;      break;
        case FieldNumber::CYCLES:    name = CYCLES_NAME;    break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 thread_id_ = 0U;
    EmbeddedProto::boolean success_ = false;
    ::EmbeddedProto::FieldBytes<InferResponse_data_LENGTH> data_;
    EmbeddedProto::uint32 cycles_ = 0U;
};

// ============================================================
// ConfigResponse
// ============================================================

class ConfigResponse final: public ::EmbeddedProto::MessageInterface
{
  public:
    ConfigResponse() = default;
    ConfigResponse(const ConfigResponse& rhs)
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
    }
    ConfigResponse(const ConfigResponse&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
    }
    ~ConfigResponse() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      BLOCK_INDEX = 1,
      SUCCESS = 2
    };

    ConfigResponse& operator=(const ConfigResponse& rhs)
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      return *this;
    }
    ConfigResponse& operator=(const ConfigResponse&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      return *this;
    }

    static constexpr char const* BLOCK_INDEX_NAME = "block_index";
    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const uint32_t& value) { block_index_ = value; }
    inline void set_block_index(const uint32_t&& value) { block_index_ = value; }
    inline uint32_t& mutable_block_index() { return block_index_.get(); }
    inline const uint32_t& get_block_index() const { return block_index_.get(); }
    inline uint32_t block_index() const { return block_index_.get(); }

    static constexpr char const* SUCCESS_NAME = "success";
    inline void clear_success() { success_.clear(); }
    inline void set_success(const bool& value) { success_ = value; }
    inline void set_success(const bool&& value) { success_ = value; }
    inline bool& mutable_success() { return success_.get(); }
    inline const bool& get_success() const { return success_.get(); }
    inline bool success() const { return success_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BLOCK_INDEX), buffer, false);
      }
      if((false != success_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = success_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUCCESS), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::BLOCK_INDEX:
            return_value = block_index_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::SUCCESS:
            return_value = success_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_block_index();
      clear_success();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::BLOCK_INDEX: name = BLOCK_INDEX_NAME; break;
        case FieldNumber::SUCCESS:     name = SUCCESS_NAME;     break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
    EmbeddedProto::boolean success_ = false;
};

// ============================================================
// TestResponse
// ============================================================

class TestResponse final: public ::EmbeddedProto::MessageInterface
{
  public:
    TestResponse() = default;
    TestResponse(const TestResponse& rhs)
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      set_mismatch_frame(rhs.get_mismatch_frame());
    }
    TestResponse(const TestResponse&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      set_mismatch_frame(rhs.get_mismatch_frame());
    }
    ~TestResponse() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      BLOCK_INDEX = 1,
      SUCCESS = 2,
      MISMATCH_FRAME = 3
    };

    TestResponse& operator=(const TestResponse& rhs)
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      set_mismatch_frame(rhs.get_mismatch_frame());
      return *this;
    }
    TestResponse& operator=(const TestResponse&& rhs) noexcept
    {
      set_block_index(rhs.get_block_index());
      set_success(rhs.get_success());
      set_mismatch_frame(rhs.get_mismatch_frame());
      return *this;
    }

    static constexpr char const* BLOCK_INDEX_NAME = "block_index";
    inline void clear_block_index() { block_index_.clear(); }
    inline void set_block_index(const uint32_t& value) { block_index_ = value; }
    inline void set_block_index(const uint32_t&& value) { block_index_ = value; }
    inline uint32_t& mutable_block_index() { return block_index_.get(); }
    inline const uint32_t& get_block_index() const { return block_index_.get(); }
    inline uint32_t block_index() const { return block_index_.get(); }

    static constexpr char const* SUCCESS_NAME = "success";
    inline void clear_success() { success_.clear(); }
    inline void set_success(const bool& value) { success_ = value; }
    inline void set_success(const bool&& value) { success_ = value; }
    inline bool& mutable_success() { return success_.get(); }
    inline const bool& get_success() const { return success_.get(); }
    inline bool success() const { return success_.get(); }

    static constexpr char const* MISMATCH_FRAME_NAME = "mismatch_frame";
    inline void clear_mismatch_frame() { mismatch_frame_.clear(); }
    inline void set_mismatch_frame(const uint32_t& value) { mismatch_frame_ = value; }
    inline void set_mismatch_frame(const uint32_t&& value) { mismatch_frame_ = value; }
    inline uint32_t& mutable_mismatch_frame() { return mismatch_frame_.get(); }
    inline const uint32_t& get_mismatch_frame() const { return mismatch_frame_.get(); }
    inline uint32_t mismatch_frame() const { return mismatch_frame_.get(); }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if((0U != block_index_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = block_index_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BLOCK_INDEX), buffer, false);
      }
      if((false != success_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = success_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUCCESS), buffer, false);
      }
      if((0U != mismatch_frame_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = mismatch_frame_.serialize_with_id(static_cast<uint32_t>(FieldNumber::MISMATCH_FRAME), buffer, false);
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::BLOCK_INDEX:
            return_value = block_index_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::SUCCESS:
            return_value = success_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::MISMATCH_FRAME:
            return_value = mismatch_frame_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_block_index();
      clear_success();
      clear_mismatch_frame();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::BLOCK_INDEX:    name = BLOCK_INDEX_NAME;    break;
        case FieldNumber::SUCCESS:        name = SUCCESS_NAME;        break;
        case FieldNumber::MISMATCH_FRAME: name = MISMATCH_FRAME_NAME; break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    EmbeddedProto::uint32 block_index_ = 0U;
    EmbeddedProto::boolean success_ = false;
    EmbeddedProto::uint32 mismatch_frame_ = 0U;
};

// ============================================================
// DeviceMessage (oneof: init_resp | infer_resp | config_resp | test_resp)
// ============================================================

template<
    uint32_t DeviceMessage_infer_resp_InferResponse_data_LENGTH
>
class DeviceMessage final: public ::EmbeddedProto::MessageInterface
{
  public:
    DeviceMessage() = default;
    DeviceMessage(const DeviceMessage& rhs)
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_RESP:
          set_init_resp(rhs.get_init_resp());
          break;
        case FieldNumber::INFER_RESP:
          set_infer_resp(rhs.get_infer_resp());
          break;
        case FieldNumber::CONFIG_RESP:
          set_config_resp(rhs.get_config_resp());
          break;
        case FieldNumber::TEST_RESP:
          set_test_resp(rhs.get_test_resp());
          break;
        default:
          break;
      }
    }
    DeviceMessage(const DeviceMessage&& rhs) noexcept
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_RESP:
          set_init_resp(rhs.get_init_resp());
          break;
        case FieldNumber::INFER_RESP:
          set_infer_resp(rhs.get_infer_resp());
          break;
        case FieldNumber::CONFIG_RESP:
          set_config_resp(rhs.get_config_resp());
          break;
        case FieldNumber::TEST_RESP:
          set_test_resp(rhs.get_test_resp());
          break;
        default:
          break;
      }
    }
    ~DeviceMessage() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      INIT_RESP = 1,
      INFER_RESP = 2,
      CONFIG_RESP = 3,
      TEST_RESP = 4
    };

    DeviceMessage& operator=(const DeviceMessage& rhs)
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_RESP:
          set_init_resp(rhs.get_init_resp());
          break;
        case FieldNumber::INFER_RESP:
          set_infer_resp(rhs.get_infer_resp());
          break;
        case FieldNumber::CONFIG_RESP:
          set_config_resp(rhs.get_config_resp());
          break;
        case FieldNumber::TEST_RESP:
          set_test_resp(rhs.get_test_resp());
          break;
        default:
          break;
      }
      return *this;
    }
    DeviceMessage& operator=(const DeviceMessage&& rhs) noexcept
    {
      if(rhs.get_which_payload() != which_payload_)
      {
        clear_payload();
      }
      switch(rhs.get_which_payload())
      {
        case FieldNumber::INIT_RESP:
          set_init_resp(rhs.get_init_resp());
          break;
        case FieldNumber::INFER_RESP:
          set_infer_resp(rhs.get_infer_resp());
          break;
        case FieldNumber::CONFIG_RESP:
          set_config_resp(rhs.get_config_resp());
          break;
        case FieldNumber::TEST_RESP:
          set_test_resp(rhs.get_test_resp());
          break;
        default:
          break;
      }
      return *this;
    }

    FieldNumber get_which_payload() const { return which_payload_; }

    static constexpr char const* INIT_RESP_NAME = "init_resp";
    inline bool has_init_resp() const { return FieldNumber::INIT_RESP == which_payload_; }
    inline void clear_init_resp()
    {
      if(FieldNumber::INIT_RESP == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.init_resp_.~InitResponse();
      }
    }
    inline void set_init_resp(const InitResponse& value)
    {
      if(FieldNumber::INIT_RESP != which_payload_)
      {
        init_payload(FieldNumber::INIT_RESP);
      }
      payload_.init_resp_ = value;
    }
    inline void set_init_resp(const InitResponse&& value)
    {
      if(FieldNumber::INIT_RESP != which_payload_)
      {
        init_payload(FieldNumber::INIT_RESP);
      }
      payload_.init_resp_ = value;
    }
    inline InitResponse& mutable_init_resp()
    {
      if(FieldNumber::INIT_RESP != which_payload_)
      {
        init_payload(FieldNumber::INIT_RESP);
      }
      return payload_.init_resp_;
    }
    inline const InitResponse& get_init_resp() const { return payload_.init_resp_; }
    inline const InitResponse& init_resp() const { return payload_.init_resp_; }

    static constexpr char const* INFER_RESP_NAME = "infer_resp";
    inline bool has_infer_resp() const { return FieldNumber::INFER_RESP == which_payload_; }
    inline void clear_infer_resp()
    {
      if(FieldNumber::INFER_RESP == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.infer_resp_.~InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>();
      }
    }
    inline void set_infer_resp(const InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>& value)
    {
      if(FieldNumber::INFER_RESP != which_payload_)
      {
        init_payload(FieldNumber::INFER_RESP);
      }
      payload_.infer_resp_ = value;
    }
    inline void set_infer_resp(const InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>&& value)
    {
      if(FieldNumber::INFER_RESP != which_payload_)
      {
        init_payload(FieldNumber::INFER_RESP);
      }
      payload_.infer_resp_ = value;
    }
    inline InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>& mutable_infer_resp()
    {
      if(FieldNumber::INFER_RESP != which_payload_)
      {
        init_payload(FieldNumber::INFER_RESP);
      }
      return payload_.infer_resp_;
    }
    inline const InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>& get_infer_resp() const { return payload_.infer_resp_; }
    inline const InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>& infer_resp() const { return payload_.infer_resp_; }

    static constexpr char const* CONFIG_RESP_NAME = "config_resp";
    inline bool has_config_resp() const { return FieldNumber::CONFIG_RESP == which_payload_; }
    inline void clear_config_resp()
    {
      if(FieldNumber::CONFIG_RESP == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.config_resp_.~ConfigResponse();
      }
    }
    inline void set_config_resp(const ConfigResponse& value)
    {
      if(FieldNumber::CONFIG_RESP != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_RESP);
      }
      payload_.config_resp_ = value;
    }
    inline void set_config_resp(const ConfigResponse&& value)
    {
      if(FieldNumber::CONFIG_RESP != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_RESP);
      }
      payload_.config_resp_ = value;
    }
    inline ConfigResponse& mutable_config_resp()
    {
      if(FieldNumber::CONFIG_RESP != which_payload_)
      {
        init_payload(FieldNumber::CONFIG_RESP);
      }
      return payload_.config_resp_;
    }
    inline const ConfigResponse& get_config_resp() const { return payload_.config_resp_; }
    inline const ConfigResponse& config_resp() const { return payload_.config_resp_; }

    static constexpr char const* TEST_RESP_NAME = "test_resp";
    inline bool has_test_resp() const { return FieldNumber::TEST_RESP == which_payload_; }
    inline void clear_test_resp()
    {
      if(FieldNumber::TEST_RESP == which_payload_)
      {
        which_payload_ = FieldNumber::NOT_SET;
        payload_.test_resp_.~TestResponse();
      }
    }
    inline void set_test_resp(const TestResponse& value)
    {
      if(FieldNumber::TEST_RESP != which_payload_)
      {
        init_payload(FieldNumber::TEST_RESP);
      }
      payload_.test_resp_ = value;
    }
    inline void set_test_resp(const TestResponse&& value)
    {
      if(FieldNumber::TEST_RESP != which_payload_)
      {
        init_payload(FieldNumber::TEST_RESP);
      }
      payload_.test_resp_ = value;
    }
    inline TestResponse& mutable_test_resp()
    {
      if(FieldNumber::TEST_RESP != which_payload_)
      {
        init_payload(FieldNumber::TEST_RESP);
      }
      return payload_.test_resp_;
    }
    inline const TestResponse& get_test_resp() const { return payload_.test_resp_; }
    inline const TestResponse& test_resp() const { return payload_.test_resp_; }

    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      switch(which_payload_)
      {
        case FieldNumber::INIT_RESP:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.init_resp_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INIT_RESP), buffer, true);
          }
          break;
        case FieldNumber::INFER_RESP:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.infer_resp_.serialize_with_id(static_cast<uint32_t>(FieldNumber::INFER_RESP), buffer, true);
          }
          break;
        case FieldNumber::CONFIG_RESP:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.config_resp_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CONFIG_RESP), buffer, true);
          }
          break;
        case FieldNumber::TEST_RESP:
          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = payload_.test_resp_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEST_RESP), buffer, true);
          }
          break;
        default:
          break;
      }
      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::INIT_RESP:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::INFER_RESP:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::CONFIG_RESP:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::TEST_RESP:
            return_value = deserialize_payload(id_tag, buffer, wire_type);
            break;
          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;
          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value))
      {
        return_value = tag_value;
      }
      return return_value;
    };

    void clear() override
    {
      clear_payload();
    }

#ifndef DISABLE_FIELD_NUMBER_TO_NAME
    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::INIT_RESP:  name = INIT_RESP_NAME;  break;
        case FieldNumber::INFER_RESP: name = INFER_RESP_NAME; break;
        case FieldNumber::CONFIG_RESP: name = CONFIG_RESP_NAME; break;
        case FieldNumber::TEST_RESP:   name = TEST_RESP_NAME;   break;
        default: name = "Invalid FieldNumber"; break;
      }
      return name;
    }
#endif

  private:
    FieldNumber which_payload_ = FieldNumber::NOT_SET;
    union payload_union
    {
      payload_union() {}
      ~payload_union() {}
      InitResponse init_resp_;
      InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH> infer_resp_;
      ConfigResponse config_resp_;
      TestResponse test_resp_;
    } payload_;

    void init_payload(const FieldNumber field_id)
    {
      if(FieldNumber::NOT_SET != which_payload_)
      {
        clear_payload();
      }
      switch(field_id)
      {
        case FieldNumber::INIT_RESP:
          new(&payload_.init_resp_) InitResponse;
          break;
        case FieldNumber::INFER_RESP:
          new(&payload_.infer_resp_) InferResponse<DeviceMessage_infer_resp_InferResponse_data_LENGTH>;
          break;
        case FieldNumber::CONFIG_RESP:
          new(&payload_.config_resp_) ConfigResponse;
          break;
        case FieldNumber::TEST_RESP:
          new(&payload_.test_resp_) TestResponse;
          break;
        default:
          break;
      }
      which_payload_ = field_id;
    }

    void clear_payload()
    {
      switch(which_payload_)
      {
        case FieldNumber::INIT_RESP:
          ::EmbeddedProto::destroy_at(&payload_.init_resp_);
          break;
        case FieldNumber::INFER_RESP:
          ::EmbeddedProto::destroy_at(&payload_.infer_resp_);
          break;
        case FieldNumber::CONFIG_RESP:
          ::EmbeddedProto::destroy_at(&payload_.config_resp_);
          break;
        case FieldNumber::TEST_RESP:
          ::EmbeddedProto::destroy_at(&payload_.test_resp_);
          break;
        default:
          break;
      }
      which_payload_ = FieldNumber::NOT_SET;
    }

    ::EmbeddedProto::Error deserialize_payload(const FieldNumber field_id,
                                               ::EmbeddedProto::ReadBufferInterface& buffer,
                                               const ::EmbeddedProto::WireFormatter::WireType wire_type)
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      if(field_id != which_payload_)
      {
        init_payload(field_id);
      }
      switch(which_payload_)
      {
        case FieldNumber::INIT_RESP:
          return_value = payload_.init_resp_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::INFER_RESP:
          return_value = payload_.infer_resp_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::CONFIG_RESP:
          return_value = payload_.config_resp_.deserialize_check_type(buffer, wire_type);
          break;
        case FieldNumber::TEST_RESP:
          return_value = payload_.test_resp_.deserialize_check_type(buffer, wire_type);
          break;
        default:
          break;
      }
      if(::EmbeddedProto::Error::NO_ERRORS != return_value)
      {
        clear_payload();
      }
      return return_value;
    }
};

} // namespace paicore

#endif // PAICORE_H
