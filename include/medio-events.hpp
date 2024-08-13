#pragma once

#include <string>
#include <vector>
#include <variant>
#include <stdint.h>

// #if __has_include("Arduino.h")
// #include "Arduino.h"
// #endif

// Define the enum items dynamically so the count of items can be determined
// at compile time.
// IMPORTANT: do not ever remove. Only add new items. Always preserve order.
#define PULSE_EVENT_KINDS(X) \
  X(EV_UNKNOWN)              \
  /* Home */                 \
  X(EV_HOME)                 \
  /* Aplicador 9D */         \
  X(EV_9D)                   \
  /* Aplicador Íntimo */     \
  X(EV_INT)                  \
  /* Aplicador 2D */         \
  X(EV_2D)                   \
  /* Aplicador Pen */        \
  X(EV_PEN)                  \
  /* Specific for VLY */     \
  X(EV_VLY)                  \
  /* Specific for TGND */    \
  X(EV_TGND)

// Generate the enum class
enum class PulseEventKind : uint32_t
{
#define X(name) name,
  PULSE_EVENT_KINDS(X)
#undef X
};

// Count the number of items
constexpr std::size_t PulseEventKindCount()
{
  enum Count : uint32_t
  {
#define X(name) +1
    Count = 0 PULSE_EVENT_KINDS(X)
#undef X
  };
  return static_cast<std::size_t>(Count);
}

struct PulseEvent
{
  // kind of the event
  PulseEventKind kind = PulseEventKind::EV_UNKNOWN;
  // duration in ms of the event, from first fire to the end of the window
  uint32_t duration = 0;
  // how many internal measurements were created during the execution of the event
  uint32_t measurements = 0;
};

const std::string PulseEventKindToString(const PulseEventKind &kind);

struct ModeChangeEvent
{
  // The new mode
  PulseEventKind kind = PulseEventKind::EV_UNKNOWN;
};

struct PingEvent {
  bool is_ping;
};

struct OnOffEvent {
  bool enabled;
  uint32_t input;
  uint32_t elapsed_time_secs;
};

struct WifiScanSingle
{
  uint8_t bssid[6];
  int32_t signal;
};

struct WifiScanEvent
{
  WifiScanSingle scans[5];
};

struct SyncEvent
{
  std::variant<PulseEvent, ModeChangeEvent, PingEvent, WifiScanEvent, OnOffEvent> payload;
};