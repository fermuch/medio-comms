#include <gtest/gtest.h>

#include <string>
#include <variant>
#include <iostream>
#include <sstream>

#include "suntech-parser.hpp"

class SuntechParserTest : public ::testing::Test
{
protected:
  void SetUp() override
  {}

  void TearDown() override
  {}
};

const char* msg = "ST300STT;511704164;45;315;20230921;18:27:48;60929;-25.526483;-054.568158;000.000;254.87;10;1;2169293;15.61;100010;2;0008;002922;0.0;1;00000000000000;0;10345;724;5;-78;1545;6\r\n";

TEST_F(SuntechParserTest, parse_valid)
{
  MessageParser parser;
  SuntechMessage expectedMsg = {
    .valid = true,
    .kind = "STT",
    .parts = {
      "ST300STT",
      "511704164",
      "45",
      "315",
      "20230921",
      "18:27:48",
      "60929",
      "-25.526483",
      "-054.568158",
      "000.000",
      "254.87",
      "10",
      "1",
      "2169293",
      "15.61",
      "100010",
      "2",
      "0008",
      "002922",
      "0.0",
      "1",
      "00000000000000",
      "0",
      "10345",
      "724",
      "5",
      "-78",
      "1545",
      "6"
    }
  };

  for (size_t i = 0; i < strlen(msg); ++i)
  {
    const auto received = parser.processByte(msg[i]);
    if (received.valid)
    {
      ASSERT_TRUE(received.valid);
      ASSERT_EQ(received.kind, expectedMsg.kind);
      ASSERT_EQ(received.parts.size(), expectedMsg.parts.size());
      for (size_t j = 0; j < received.parts.size(); ++j) {
        ASSERT_STREQ(received.parts[j].c_str(), expectedMsg.parts[j].c_str());
      }
      return;
    }
  }

  // should never reach here
  ASSERT_TRUE(false);
}


TEST_F(SuntechParserTest, get_valid_IO)
{
  MessageParser parser;

  for (size_t i = 0; i < strlen(msg); ++i)
  {
    const auto received = parser.processByte(msg[i]);
    if (received.valid)
    {
      ASSERT_TRUE(received.valid);
      ASSERT_EQ(parser.getIO(received, 0), 1);
      ASSERT_EQ(parser.getIO(received, 1), 0);
      ASSERT_EQ(parser.getIO(received, 2), 0);
      ASSERT_EQ(parser.getIO(received, 3), 0);
      ASSERT_EQ(parser.getIO(received, 4), 1);
      ASSERT_EQ(parser.getIO(received, 5), 0);
      ASSERT_EQ(parser.getIO(received, 6), -1);
      return;
    }
  }

  // should never reach here
  ASSERT_TRUE(false);
}