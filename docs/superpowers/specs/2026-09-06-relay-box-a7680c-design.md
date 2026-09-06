# Thiết kế: Hộp 4 relay độc lập trên nền A7680C

**Ngày:** 2026-09-06
**Nhánh:** `claude/relay-onoff` (viết lại từ base `origin/master`)
**Trạng thái:** Đã chốt với người dùng, chờ viết plan triển khai.

## 1. Bối cảnh & mục tiêu

Firmware master hiện tại (`CUACUON A7-1.6`) là bộ điều khiển **cửa cuốn**: Relay1/2/3
là relay motor **momentary** (bật rồi tự tắt), bị bộ giải mã RF trong `PCA_Handler`
tự động ngắt khi hết tín hiệu; Relay4 là UPS; có khóa cửa, hướng motor.

Người dùng muốn một sản phẩm **khác**: **hộp 4 relay độc lập** (điều khiển bơm,
đèn, thiết bị) trên cùng nền phần cứng + A7680C, **giữ lại toàn bộ hạ tầng
GSM/SMS/menu/PIN/đăng ký số/học remote**, nhưng đổi bản chất relay từ momentary
(cửa) sang **giữ trạng thái (hold) độc lập**.

Đây KHÔNG phải cải tiến cửa cuốn — là một dòng sản phẩm riêng. Nhánh này thay thế
hoàn toàn bản "buttons-only, gut GSM + auto-off 10s" đã commit nhầm trước đó
(commit `db8f442`) — bản đó sẽ bị bỏ (reset nhánh về master rồi làm lại).

## 2. Phạm vi

### 2.1 Giữ nguyên (hạ tầng, không đổi hành vi)
- GSM/A7680C init & thiết lập (COPS, CMGF, CNMI, CLIP, CSQ).
- Bảng số điện thoại trong EEPROM (vai trò M/m/u), đăng ký số bằng **gọi đến** và
  bằng **SMS "luu"/"save"** (kèm màn hình xác nhận số trên LCD như A7-1.x).
- Menu + PIN 4 số, đổi PIN.
- Báo cáo (báo cáo khởi động, xác nhận thao tác qua SMS).
- Lệnh quản lý giữ nguyên: `Pin`, `Luu`, `Xoa`, `D`, `BC`, `R`, `Cg`, `Rr`, `B` (ping).
- Học remote RF (lưu ID remote vào SECTOR2).

### 2.2 Gỡ bỏ (đặc thù cửa cuốn)
- Logic motor momentary + biến `run_button` ở màn hình chính.
- **Tự-ngắt relay trong `PCA_Handler` CCF1** (dòng `rfstop = 1; Relay1 = Relay3 = 0;
  Relay2 = relay2giu;`) — BẮT BUỘC gỡ, nếu không relay không giữ được trạng thái.
- Khóa cửa: `relay2giu`, `eep_khoa`, các đường liên quan Relay2 làm relay khóa.
- UPS trên Relay4: `eep_ups`, đếm ngược tắt UPS, `Relay4 = eep_ups?1:0` lúc boot.
- Hướng motor: `eep_huong` và toàn bộ nhánh chọn Relay1/Relay3 theo hướng.
- Lệnh SMS cửa: `Le`/`Len` (mở), `Xu`/`Xuong` (đóng), `S` (dừng), `K` (khóa),
  `Kp` (khóa phím vẫn có thể giữ — xem mở rộng), lệnh mở cửa khi có cuộc gọi đến.
- RF-lock (`rflock`) và remote khẩn cấp (`rf_khancap`, khe emergency) — đặc thù an ninh cửa.
- Mục menu **HƯỚNG MOTOR**.

> Ghi chú: giữ lại `Kp` (khóa bàn phím) là tùy chọn hợp lý về bảo mật; plan sẽ
> quyết định giữ hay bỏ. Mặc định đề xuất: **giữ** `Kp` vì không liên quan relay.

## 3. Mô hình relay

- 4 relay độc lập: `Relay1`(P23), `Relay2`(P22), `Relay3`(P21), `Relay4`(P42).
- Mỗi relay là **hold** (0/1), chỉ bị đổi bởi: nút bấm, remote RF, hoặc SMS.
  **Không** đường nào khác được ghi đè (đã gỡ ở 2.2).
- **Persist trạng thái vào EEPROM** và khôi phục khi khởi động:
  - Dùng 1 byte trong SECTOR1 làm bitmask 4 relay (bit0=R1 … bit3=R4).
  - Tái dùng offset đã giải phóng (đề xuất `UPS_EEPROM`, đổi alias thành
    `RELAY_STATE_EEPROM`) để **không làm dịch layout bảng số**.
  - Ghi read-modify-write SECTOR1 **mỗi khi trạng thái relay đổi** (tần suất do người
    bấm/điều khiển → thưa, an toàn với độ bền IAP ~100k ghi).
  - Lúc boot: đọc byte này, set 4 relay theo bitmask (thay cho "tất cả OFF").

## 4. Giao diện nút bấm

### 4.1 Màn hình chính (mode 0)
- **LCD hiển thị:** giờ + ngày (như cũ) **+ trạng thái 4 relay + phiên bản (ver)**.
  Bố cục 16x2: dòng trên giờ:phút:giây; dòng dưới xen ngày, trạng thái relay
  (vd `R:1010`) và nhét chuỗi ver. Plan chốt bố trí ký tự cụ thể cho vừa 16 cột.
- **`+`**: nếu **cả 4 relay đang ON** → tắt hết; ngược lại → **bật hết**. (Ghi EEPROM.)
- **`B`**: vào **màn hình panel** (mode mới, xem 4.2).
- **`M`**: vào nhập PIN → menu (không đổi so với master).

### 4.2 Màn hình panel 4 relay (mode mới — đề xuất `mode == 5`)
- LCD hiện trạng thái 4 relay; **relay đang chọn nhấp nháy** (dùng `chop`/`LCD_blinkXY`).
- **`B`**: chuyển chọn sang relay kế tiếp, vòng R1→R2→R3→R4→R1.
- **`+`**: bật/tắt relay đang chọn (ghi EEPROM).
- **`M`**: thoát về màn hình chính.
- **Timeout ~15s** kể từ phím cuối: tự thoát về màn hình chính (tái dùng cơ chế
  `mode_wait` đang có, đặt lại 15 mỗi lần bấm phím trong panel).

## 5. Remote RF

- Chỉ remote **đã học** (ID khớp bảng SECTOR2) mới điều khiển.
- Mỗi frame cho 4 bit nút `cmd[0..3]` + ID 3 byte `data[0..2]`. Bình thường mỗi lần
  bấm chỉ 1 nút active.
- **Ánh xạ nút → relay (toggle/hold):**
  - lên → **R1**
  - xuống → **R2**
  - khóa → **R3**
  - stop → **R4**
- **Toggle theo cạnh nhấn:** bấm 1 lần bật, bấm lần nữa tắt. Chỉ toggle khi là
  **nhấn mới** (rising edge), không đảo liên tục khi giữ nút.
  - Cơ chế: cờ `rf_dang_giu`. Khi nhận frame khớp và `rf_dang_giu==0` → áp toggle cho
    nút đang active rồi đặt `rf_dang_giu=1`. Reset `rf_dang_giu=0` khi RF timeout
    (chỗ `rfwait` wrap / `rfstop` trong CCF1) — tức khi nhả nút.
  - Mỗi toggle → cập nhật relay + ghi EEPROM.
- **Học remote:** giữ luồng học trong menu (mode 2). Bỏ khe emergency/RF-lock.
- **Kiểm chứng bit↔nhãn nút:** code đã gửi `cmd[0..3]` ra serial (dòng debug 'P').
  Ánh xạ cmd-bit ↔ nhãn (lên/xuống/khóa/stop) phải **xác nhận trên phần cứng thật**
  với remote thực tế (đề xuất ban đầu: lên=cmd[1], xuống=cmd[3], khóa=cmd[0], stop=cmd[2]).

## 6. Lệnh SMS điều khiển relay

Cú pháp chung `<PIN>,<lệnh>` như cũ; người gửi phải là số đã lưu.
- `0000,R1,ON` / `0000,R1,OFF` … tới `R4` — bật/tắt từng relay (quyền: master/user — chốt ở plan, đề xuất cho cả user).
- `0000,ALL,ON` / `0000,ALL,OFF` — bật/tắt cả 4.
- `0000,B` (ping) → nhắn lại **trạng thái 4 relay** (vd `R1:1 R2:0 R3:1 R4:0`).
- Mỗi lệnh relay thành công → có thể nhắn xác nhận trạng thái (tùy `eep_baocao`).
- **Không** tự nhắn mỗi lần bấm nút/remote (tránh spam + tốn cước).
- Ghi EEPROM khi SMS đổi trạng thái relay.

## 7. Tự lấy lại giờ

Vấn đề master: `gsm_thietlapngaygiothuc()` chỉ chạy lúc boot 1 lần và mỗi giờ
(`gio_out`); nếu lần đó module chưa lấy được giờ mạng (NITZ) thì bỏ qua, đợi tới
giờ sau — có thể rất lâu mới có giờ đúng.

- Định nghĩa **giờ hợp lệ**: `year` trong khoảng hợp lý (đề xuất `year >= 24 && year <= 99`
  và `month` 1–12). Ngưỡng cụ thể chốt ở plan.
- Thêm đếm ngược `gio_retry` (giây) trong `xunggiay()`. Khi giờ **chưa hợp lệ** và
  `gio_retry` về 0 → đặt cờ `lay_lai_gio=1`, nạp lại `gio_retry = 180` (~3 phút).
- Vòng lặp chính: nếu `lay_lai_gio` → xóa cờ, gọi `gsm_thietlapngaygiothuc()`
  (KHÔNG gọi GSM trong ISR).
- Khi đã có giờ hợp lệ: ngừng retry nhanh; **vẫn giữ làm mới mỗi giờ** như master.
- Lưu ý: mỗi lần lấy giờ có `COPS=2`/`COPS=0` (rời/vào mạng ngắn) → có thể lỡ SMS/
  cuộc gọi trong khoảnh khắc đó; chấp nhận vì chỉ lặp cho tới khi có giờ.

## 8. Version

- Đổi chuỗi `ver` sang dòng sản phẩm riêng để không lẫn với dòng cửa cuốn.
- Đề xuất: `" RELAY4 RL-1.0"` (hoặc tên người dùng chọn). Chốt lúc release.

## 9. EEPROM — thay đổi layout

Không dịch bảng số. Tái dùng byte đã giải phóng:
- `HUONG_MOTOR`, `KHOA_EEPROM`, `UPS_EEPROM` trở nên không dùng.
- Định nghĩa `RELAY_STATE_EEPROM` = offset của `UPS_EEPROM` (giữ nguyên vị trí), lưu
  bitmask 4 relay.
- `BAOCAO_EEPROM`, `PIN_EEPROM`, `PHONE_COUNT_EEPROM`, bảng số: **giữ nguyên**.
- SECTOR2 (remote): giữ nguyên, bỏ dùng `RFLOCK_EEPROM`.

## 10. Rủi ro / điểm cần test trên phần cứng

1. Gỡ tự-ngắt CCF1 rồi relay phải **giữ ổn định** (không nhiễu RF làm bật/tắt sai).
   Vẫn phải bảo đảm bộ giải mã RF hoạt động cho việc học + toggle.
2. Toggle remote theo cạnh nhấn: không double-toggle khi giữ, không miss khi bấm nhanh.
3. Ánh xạ cmd-bit ↔ nhãn nút remote (xem 5) — xác nhận với remote thật.
4. Độ bền EEPROM khi ghi mỗi lần đổi relay (chấp nhận vì thao tác thưa).
5. Bố cục LCD 16 cột chứa đủ giờ/ngày/trạng thái relay/ver.
6. Tự lấy lại giờ không gây rớt mạng kéo dài ảnh hưởng nhận SMS.

## 11. Ngoài phạm vi (YAGNI)

- Không hẹn giờ tự tắt relay (auto-off) — người dùng đã yêu cầu bỏ.
- Không lịch bật/tắt theo thời gian thực.
- Không điều khiển qua cuộc gọi (gọi đến chỉ dùng để đăng ký số master/user như cũ;
  KHÔNG mở relay khi có cuộc gọi — đã gỡ hành vi mở cửa khi gọi).
