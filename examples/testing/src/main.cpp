
// UNIX startup header
#include <thread>
extern void setup();
extern void loop();
int main() {
  setup();

  for (;;) {
    loop();
    std::this_thread::yield();
  }
}

// Testing setup
// call with: pio test
#ifdef PIO_UNIT_TESTING
#include <gtest/gtest.h>
void setup() {
  ::testing::InitGoogleTest();
  exit(RUN_ALL_TESTS());
}
void loop() {}
#else
void setup() {}
void loop() {}
#endif // PIO_UNIT_TESTING