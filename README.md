SUN is short for Sevtlitsa Universal Node
by Eugene Svetlitsa
2026

## Local Wi-Fi configuration

Create `src/secrets.h` with your local Wi-Fi settings before building:

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define DEFAULT_WIFI_CHANNEL 13
```

The real `secrets.h` file and `pio_build.log` are excluded from Git; do not
commit credentials or build logs.
