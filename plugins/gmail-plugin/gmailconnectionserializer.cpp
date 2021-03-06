#include "gmailconnectionserializer.h"
#include "gmailconnectioninfo.h"
#include "serialization-helpers.h"

uint32_t GmailConnectionSerializer::get_size(
    const PluginConnectionInfo* connInfo) const noexcept {
  if (const auto* conn = dynamic_cast<const GmailConnectionInfo*>(connInfo);
      conn == nullptr) {
    return 0;
  } else {
    uint32_t size = 0;
    size += sizeof(std::string::size_type) + conn->email.length();
    size += sizeof(std::string::size_type) + conn->accessToken.length();
    return size;
  }
}

bool GmailConnectionSerializer::serialize(
    void* buffer,
    const PluginConnectionInfo* connInfo) const noexcept {
  if (const auto* conn = dynamic_cast<const GmailConnectionInfo*>(connInfo);
      conn == nullptr) {
    return false;
  } else {
    buffer = serialization_helpers::save_str(conn->email, buffer);
    buffer = serialization_helpers::save_str(conn->accessToken, buffer);
    return true;
  }
}

bool GmailConnectionSerializer::deserialize(
    const void* buffer,
    PluginConnectionInfo* connInfo) const noexcept {
  if (auto* conn = dynamic_cast<GmailConnectionInfo*>(connInfo);
      conn == nullptr) {
    return false;
  } else {
    buffer = serialization_helpers::restore_str(conn->email, buffer);
    buffer = serialization_helpers::restore_str(conn->accessToken, buffer);
    return true;
  }
}
