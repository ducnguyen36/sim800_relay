# A7680C Migration + EEPROM Caller Recognition — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace SIM800 + SIM-phonebook caller recognition with A7680C + a 20-entry EEPROM number table, on the STC15F2K60S2 door controller.

**Architecture:** Store up to 20 authorized numbers (9 VN digits + role byte) in SECTOR1 where call-history used to live. The serial ISR captures the incoming CLIP/SMS number into `phone[]`; the main loop scans the table (MOVC, <1 ms) and authorizes by role. RF-remote storage in SECTOR2 is untouched.

**Tech Stack:** C (SDCC 4.1.0), STC15 IAP EEPROM, A7680C AT commands over UART @57600.

## Global Constraints

- Build tool: SDCC **4.1.0** via `toolchain-sdcc@1.40100.12072`; do NOT change interrupt/`__at` syntax (4.4.0 breaks it). Local build: `scratchpad/build.sh`.
- Do NOT modify SECTOR2 / RF-remote logic (`eep_rfdata`, `eep_rfindex`, `eep_rflock`, `motor_cam_phim.c` RF decode).
- Roles: super `'M'`, master `'m'`, user `'u'`; empty slot byte = `0`.
- VN numbers stored as **9 digits, no leading 0**. CLIP arrives as `84xxxxxxxxx` or `0xxxxxxxxx`.
- A7680C only — no dual-module detection.
- No unit-test framework: **automated gate = build stays green**; each task lists a **hardware check** the author runs after flashing.
- Firmware release binary `.pio/build/.../firmware.hex` must not be committed as a local rebuild; `build.sh` restores it.

---

### Task 1: EEPROM layout — drop history, add phone table

**Files:**
- Modify: `src/eeprom.h` (SECTOR1 defines + `__code __at` views)
- Modify: `src/main.c` (boot EEPROM validation that references history index)

**Interfaces produced:**
- `#define PHONE_COUNT_EEPROM`, `#define PHONE_TABLE_EEPROM`, `#define PHONE_MAX 20`, `#define PHONE_ENTRY 10`
- `__code __at ... u8 eep_phone_count;` and `__code __at ... u8 eep_phone[200];`
- Removed: `INDEX_HISTORY_EEPROM`, `HISTORY_EEPROM`, `eep_index_history`, `eep_history`.

- [ ] **Step 1:** In `eeprom.h`, replace the history defines (`INDEX_HISTORY_EEPROM`, `HISTORY_EEPROM`) with `PHONE_COUNT_EEPROM = UPS_EEPROM+1`, `PHONE_TABLE_EEPROM = PHONE_COUNT_EEPROM+1`, `PHONE_MAX 20`, `PHONE_ENTRY 10`; set `SECTOR1_LENGTH = PHONE_TABLE_EEPROM + PHONE_MAX*PHONE_ENTRY`.
- [ ] **Step 2:** Replace the history `__code __at` views (`eep_index_history`, `eep_history[400]`) with `eep_phone_count` and `eep_phone[200]` at the matching MOVC addresses.
- [ ] **Step 3:** In `main.c` boot validation, delete the `eep_index_history`/history clamp lines; add a clamp `if(eeprom_buf[PHONE_COUNT_EEPROM]>PHONE_MAX) eeprom_buf[PHONE_COUNT_EEPROM]=0;`.
- [ ] **Step 4 (build gate):** `bash scratchpad/build.sh` → expect SUCCESS. It will still reference history in other files (compile errors there are expected and fixed in later tasks **only if** this task's files fail); scope this task so `eeprom.c`/`eeprom.h`/`main.c` boot compiles. If history calls elsewhere break the build, they are removed in Task 5 — so land Task 1+5 before a green full build, or temporarily stub. (See Task 5.)
- [ ] **Step 5:** Commit: `git commit -m "feat(eeprom): replace history region with phone table layout"`.

**Hardware check (later):** config (PIN, khoa, ups) still persists across reboot.

---

### Task 2: Phone-table access helpers

**Files:**
- Modify: `include/gsm_serial.c` (add helpers near top, after includes) OR `src/eeprom.c`.

**Interfaces produced:**
- `u8 phone_find(u8 *num9)` → returns index+1 if the 9-digit `num9` is in the table, else 0. Sets a global `u8 found_role` to the entry's role byte on hit.
- `__bit phone_add(u8 *num9, u8 role)` → write to first empty/oldest slot; returns 1 on success, 0 if full.
- `void phone_del(u8 idx)` → clear entry `idx` (1-based); `idx==0` clears all.
- `__bit have_master_table()` → 1 if any entry role is `'m'` or `'M'`.

- [ ] **Step 1:** Implement `phone_find` scanning `eep_phone[i*10 .. i*10+8]` for `i<eep_phone_count`, comparing 9 bytes; on match set `found_role = eep_phone[i*10+9]`.
- [ ] **Step 2:** Implement `phone_add`: `IAP_docxoasector1(); ` find empty slot (role byte 0) or append at `eep_phone_count`; copy 9 digits + role into `eeprom_buf[PHONE_TABLE_EEPROM + slot*10 ..]`; bump count; `IAP_ghisector1();`.
- [ ] **Step 3:** Implement `phone_del` and `have_master_table` similarly (RMW via sector1).
- [ ] **Step 4 (build gate):** `bash scratchpad/build.sh` → SUCCESS (helpers may be unused-warned; OK).
- [ ] **Step 5:** Commit: `git commit -m "feat: phone-table access helpers"`.

**Hardware check (later):** covered by Tasks 4–5 behavior.

---

### Task 3: A7680C AT-command changes in GSM setup

**Files:**
- Modify: `include/gsm_serial.c` (`gsm_thietlapngaygiothuc`, `gsm_thietlapnhantin`, `gsm_thietlapgoidien`, and the NORMAL-state `AT+CMGDA` send)

- [ ] **Step 1:** Replace both `AT+CMGDA="DEL ALL"` occurrences (setup + main-loop delete after SMS handled) with `AT+CMGD=1,4`.
- [ ] **Step 2:** In `gsm_thietlapngaygiothuc`, replace `AT+CLTS=1` with `AT+CTZU=1`; keep the `COPS=2`/`COPS=0`/`CCLK?` sequence.
- [ ] **Step 3:** Remove the `AT+CCALR?` step in `gsm_thietlapgoidien` (keep `CLIP=1`, return 1 after CLIP OK). Add `AT+IPR=0` once in `gsm_thietlapsim800` (after first `AT` OK).
- [ ] **Step 4 (build gate):** `bash scratchpad/build.sh` → SUCCESS.
- [ ] **Step 5:** Commit: `git commit -m "feat(gsm): A7680C AT-command set (CMGD/CTZU/IPR, drop CCALR)"`.

**Hardware check:** on power-up the LCD walks the setup screens without hanging; clock syncs (may need `COPS` time per reference note); SMS inbox clears.

---

### Task 4: Rework caller recognition (ISR capture + main-loop match)

**Files:**
- Modify: `include/gsm_serial.c` (`PHONE` and `CMD` serial states)
- Modify: `src/main.c` (`phone_update` handler; boot master check)

**Interfaces consumed:** `phone_find`, `found_role`, `have_master_table` (Task 2).

- [ ] **Step 1:** In the `PHONE` state, keep capturing the incoming digits into `phone[1..9]` after `+84`/`0`; on line end set `phone[10]=0` and a `__bit phone_ready` flag; drop the per-digit phonebook-name comparison. Set `phone_super/phone_master` NOT here (moved to main loop).
- [ ] **Step 2:** In the `CMD` state (SMS path), gate command execution on the sender being found in the table (`phone_find`) and PIN — replace the phonebook-name pin logic with: sender role from `found_role`; `phone_super` if role `'M'`.
- [ ] **Step 3:** In `main.c` `phone_update` (incoming call), call `phone_find(phone+1)`; if found and it's a call (`co_cuoc_goi_toi`), open door (reuse existing relay-open pulse) and set role flags; else ignore.
- [ ] **Step 4:** In `main.c` boot, set `have_master = have_master_table();` (replace `kiemtraphonemaster()`).
- [ ] **Step 5 (build gate):** `bash scratchpad/build.sh` → SUCCESS.
- [ ] **Step 6:** Commit: `git commit -m "feat: caller recognition via EEPROM table"`.

**Hardware check:** calling from a stored number opens the door within ~1 ring; calling from an unstored number does nothing.

---

### Task 5: SMS management commands + remove history & phonebook plumbing

**Files:**
- Modify: `include/xu_ly_tin_nhan.c` (`Lu`, `Xo`, `Db`, remove `H`, remove `luu_lich_su` calls)
- Modify: `include/gsm_serial.c` (remove `kiemtraphonemaster`, `gsm_themdanhba`, `xoadanhba`, `baocaodanhba`, `baocaolichsu`, `kiemtradanhba`; keep `send_pb_index` only if still used, else remove)
- Modify: `src/main.c` (remove `luu_lich_su` definition + all calls)

- [ ] **Step 1:** `Lu` command → parse `number,role`, strip leading `0`, call `phone_add(digits, role)`; reply success/full.
- [ ] **Step 2:** `Xo` command → `Xo,0` = clear all (`phone_del(0)`), `Xo,<n>` = `phone_del(n)`; reply.
- [ ] **Step 3:** `Db` command → iterate table, SMS each stored number + role via `send_gsm_cmd`/`send_pb_index`.
- [ ] **Step 4:** Delete the `H`/`h` history case; delete `luu_lich_su()` and every call to it (S, K, Xu, Le, phone-open paths).
- [ ] **Step 5:** Delete the now-unused phonebook functions in `gsm_serial.c`.
- [ ] **Step 6 (build gate):** `bash scratchpad/build.sh` → SUCCESS with no unresolved refs.
- [ ] **Step 7:** Commit: `git commit -m "feat: table-backed SMS commands; remove history + phonebook"`.

**Hardware check:** `Lu` adds a number (then that number can open by call); `Db` lists it; `Xo` removes it.

---

### Task 6: Version bump + final verification

**Files:** Modify `src/main.c` (`ver[]` string), any stale comments.

- [ ] **Step 1:** Bump `ver[]` (e.g. `" CUACUON A76-1.0"`).
- [ ] **Step 2 (build gate):** `bash scratchpad/build.sh` → SUCCESS; note final flash size.
- [ ] **Step 3:** Commit: `git commit -m "chore: version bump A7680C 1.0"`.

**Hardware check (full regression):** keypad menu, RF remote open/close/lock, UPS relay, SMS open/close/lock, call-open — all still work on the A7680C.

## Self-Review

- **Spec coverage:** AT changes → T3; EEPROM layout → T1; table helpers → T2; recognition flow → T4; SMS mgmt → T5; removals → T1/T5. All covered.
- **Ordering caveat:** T1 removes history symbols that T5 stops referencing. To keep an always-green build, land T1 together with the T5 history-removal (or stub `luu_lich_su` as an empty function in T1, deleted in T5). Chosen approach: **stub `luu_lich_su` to empty in T1**, delete in T5 — noted in T1 Step 4.
- **Type consistency:** `phone_find`→`found_role`; `phone_add(num9,role)`; `phone_del(idx)`; `have_master_table()` used consistently across T2/T4/T5.
