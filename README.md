# MedIO Communications

Library to handle common communications with MedIO systems in a PlatformIO environment.

Supports both native (UNIX) and ESP32 platforms.

# Table of content

-   [MedIO Communications](#medio-communications)
-   [Setup](#setup)
    -   [Set up the ProtoBuf file](#set-up-the-protobuf-file)
-   [SuntechParser](#suntechparser)
    -   [Usage](#usage)
    -   [Read IO](#read-io)
-   [MessageEncoder](#messageencoder)
    -   [Usage: quick method](#usage-quick-method)
    -   [Usage: advanced method with
        callbacks](#usage-advanced-method-with-callbacks)
        -   [How to send it to Suntech?](#how-to-send-it-to-suntech)
        -   [Do I need to always send a
            SyncEvent?](#do-i-need-to-always-send-a-syncevent)
        -   [Where can I see the other
            events?](#where-can-i-see-the-other-events)

# Setup

Add the library to your `platformio.ini` lib_deps:

```ini
lib_deps =
  https://github.com/fermuch/medio-comms.git
  nanopb/Nanopb@^0.4.7
```

**NOTE:** it is important to also add Nanopb, as it is used to compile the protobuf messages.

### Set up the ProtoBuf file

First of all, copy the file `proto/v1/messages_medio.proto` into your project.

**NOTE:** Remember to update the file when updating this library!

Add the following to your `platformio.ini`:

```ini
custom_nanopb_protos =
	+<proto/v1/messages_medio.proto>
custom_nanopb_options =
	--error-on-unmatched
```

For a complete example check out the file `examples/testing/platformio.ini`.

**NOTE:** You might need to switch to C++20 to use some features.

# SuntechParser

SuntechParser is a parser for the Suntech protocol. It can be used to parse raw data from a serial port, byte by byte.

## Usage

```c++
#include <suntech-parser.hpp>

SuntechParser parser;

void setup() {}

void loop() {
  while (Serial.available()) {
    const auto receivedMessage = parser.processByte(Serial.read());
    if (receivedMessage.valid) {
      // handle the message here
      Serial1.print("Received message of kind: ");
      Serial1.println(receivedMessage.kind);
    }

    // do not forget to yield to not lock on large messages!
    yield();
  }
}
```

## Read IO

There is a helper to read IO values from standard messages (ST300STT).

The returned values are:

* **-1:** Error reading that value
* **0:** IO is OFF
* **1:** IO is ON

```c++
#include <suntech-parser.hpp>

SuntechParser parser;

void setup() {}

void loop() {
  while (Serial.available()) {
    const auto receivedMessage = parser.processByte(Serial.read());
    if (receivedMessage.valid) {
      Serial1.print("The state of IGNITION is: ");
      // We are reading IO 0, which is IGNITION
      Serial1.println(parser.getIO(receivedMessage, 0));
    }

    // Note: you can request an STT to the Suntech device sending it the following message:
    // Serial.write("SttReq\r\n");

    // do not forget to yield to not lock on large messages!
    yield();
  }
}
```

# MessageEncoder

MessageEncoder converts MedIO events into its codified version, returning a string.

## Usage: quick method

```C++
#include <medio-message-encoder.hpp>

void setup() {}

void loop() {
  // Create a ModeChangeEvent
  ModeChangeEvent modeChangeEvent;
  modeChangeEvent.kind = PulseEventKind::EV_INT;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = modeChangeEvent;

  // Convert to Protobuf string
  std::string protobufStr = MedIOMessageEncoder::SyncEventToProtobuf(
    /* the version of our board */
    42,
    /* the current boot id, set at random when booting up */
    /* this is also used by the server to aggregate sessions, so keep it the same if the board did not restart! */
    23,
    /* the event to encode */
    syncEvent
  );

  Serial.print("I've encoded a ModeChangeEvent, and here it is: ");
  Serial.println(protobufStr);

  delay(5000);
}
```

## Usage: advanced method with callbacks

```C++
#include <medio-message-encoder.hpp>

void setup() {}

void loop() {
  // Create a new encoder
  MedIOMessageEncoder encoder(__BOARD_VERSION_U32__, state.bootId, encode_counters, encode_stored_counters, encode_stored_pulses);

  // Create a ModeChangeEvent
  ModeChangeEvent modeChangeEvent;
  modeChangeEvent.kind = PulseEventKind::EV_INT;

  // Wrap it in a SyncEvent
  SyncEvent syncEvent;
  syncEvent.payload = modeChangeEvent;

  // Convert to Protobuf string
  std::string protobufStr = encoder.SyncEventToProtobuf(
    /* the version of our board */
    42,
    /* the current boot id, set at random when booting up */
    /* this is also used by the server to aggregate sessions, so keep it the same if the board did not restart! */
    23,
    /* the event to encode */
    syncEvent
  );

  Serial.print("I've encoded a ModeChangeEvent, and here it is: ");
  Serial.println(protobufStr);

  delay(5000);
}
```

### How to send it to Suntech?

After encoding a message as a string, send it directly to the Suntech device and append a "\r\n" at the end.

Most Suntech devices can store between 100-500 messages offline and send them when a connection is available.

### Do I need to always send a SyncEvent?

Yes. SyncEvent is the only kind of event the server can receive. It embeds metadata (as stored pulses, or board ID)
and sends the inner event.

### Where can I see the other events?

Take a look at the protobuf file, or at the events declaration (`medio-events.hpp`).