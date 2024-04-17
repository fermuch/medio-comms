# MedIO Communications

Library to handle common communications with MedIO systems.

# Setup

Add the library to your `platformio.ini` lib_deps:

```ini
lib_deps =
  https://github.com/fermuch/medio-comms.git
  nanopb/Nanopb@^0.4.7
```

**NOTE:** it is important to also add Nanopb, as it is used to compile the protobuf messages.

It is also needed to set up compilation of the ProtoBuf file.

First of all, copy the file `proto/v1/messages_medio.proto` into your project. Remember to update it when updating
this dependency!

Add the following to your `platformio.ini`:

```ini
custom_nanopb_protos =
	+<proto/v1/messages_medio.proto>
custom_nanopb_options =
	--error-on-unmatched
```

For a complete example check out the file `examples/testing/platformio.ini`.

# SuntechParser

SuntechParser is a parser for the Suntech protocol. It can be used to parse raw data from a serial port, byte by byte.

## Usage

```c++
#include <suntech-parser.hpp>

SuntechParser parser;

int setup() {}

int loop() {
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

int setup() {}

int loop() {
  while (Serial.available()) {
    const auto receivedMessage = parser.processByte(Serial.read());
    if (receivedMessage.valid) {
      Serial1.print("The state of IGNITION is: ");
      // We are reading IO 0, which is IGNITION
      Serial1.println(parser.getIO(receivedMessage, 0));
    }

    // do not forget to yield to not lock on large messages!
    yield();
  }
}
```

# MessageEncoder

**TODO!**