#pragma once

#include "medio-events.hpp"

#include <string>
#include <cstring>

#include <pb_encode.h>
#include "messages_medio.pb.h"

#define UNUSED(x) ((void)x)

// max protobuf message size
#define MAX_PB_MESSAGE_SIZE 450

// [[nodiscard]]
// static proto_v1_medio_PulseEventKind enumKindToProto(PulseEventKind kind)
// {
//   switch (kind) {
//     case PulseEventKind::EV_UNKNOWN: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_UNSPECIFIED;
//     case PulseEventKind::EV_HOME: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_HOME;
//     case PulseEventKind::EV_9D: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_9D;
//     case PulseEventKind::EV_INT: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_INT;
//     case PulseEventKind::EV_2D: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_2D;
//     case PulseEventKind::EV_PEN: return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_PEN;
//     default:
//     //   ULOG_ERROR("Unknown pulse event kind: {}", std::underlying_type_t<PulseEventKind>(kind));
//       return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_UNSPECIFIED;
//   }
// }

static bool encode_counters(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  UNUSED(stream);
  UNUSED(field);
  UNUSED(arg);

//   proto_v1_medio_CounterValue counters[PulseEventKindCount()] = {};
//   for (std::size_t i = 0; i < PulseEventKindCount(); ++i)
//   {
//     counters[i].counter = enumKindToProto(static_cast<PulseEventKind>(i));
//     counters[i].count = state.counters.get(static_cast<PulseEventKind>(i));

//     if (counters[i].count > 0) {
//       if (!pb_encode_tag_for_field(stream, field))
//       {
//         return false;
//       }

//       if (!pb_encode_submessage(stream, proto_v1_medio_CounterValue_fields, &counters[i]))
//       {
//         return false;
//       }
//     }
//   }

  return true;
}

static bool encode_stored_counters(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  UNUSED(stream);
  UNUSED(field);
  UNUSED(arg);

//   proto_v1_medio_CounterValue counters[PulseEventKindCount()] = {};
//   for (std::size_t i = 0; i < PulseEventKindCount(); ++i)
//   {
//     counters[i].counter = enumKindToProto(static_cast<PulseEventKind>(i));
//     counters[i].count = state.storedCounters.get(static_cast<PulseEventKind>(i));

//     if (counters[i].count > 0) {
//       if (!pb_encode_tag_for_field(stream, field))
//       {
//         return false;
//       }

//       if (!pb_encode_submessage(stream, proto_v1_medio_CounterValue_fields, &counters[i]))
//       {
//         return false;
//       }
//     }
//   }

  return true;
}

static bool encode_stored_pulses(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  UNUSED(stream);
  UNUSED(field);
  UNUSED(arg);

//   proto_v1_medio_CounterValue counters[PulseEventKindCount()] = {};
//   for (std::size_t i = 0; i < PulseEventKindCount(); ++i)
//   {
//     counters[i].counter = enumKindToProto(static_cast<PulseEventKind>(i));
//     counters[i].count = state.storedPulses.get(static_cast<PulseEventKind>(i));

//     if (counters[i].count > 0) {
//       if (!pb_encode_tag_for_field(stream, field))
//       {
//         return false;
//       }

//       if (!pb_encode_submessage(stream, proto_v1_medio_CounterValue_fields, &counters[i]))
//       {
//         return false;
//       }
//     }
//   }

  return true;
}

static bool encode_scans(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  static constexpr size_t maxScanCount = sizeof(WifiScanEvent::scans) / sizeof(WifiScanSingle);
  static constexpr size_t bssidItemCount = sizeof(WifiScanSingle::bssid) / sizeof(uint8_t);
  proto_v1_medio_WifiScan scans[maxScanCount] = {proto_v1_medio_WifiScan_init_default};
  const WifiScanEvent *ev = reinterpret_cast<const WifiScanEvent *>(*arg);

  for (size_t i = 0; i < maxScanCount; ++i) {
    WifiScanSingle wss = ev->scans[i];
    
    // if the bssid is 0x00, we skip it
    bool isZero = true;
    for (size_t j = 0; j < bssidItemCount; ++j) {
      if (wss.bssid[j] != 0x00) {
        isZero = false;
        break;
      }
    }
    
    if (isZero) {
      continue;
    }

    std::memcpy(scans[i].bssid.bytes, wss.bssid, bssidItemCount);
    scans[i].bssid.size = bssidItemCount;
    scans[i].signal = wss.signal;

    if (!pb_encode_tag_for_field(stream, field))
    {
      return false;
    }

    if (!pb_encode_submessage(stream, proto_v1_medio_WifiScan_fields, &scans[i]))
    {
      return false;
    }
  }

  return true;
}

class MedIOMessageEncoder
{
public:
  MedIOMessageEncoder(uint32_t board_version, uint32_t boot_id)
  {
    message_.counters.funcs.encode = &encode_counters;
    message_.stored_counters.funcs.encode = &encode_stored_counters;
    message_.stored_pulses.funcs.encode = &encode_stored_pulses;
    message_.board_version = board_version;
    message_.boot_id = boot_id;
    message_.boot_id_2 = boot_id;
    message_.boot_id_3 = boot_id;
  }

  static std::string SyncEventToProtobuf(uint32_t board_version, uint32_t boot_id, SyncEvent &event)
  {
    MedIOMessageEncoder msg(board_version, boot_id);

    auto visitor = [&](auto &&arg)
    {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, PulseEvent>)
      {
        msg.SetPulseEvent(std::forward<decltype(arg)>(arg));
      }
      else if constexpr (std::is_same_v<T, ModeChangeEvent>)
      {
        msg.SetModeChangeEvent(std::forward<decltype(arg)>(arg));
      }
      else if constexpr (std::is_same_v<T, WifiScanEvent>)
      {
        msg.SetWifiScanEvent(std::forward<decltype(arg)>(arg));
      }
      else if constexpr (std::is_same_v<T, PingEvent>)
      {
        msg.SetPingEvent(std::forward<decltype(arg)>(arg));
      }
      else if constexpr (std::is_same_v<T, OnOffEvent>)
      {
        msg.SetOnOffChangeEvent(std::forward<decltype(arg)>(arg));
      }

      return msg.ToString();
    };

    return std::visit(visitor, event.payload);
  }

  proto_v1_medio_SyncMessage Build()
  {
    return message_;
  }

  std::string ToString()
  {
    uint8_t buf[MAX_PB_MESSAGE_SIZE] = {0};
    std::string encoded;

    auto ostream = pb_ostream_from_buffer(buf, sizeof(buf));
    auto success = pb_encode(&ostream, &proto_v1_medio_SyncMessage_msg, &message_);
    if (!success)
    {
      return "";
    }

    return std::string((const char *)buf, ostream.bytes_written);
  }

  void SetPulseEvent(const PulseEvent &event)
  {
    proto_v1_medio_PulseEvent msg = proto_v1_medio_PulseEvent_init_default;
    msg.duration = event.duration;
    msg.kind = PulseEventKindToProtobuf(event.kind);
    msg.measurements = event.measurements;

    message_.which_msg = proto_v1_medio_SyncMessage_pulse_event_tag;
    message_.msg.pulse_event = msg;
  }

  void SetModeChangeEvent(const ModeChangeEvent &event)
  {
    proto_v1_medio_ModeChangeEvent msg = proto_v1_medio_ModeChangeEvent_init_default;
    msg.mode = PulseEventKindToProtobuf(event.kind);

    message_.which_msg = proto_v1_medio_SyncMessage_mode_change_event_tag;
    message_.msg.mode_change_event = msg;
  }

  void SetWifiScanEvent(WifiScanEvent &event)
  {
    proto_v1_medio_WifiScanList msg = proto_v1_medio_WifiScanList_init_default;
    msg.scans.arg = reinterpret_cast<void *>(&event);
    msg.scans.funcs.encode = &encode_scans;

    message_.which_msg = proto_v1_medio_SyncMessage_wifi_scan_event_tag;
    message_.msg.wifi_scan_event = msg;
  }

  void SetPingEvent(PingEvent &event)
  {
    UNUSED(event);
    proto_v1_medio_PingSync msg = proto_v1_medio_PingSync_init_default;
    message_.which_msg = proto_v1_medio_SyncMessage_ping_sync_event_tag;
    message_.msg.ping_sync_event = msg;
    message_.msg.ping_sync_event.is_ping = true;
  }

  void SetOnOffChangeEvent(OnOffEvent &event)
  {
    proto_v1_medio_OnOffChange msg = proto_v1_medio_OnOffChange_init_default;
    message_.which_msg = proto_v1_medio_SyncMessage_on_off_change_event_tag;
    message_.msg.on_off_change_event = msg;
    message_.msg.on_off_change_event.enabled = event.enabled;
    message_.msg.on_off_change_event.input = event.input;
    message_.msg.on_off_change_event.elapsed_time_secs = event.elapsed_time_secs;
  }

private:
  proto_v1_medio_SyncMessage message_ = proto_v1_medio_SyncMessage_init_default;

  proto_v1_medio_PulseEventKind PulseEventKindToProtobuf(const PulseEventKind &kind)
  {
    switch (kind)
    {
    case PulseEventKind::EV_HOME:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_HOME;
    case PulseEventKind::EV_9D:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_9D;
    case PulseEventKind::EV_INT:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_INT;
    case PulseEventKind::EV_2D:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_2D;
    case PulseEventKind::EV_PEN:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_PEN;
    default:
      return proto_v1_medio_PulseEventKind_PULSE_EVENT_KIND_UNSPECIFIED;
    }
  }
};

