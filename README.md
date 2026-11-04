SUN is short for Sevtlitsa Universal Node
by Eugene Svetlitsa
2026

## Local Wi-Fi configuration

Create `src/secrets.h` with your local Wi-Fi settings before building:

```cpp
#pragma once

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define DEFAULT_WIFI_CHANNEL "YOUR_WIFI_CHANNEL"
#define SUN_PACKET_HMAC_KEY "CHANGE_ME"
```

Use a randomly generated secret with at least 32 characters, and configure the
same key in every SPACE, BEAM, and SUN device. The firmware authenticates bytes
0-241 of each 250-byte packet with HMAC-SHA-256 and stores the first 8 digest
bytes at offsets 242-249. The command timestamp remains at offsets 7-10 and is
echoed by SUN status packets as the receipt acknowledgement. Do not commit the
real `secrets.h` file or `pio_build.log`.

The node connects to the configured Wi-Fi network during startup and retries
every 30 seconds if disconnected. Set `DEFAULT_WIFI_CHANNEL` to the access
point's fixed 2.4 GHz channel, and use that same channel for every ESP-NOW
peer. OTA becomes available after Wi-Fi connects.

## ESP-NOW command packet

The 250-byte command packet keeps the common header and command values at
offsets 0-24. Byte 25 contains update flags (bit 0 updates `command` and
`parameter`; bit 1 applies selected `constantCommands`). Bytes 26-27 contain a
little-endian constant-command mask, where bit *n* selects
`constantCommands[n]`. Bytes 28-59 are a 256-node bitmap: node *n* is selected
by bit `n % 8` in byte `n / 8`. Bytes 60-241 are reserved and must be included
in the HMAC calculation. The final eight bytes are the truncated HMAC tag.

SUN only applies a packet after its HMAC is valid, its target bit is set, and
its packet type and TTL are valid. A status packet repeats the accepted
command timestamp at offsets 7-10, allowing SPACE to track per-node receipt
acknowledgements while retrying the same command timestamp.
