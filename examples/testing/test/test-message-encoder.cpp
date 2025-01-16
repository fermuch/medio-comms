#include <gtest/gtest.h>
#include <string>
#include <variant>

#include "medio-message-encoder.hpp"

class MedIOMessageEncoderTest : public ::testing::Test
{
protected:
  void SetUp() override
  {}

  void TearDown() override
  {}
};

TEST_F(MedIOMessageEncoderTest, SyncEventToProtobuf_PulseEvent)
{
  // Create a PulseEvent
  PulseEvent pulseEvent;
  pulseEvent.kind = PulseEventKind::EV_VLY;
  pulseEvent.duration = 100;
  pulseEvent.measurements = 42;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = pulseEvent;

  // Convert to Protobuf string
  auto encoder = MedIOMessageEncoder(42, 69);
  std::string protobufStr = encoder.SyncEventToProtobuf(syncEvent);

  // TODO: use decoder to check if it is a valid message
  ASSERT_FALSE(protobufStr.empty());
}

TEST_F(MedIOMessageEncoderTest, SyncEventToProtobuf_ModeChangeEvent)
{
  // Create a ModeChangeEvent
  ModeChangeEvent modeChangeEvent;
  modeChangeEvent.kind = PulseEventKind::EV_INT;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = modeChangeEvent;

  // Convert to Protobuf string
  auto encoder = MedIOMessageEncoder(42, 69);
  std::string protobufStr = encoder.SyncEventToProtobuf(syncEvent);

  // TODO: use decoder to check if it is a valid message
  ASSERT_FALSE(protobufStr.empty());
}

TEST_F(MedIOMessageEncoderTest, ToString)
{
  MedIOMessageEncoder builder(42, 69);

  // Convert to string
  std::string str = builder.ToString();

  // TODO: use decoder to check if it is a valid message
  ASSERT_FALSE(str.empty());
}

bool encoder_tester_called = false;
bool encoder_tester(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
  UNUSED(arg);
  UNUSED(field);
  UNUSED(stream);
  encoder_tester_called = true;
  return true;
}

TEST_F(MedIOMessageEncoderTest, SyncEventToProtobuf_CallsEncoders)
{
  // reset state
  encoder_tester_called = false;

  // Create a PulseEvent
  PulseEvent pulseEvent;
  pulseEvent.kind = PulseEventKind::EV_VLY;
  pulseEvent.duration = 100;
  pulseEvent.measurements = 42;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = pulseEvent;

  // Convert to Protobuf string
  auto encoder = MedIOMessageEncoder(42, 69, &encoder_tester);
  std::string protobufStr = encoder.SyncEventToProtobuf(syncEvent);

  ASSERT_TRUE(encoder_tester_called);
}