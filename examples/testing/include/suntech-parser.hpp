#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>

struct SuntechMessage
{
  bool valid;
  std::string kind;
  std::vector<std::string> parts;
};

// trim from start (in place)
static inline void ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                  { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                       { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
  rtrim(s);
  ltrim(s);
}

class MessageParser
{
private:
  static constexpr size_t BUFFER_SIZE = 512; // XXX: Might need to be adjusted in the future
  char buffer[BUFFER_SIZE] = {};
  size_t currentIndex = 0;
  bool isReceiving = false;

  // Check if the buffer contains a valid message
  bool isValidMessage() const
  {
    return (currentIndex > 7) &&
           (buffer[currentIndex - 2] == '\r') &&
           (buffer[currentIndex - 1] == '\n');
  }

public:
  // Process a byte and return a pointer to the message if it's valid
  SuntechMessage processByte(char b)
  {
    SuntechMessage msg;
    msg.valid = false;

    // If we're not currently receiving a message, check for the "ST300" prefix
    // since all suntech messages start with that
    if (!isReceiving)
    {
      if (currentIndex < 5)
      {
        if (b == "ST300"[currentIndex])
        {
          buffer[currentIndex++] = b;
        }
        else
        {
          currentIndex = 0; // Reset if the sequence is broken
        }
      }
      if (currentIndex == 5)
      {
        isReceiving = true;
        // No need to copy or reset currentIndex here
      }
    }
    else // If we're receiving a message, append the byte to the buffer
    {
      if (currentIndex >= BUFFER_SIZE - 1)
      {
        // Buffer overflow, reset everything
        currentIndex = 0;
        isReceiving = false;
        return msg;
      }
      buffer[currentIndex++] = b;

      // Check for a valid message
      if (isValidMessage())
      {
        buffer[currentIndex] = '\0'; // Null-terminate the message
        currentIndex = 0;            // Reset the index for the next message
        isReceiving = false;         // Reset the receiving flag

        msg.kind = std::string(buffer + 5, 3);

        const char *start = buffer;
        const char *end = strchr(start, ';'); // Find the first semicolon
        while (end)
        {
          msg.parts.push_back(std::string(start, end - start).c_str());
          start = end + 1;
          end = strchr(start, ';');
        }
        if (*start) // Add the last part if there's no semicolon at the end
        {
          std::string lastPart = std::string(start);
          trim(lastPart);
          msg.parts.push_back(lastPart.c_str());
        }

        msg.valid = true;
        return msg;
      }
    }

    return msg; // Returns null if no valid message is found
  }

  // -1 means invalid, 0 is false and 1 is true
  int getIO(const SuntechMessage& msg, std::size_t io) {
    if (!msg.valid || msg.kind != "STT" || msg.parts.size() < 15) {
      return -1;
    }

    // Example: "100010"
    const auto &part = msg.parts[15];
    auto ioCount = part.size();
    if (io > (ioCount - 1)) {
      return -1;
    }

    return part[io] == '1' ? 1 : 0;
  }
};
