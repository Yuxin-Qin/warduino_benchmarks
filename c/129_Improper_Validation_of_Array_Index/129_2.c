#include "wasm_layout.h"

/*
 * CWE-129:
 * Only checks index >= 0, not < size.
 */
void start(void) {
    int sensor_values[8];
    int sensor_id = 15;
    int reading   = -1;

    for (int i = 0; i < 8; i++) {
        sensor_values[i] = i * 10;
    }

    if (sensor_id >= 0) {
        reading = sensor_values[sensor_id];  /* out-of-bounds */
    }

    print_int(reading);
}
