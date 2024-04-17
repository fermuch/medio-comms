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
  pulseEvent.duration = 100;
  pulseEvent.kind = PulseEventKind::EV_HOME;
  pulseEvent.measurements = 42;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = pulseEvent;

  // Convert to Protobuf string
  std::string protobufStr = MedIOMessageEncoder::SyncEventToProtobuf(42, 69, syncEvent);

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
  std::string protobufStr = MedIOMessageEncoder::SyncEventToProtobuf(42, 69, syncEvent);

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