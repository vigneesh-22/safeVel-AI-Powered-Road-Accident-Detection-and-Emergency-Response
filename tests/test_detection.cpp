#include <cassert>
#include <iostream>
#include "../src/detection_engine.h"

int main() {
  DetectionEngine engine(3.0f, 5.5f, 280.0f, 1000UL);
  assert(!engine.evaluate({0, 0, 1, 0, 0, 0, 10}).triggered);
  auto low = engine.evaluate({0, 0, 4.2f, 0, 0, 0, 20}); // |4.2g - 1g| = 3.2g
  assert(low.triggered && low.severity == Severity::Low);
  assert(!engine.evaluate({0, 0, 8.0f, 0, 0, 0, 30}).triggered); // cooldown
  auto high = engine.evaluate({0, 0, 8.0f, 0, 0, 0, 1100});
  assert(high.triggered && high.severity == Severity::High);
  DetectionEngine rotation(3.0f, 5.5f, 280.0f, 0UL);
  assert(rotation.evaluate({0, 0, 3.1f, 300, 0, 0, 1}).triggered);
  std::cout << "detection tests passed\n";
}

