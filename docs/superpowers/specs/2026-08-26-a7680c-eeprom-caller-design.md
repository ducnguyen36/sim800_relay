# A7680C Migration + EEPROM Caller Recognition — Design

**Date:** 2026-08-26
**Target:** `sim800_relay` firmware (STC15F2K60S2, SDCC 4.1.0), house-door controller
**Branch:** `claude/a7680c-eeprom-caller`

## Problem

2G is being shut down in Vietnam, so the SIM800C module can no longer be used. The
replacement is the **A7680C** (4G/LTE). The A7680C's SIM phonebook is unreliable (the
author's own `new_dhcc` `gps_a7680c` branch abandoned it), so caller recognition must be
reworked. CLIP (caller ID) still works on the A7680C.

## Decision summary

| Decision | Choice |
|---|---|
| Number storage | **EEPROM table**, not SIM phonebook |
| Number format | **Full 9 digits** (VN number without leading `0`) + 1 role byte |
| Capacity | **20 numbers** (one table, ~201 bytes) |
| Call history | **Removed** entirely (frees the SECTOR1 history region) |
| Module support | **A7680C only** (no dual-module CGMM detection) |
| Roles | **Keep 3**: super `M`, master `m`, user `u` |
| Match timing | ISR captures digits, main loop scans table (<1 ms) |
| Door trigger | Open-on-RING via CLIP, hang up with `ATH` (caller not charged) |

## A7680C vs SIM800 command changes (from `gps_a7680c` reference)

- Delete all SMS: `AT+CMGDA="DEL ALL"` → **`AT+CMGD=1,4`**
- Clock: `AT+CLTS=1` → **`AT+CTZU=1`** (auto timezone); add **`AT+IPR=0`** in init
- Drop **`AT+CCALR?`** (call-ready check)
- Keep **`AT+CLIP=1`** (caller ID)
- Baud unchanged (57600)

## EEPROM layout (still 2 sectors, 512 B each)

**SECTOR1 (0x000)** — config + phone table (replaces history):

| Field | Offset | Size | Notes |
|---|---|---|---|
| `PIN_EEPROM` | 0 | 4 | unchanged |
| `BAOCAO_EEPROM` | 4 | 1 | unchanged |
| `HUONG_MOTOR` | 5 | 1 | unchanged |
| `KHOA_EEPROM` | 6 | 1 | unchanged |
| `UPS_EEPROM` | 7 | 1 | unchanged |
| `PHONE_COUNT_EEPROM` | 8 | 1 | number of stored phones (0..20) |
| `PHONE_TABLE_EEPROM` | 9 | 200 | 20 entries × 10 B |

Each phone entry (10 B): bytes 0–8 = 9 ASCII digits; byte 9 = role (`'M'`/`'m'`/`'u'`, or 0 = empty).

**SECTOR2 (0x200)** — RF remotes. **Unchanged.** (`eep_rfdata[300]`, `eep_rfindex`, `eep_rflock`.)

Removed from SECTOR1: `INDEX_HISTORY_EEPROM`, `HISTORY_EEPROM[400]`.

## Caller recognition flow

1. Incoming call → A7680C emits `RING` + `+CLIP:"84xxxxxxxxx",...`.
2. Serial ISR (`PHONE` state) captures the 9 significant digits into `phone[]` (strip `+84`
   or leading `0`), reusing today's digit-streaming machinery.
3. On line completion the ISR sets a "number ready" flag (no per-entry compare in the ISR —
   keeps it short).
4. Main loop (`phone_update` handler) scans the 20-entry table via MOVC (`eep_phone`), <1 ms:
   - match → read role → authorize per role → open door (open-on-RING semantics preserved).
   - no match → ignore.

Same for SMS: the CMD/PIN path validates the sender number against the table before executing.

## SMS command management (same command letters, table-backed)

- `Lu` (lưu): add `number,role` to the table (strip leading `0`, store 9 digits + role).
- `Xo` (xóa): delete by table index or by number; `Xo,0` clears all.
- `Db`: SMS back the stored numbers + roles.
- Master-exists check at boot: scan table for any `m`/`M` entry (replaces `kiemtraphonemaster`).

## Code removed / replaced

- **Removed:** `luu_lich_su()`, `baocaolichsu()`, `H`/`h` history SMS command, history EEPROM.
- **Replaced (phonebook → table):** `kiemtraphonemaster`, `gsm_themdanhba`, `xoadanhba`,
  `baocaodanhba`, and the `PHONE`/`CMD` ISR states that matched against the phonebook stream.

## Verification

- Build every step against SDCC 4.1.0 (`toolchain-sdcc@1.40100.12072`); keep it green.
- Flash/RAM budget is comfortable (was 23.4 KB/60 KB, 663 B/1792 B xdata); no pressure.
- Author flash-tests on real A7680C hardware between milestones (no HW test possible here).

## Out of scope

- GPS (present in the reference branch; explicitly excluded).
- Dual SIM800/A7680C support.
- RF remote logic (untouched).
