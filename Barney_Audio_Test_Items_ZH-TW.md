# Barney Audio 測試項目清單

整理日期：2026-11-24

## 文件範圍

本文件依使用者提供的 Barney 校正與測試盤點資料整理，未重新執行測試，也未獨立驗證原始程式或 log。聲學判定標準依該資料中的 SoundCheck Barney recipe；不代表所有歷史版本、AudioCalEngine（ACE）或 DAAT 使用相同規格。

本文編號僅供項目整理與比較，不是原程式測試 ID。量測、校正、複測、設備自檢與流程交付分開列示，不應將項目數加總為校正種類數。

## 1. 麥克風測試

測試對象：前左（FL）、前右（FR）、後左（RL）、後右（RR），共四個 DUT 麥克風。

| 編號 | 測試項目 | 測試目的／內容 | 判定標準或用途 |
|---|---|---|---|
| MIC-01 | 頻率響應測試 | 量測各麥克風在不同頻率的響應 | SoundCheck 實際保存資料的常見範圍為 100～8,000 Hz；不代表所有資料完整 |
| MIC-02 | 低／中頻通道一致性 | 比較四個麥克風在 400～2,000 Hz 的逐頻點最大差異 D | D ≤ 2.5 dB 通過；2.5 < D ≤ 6.5 dB 需校正；D > 6.5 dB 失敗 |
| MIC-03 | 高頻通道一致性 | 比較四個麥克風在 2,000～4,000 Hz 的逐頻點最大差異 D | D ≤ 4.4 dB 通過；4.4 < D ≤ 8.4 dB 需校正；D > 8.4 dB 失敗 |
| MIC-04 | 靈敏度測試 | 量測各麥克風的靈敏度 | Python 檢查不可為 None／NaN；未取得完整聲學性能上下限 |
| MIC-05 | Gain reference 量測 | 取得各麥克風的 1 kHz 參考值 | 用於計算增益補償 |
| MIC-06 | THD 失真量測 | 記錄 DUT 麥克風的失真資料 | 診斷用途；未確認完整 PASS／FAIL 門檻 |

## 2. 喇叭測試

測試對象：左、右，共兩個 DUT 喇叭。

| 編號 | 測試項目 | 測試目的／內容 | 判定標準或用途 |
|---|---|---|---|
| SPK-01 | 頻率響應測試 | 量測左右喇叭在不同頻率的響應 | SoundCheck 實際保存資料的常見範圍為 100～20,000 Hz；不代表所有資料完整 |
| SPK-02 | 左右通道一致性 | 比較左右喇叭在 400～3,150 Hz 的逐頻點差異 D | D ≤ 2.0 dB 通過；2.0 < D ≤ 6.0 dB 需校正；D > 6.0 dB 失敗 |
| SPK-03 | 靈敏度測試 | 量測各喇叭的靈敏度 | Python 檢查不可為 None／NaN，且須 > 0；不是完整聲學性能規格 |
| SPK-04 | Gain reference 量測 | 取得指定頻點響應的算術平均值 | 用於計算左右增益補償；歷史設定使用的頻點不同 |
| SPK-05 | THD 失真量測 | 記錄喇叭失真曲線 | 診斷用途；未確認完整 PASS／FAIL 門檻 |
| SPK-06 | Rub & Buzz 異音量測 | 分析喇叭異音與機械失真 | 診斷用途；未確認完整 PASS／FAIL 門檻 |
| SPK-07 | ePRB 分析 | 記錄額外異音分析曲線 | SoundCheck 有曲線；提供資料中的 ACE 欄位為空 |

目前 recipe 的 speaker gain reference 頻點為 400、500、630、800、1,000、1,250、1,600、2,000、2,500、3,150、4,000 Hz。歷史另有使用 1 kHz 與 2 kHz 的設定，不能視為所有 run 都相同。

## 3. 校正與校正後驗證

| 編號 | 項目 | 執行內容 | 判定標準或限制 |
|---|---|---|---|
| CAL-01 | 麥克風 Gain 校正 | 調整四個麥克風的個別增益 | 每通道補償幅度上限 2 dB |
| CAL-02 | 喇叭 Gain 校正 | 調整左右喇叭的個別增益 | 每通道補償幅度上限 3 dB |
| CAL-03 | 麥克風校正後複測 | 重新確認四通道頻響一致性 | 400～2,000 Hz：D ≤ 2.5 dB；2,000～4,000 Hz：D ≤ 4.4 dB |
| CAL-04 | 喇叭校正後複測 | 重新確認左右頻響一致性 | 400～3,150 Hz：D ≤ 2.0 dB 符合一般門檻；2.0 < D ≤ 3.0 dB 可按次級條件接受，且 mic 必須合格 |
| CAL-05 | 校正參數／Profile 交付 | 產生、推送並保存補償參數及 audio profile | 流程交付項目，不是聲學性能測試 |

### 判定方式

- D(f) = 同一頻點各通道響應的最大值 − 最小值。
- 四個 mic 互相比較；兩個 speaker 比較左右差異。
- 判定頻帶包含邊界，逐頻點檢查，不是只看平均值或 1 kHz。
- 2 kHz 同時落在兩個 mic 頻帶，須符合較嚴格的 2.5 dB。
- OK_GG：mic 合格，speaker 符合一般 2 dB 門檻。
- OK_SS：mic 合格，speaker 複測符合次級 3 dB 門檻。
- Gain 校正是每通道一個 scalar gain，不是逐頻率 EQ 校正，也不代表校正了 THD、噪音、極性或延遲。
- 1 kHz 配平不代表全頻帶合格；頻響形狀差異可能無法靠 scalar gain 修復。

## 4. 治具／參考設備自檢

測試對象：六支治具參考麥克風，不是 DUT 的四個 mic 加兩個 speaker。

| 編號 | 測試項目 | 測試內容 | 本份 SoundCheck recipe 標準 |
|---|---|---|---|
| STA-01 | 1 kHz Level 自檢 | 確認參考麥克風的量測音量 | mic1～4：93 < Level < 96；mic5～6：98 < Level < 100 |
| STA-02 | Self-noise 自檢 | 確認參考麥克風自噪音 | 六支皆須 28 < Noise < 39 |
| STA-03 | Polarity 記錄 | 記錄參考麥克風極性 | Python 未設獨立判定門檻 |
| STA-04 | 頻率響應記錄 | 保存六支參考麥克風的頻響曲線 | 記錄／診斷用途 |
| STA-05 | 自檢有效時間確認 | 確認近期曾成功完成自檢 | 一般 run 要求距最近成功自檢不超過 23 小時 |

Level 與 Noise 為 SoundCheck 自檢輸出；Noise 在原資料中依欄位／用途描述為 SPLA，Level 的精確單位仍應對照 sequence。自檢採嚴格不等號，等於上下限不算通過。其他引擎保存的自檢門檻不同，不應套用本表重判全部歷史結果。

## 5. 測試前置檢查

| 編號 | 檢查項目 | 檢查內容 |
|---|---|---|
| PRE-01 | DUT 身分確認 | Serial、HW ID、Firmware build、Station ID、SKU、硬體版本 |
| PRE-02 | 電池電量 | 必須 > 50%；等於 50% 不接受 |
| PRE-03 | 裝置連線／MCU 狀態 | ADB、port forwarding、MCU 啟停、Ping 與狀態確認 |
| PRE-04 | 測試環境準備 | 清除舊檔、準備音源與錄音、關閉 EQ、speaker volume 設為 100、避免休眠；完成後恢復 EQ／SystemUI 並釋放 wake lock |

前置檢查失敗不應直接歸類為 DUT 聲學性能不良。

## 6. 其他 OK2Cal 測試項目：尚未確認 Barney 已執行

以下是 Repo 中存在的測試方法分類，不是 Barney 已執行或已通過的清單。

| 類別 | 方法數 | 主要測試內容 |
|---|---:|---|
| Audio | 2 | 播放、錄音及格式檢查；不是 SoundCheck gain calibration |
| Camera | 6 | 資料生成、Metadata、Rolling shutter、掉幀、更新率、VGA 格式 |
| General | 2 | Direct channel 支援、資料收集工具 |
| Accelerometer | 9 | 原始資料、主／副 IMU 解析度、量測範圍、更新率、掉幀 |
| Gyroscope | 9 | 原始資料、主／副 IMU 解析度、量測範圍、更新率、掉幀 |
| Magnetometer | 4 | 原始資料、量測範圍、更新率、掉幀 |
| IMU temperature | 3 | 原始資料、主／副更新率 |
| Stability | 1 | 服務啟停與 ADB 連線穩定性 |
| Timestamp | 2 | Sensor 時間戳、Camera frame 對應延遲 |
| **合計** | **38** | 方法存在不代表實際執行；部分方法可依條件 skip |

未取得上述 38 個方法對 Barney 的完整執行成績，也沒有足夠資料將其他 camera／IMU／display 流程列為 Barney 已完成的校正。

## 7. 其他明確列出的 Limit／設定

以下補充原盤點資料中明確列出的數值。請區分「合格門檻」、「補償計算設定」與「資料格式／採集設定」；它們不是同一種 limit。

### 7.1 Gain 補償計算設定

| 參數 | 數值 | 用途／限制 |
|---|---|---|
| abs_max_mic_gain | 2.0 dB | 每個 mic 通道補償幅度上限 |
| abs_max_spk_gain | 3.0 dB | 每個 speaker 通道補償幅度上限 |
| max_mic_diff | 2.5 dB | Mic gain-reference 補償計算的剩餘差異目標；不取代全頻帶 cal-pass |
| max_spk_diff | 3.0 dB | Speaker gain-reference 補償計算的剩餘差異目標；不取代全頻帶 cal-pass，也不等於首測一般 2 dB 門檻 |

### 7.2 Camera／General／Timestamp 的 OK2Cal Limit

以下來自 Repo 的 OK2Cal 方法／設定，尚未取得 Barney 執行成績；不能當成 Barney 已驗證規格。

| 類別 | 測試項目 | 明確列出的 Limit／設定 | 備註 |
|---|---|---|---|
| Camera | Rolling shutter skew | 上限 33 ms | 原資料未明確提供等於上限時的比較方式 |
| Camera | Frame-drop rate | 上限 4% | Expected frames 依 20 秒、10 Hz 設定計算；等於上限時的比較方式待確認 |
| Camera | Refresh rate | 10 Hz，容許誤差 3% | 測試設定；可依 testbed／方法覆寫 |
| Camera | VGA 格式 | 640 × 480；PPM max value 255 | 格式檢查，不是校正參數 |
| General | Direct channel／收集工具 | 原資料未提供數值 limit | 功能／工具可用性檢查 |
| Timestamp | Sensor timestamp validation | Validation difference 上限 1 秒 | 等於上限時的比較方式待確認 |
| Timestamp | Camera 對應 frame delay | 原資料未提供數值 limit | 依資料條件可 skip |

### 7.3 IMU／Sensor 的 OK2Cal Limit 與設定

以下為可覆寫的 config／方法設定，不是 Barney 已確認的 sensor 清單，也不是校正後參數。

| Sensor | 解析度／格式 | 量測範圍設定 | 靈敏度／比例設定 | 更新率設定 | Expected files |
|---|---|---|---|---|---:|
| 主／副 Accelerometer | 16-bit | ±8 g | 至少 2,048 LSB/g | 200 Hz | 2 |
| 主／副 Gyroscope | 16-bit | 1,000 dps；原資料未明確描述正負範圍 | 至少 16 LSB/dps | 200 Hz | 2 |
| Magnetometer | 16-bit | 900 µT；原資料未明確描述正負範圍 | 0.15 µT/LSB | 100 Hz | 1 |
| IMU Temperature | 未提供 | 未提供 | 未提供 | 1 Hz | 1 |

| 測試項目 | 明確列出的 Limit／設定 | 備註 |
|---|---|---|
| Accel／Gyro／Mag drop rate | Factory build：3%；Google build：5% | 原始碼依 build type 使用不同門檻；等於門檻時的比較方式待確認 |
| Stability | 三輪，每輪兩次 service stop/start，並檢查 ADB 連線 | 執行次數設定，不是數值性能 limit |
| OK2Cal Audio capture | 16 kHz、mono、16-bit | 錄音格式設定，不是聲學合格門檻 |

### 7.4 引擎間的自檢設定差異

| 引擎／設定來源 | mic1～4 Level | mic5～6 Level | 備註 |
|---|---|---|---|
| 本份 SoundCheck recipe | 93 < Level < 96 | 98 < Level < 100 | 已知採嚴格不等號 |
| 保存的 ACE recipe | [92, 96] | [90, 98] | 原資料列出的設定值；括號不代表已確認 ACE 邊界接受規則 |

不得以 SoundCheck 自檢 limit 直接重判 ACE 歷史結果。ACE recipe 的 spec check／gain push 關閉，也不能據此推定 station 自檢同樣關閉。

### 7.5 尚未提供完整 Limit 的項目

THD、Rub & Buzz、ePRB、DUT 靈敏度性能上下限、Reference polarity、Reference FR，以及 Camera frame delay 尚未取得完整接受標準。表中的「未提供」不表示沒有規格或一定通過；需向對應規格／引擎擁有者確認。
