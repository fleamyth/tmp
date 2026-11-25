# Audio Station 失敗退出碼修正說明

文件日期：2026-11-25  
分析 commit：`33a4eb6679d543a76c358200521b352b57ea1999`  
Commit 主旨：`[Barney-DVT] Fix audio_station returning exit code 0 on failures`

## 1. 結論

**原本已經有回傳失敗 exit code 的功能，但部分失敗路徑沒有把錯誤狀態傳給主程式，因此可能出現「測試 FAIL，process exit code 卻是 0」。**

本次不是新增退出碼機制，而是修復狀態傳遞：

```text
修改前：
測試失敗 → log／DUT 物件有失敗資訊
         → 主程式檢查的欄位未更新
         → 沒有偵測到錯誤 → 正常退出 0

修改後：
測試失敗 → 更新 global_error 或登記 DUT 至 duts
         → 主程式偵測到錯誤 → 以非零退出碼結束
```

此外，修正自檢失敗仍更新「最近成功自檢時間」的問題，避免後續 run 把失敗的自檢當作有效成功紀錄。

本 commit 只修改 [audio_station.py](../stations/audio/audio_station.py)，共新增 10 行、刪除 4 行；沒有修改校正演算法、聲學規格或 gain 參數，也沒有新增任何 `sys.exit()`。

## 2. 原本就存在的退出碼機制

### 2.1 主程式看的是狀態，不是 log 文字

[main.py](../stations/audio/main.py#L79) 原本就有以下程式：

```python
duts = getattr(station, "duts", [])
duts_failed = any(len(getattr(dut, "failures", [])) > 0 for dut in duts) if duts else False

if getattr(station, "global_error", None):
    logger.error(f"Exiting with Error Code: {station.global_error.number}")
    sys.exit(station.global_error.number)
elif duts_failed:
    logger.error("Audio Station finished but one or more DUTs reported FAIL.")
    sys.exit(1)
```

正常退出的優先順序：

| 優先順序 | 主程式判斷 | 結果 |
|---|---|---|
| 1 | `global_error` 有值 | `sys.exit(global_error.number)` |
| 2 | 沒有站台錯誤，但 `duts` 內任一 DUT 的 `failures` 非空 | `sys.exit(1)` |
| 3 | 上述皆未偵測到，流程正常返回 | 正常退出碼 0 |

`logging.error()`、回傳 `False`、切換到 `Steps.END`，**都不會自動讓 process 以非零退出**。必須由呼叫端解讀結果、更新上述狀態，或走例外處理路徑。

### 2.2 站台錯誤碼與 DUT failure 不同

| 錯誤／情況 | 定義／處理 | 這些分支的退出碼 |
|---|---|---:|
| Audio smoke test 錯誤 | [AudioSmokeTestError](../src/utility/robocal_errors.py#L316) 的 `number=802` | 802 |
| 此處找不到 DUT | [AudioCalibrationFailureError](../src/utility/robocal_errors.py#L52) 的 `number=204` | 204 |
| DUT 物件已記錄測試失敗 | 主程式檢查 `dut.failures` | 1 |

例如 DUT failures 中即使存的是 Audio calibration error，這條主程式分支仍統一退出 **1**，不是自動退出 204。

`sys.exit()` 會以 `SystemExit` 結束執行；它不會被既有 `except Exception` 當作一般例外攔下改成其他碼。因此設定失敗退出後，不會再落到成功退出路徑而被覆蓋。

## 3. 一開始為什麼是錯誤的

### 3.1 單一 DUT 與 DUT 清單沒有接起來

[Station 基底類別](../src/base/station.py#L48) 先建立：

```python
self.duts = []
```

Audio Station 原本只建立：

```python
self.dut = audio_dut.AudioDUT(...)
```

這是兩個不同的 attribute。設定 `self.dut` 不會自動把物件加入 `self.duts`。

因此即使測試流程把錯誤寫入 `self.dut.failures`，主程式取得的 `station.duts` 仍是空清單，`duts_failed` 被算成 `False`。若同時沒有 `global_error`，便正常退出 0。

**根因：錯誤產生端使用單一 DUT 物件，但錯誤彙整端使用 DUT 清單，兩者缺少登記關係。**

### 3.2 結束流程不等於宣告失敗

Smoke test 未執行／過期、沒有 DUT 等分支，原本只記錄錯誤或切換 `Steps.END`。

`END` 表示流程需要收尾，可能是成功結束，也可能是失敗結束；不能只靠 state 名稱分辨。因為未設定 `global_error`，主程式不知道這次是因為錯誤而結束。

**根因：控制流程的結束狀態與業務上的成功／失敗狀態混淆。**

### 3.3 忽略 smoke test 的布林結果

原本：

```python
self.process.smoke_test()
```

即使被呼叫的方法正常回傳 `False`，呼叫端也沒有處理，仍然往結束流程走。

這類失敗不是必然會向外拋出例外，所以不能期待最外層例外處理自動回傳非零退出碼。

**根因：子流程用回傳值表示失敗，呼叫端卻沒有消費該結果。**

### 3.4 把「自檢結束時間」誤當成「自檢成功時間」

原本在 smoke-test `END` 狀態無條件更新 `LAST_SMOKE_TEST_RUN`。

後續 [自檢時效檢查](../src/base/station.py#L80) 使用這個時間作為放行依據，所以一次失敗的自檢仍可能刷新有效期。

**根因：成功紀錄的更新沒有以成功結果作為條件。**

以上是由程式差異可確認的技術原因；無法僅從這次 commit 推定原作者當時的意圖或開發背景。

## 4. 每一行修改的意義與必要性

本節行號均為 commit 修改後版本。

### 4.1 第 60 行：初始化錯誤狀態

[對應程式碼](../stations/audio/audio_station.py#L60)：

```python
self.global_error = None
```

| 面向 | 說明 |
|---|---|
| 修改意義 | 明確建立站台層級錯誤欄位，初始為尚無錯誤。 |
| 為何需要 | 後面的 smoke-test 結束流程會直接讀取 `self.global_error`，需要一個穩定初始值。 |
| 修改前問題 | 主程式用 `getattr(..., None)` 可以容忍欄位不存在，但站台本身沒有一致初始化的錯誤狀態。 |
| Exit code 影響 | 不直接改變退出碼；單獨加這行不會解決失敗退出 0。真正影響來自失敗分支對它賦值。 |

### 4.2 第 181 行：自檢先決條件不合格時登記錯誤

[對應程式碼](../stations/audio/audio_station.py#L181)：

```python
self.global_error = robocal_errors.AudioSmokeTestError
```

這行位於 `check_last_smoke_test_run()` 回傳 `False` 的分支。

| 面向 | 說明 |
|---|---|
| 修改意義 | 最近成功自檢不存在或已過期時，標示為站台錯誤。 |
| 為何需要 | 此時還沒建立 DUT，不能依賴 DUT failures 清單表示失敗。 |
| 修改前問題 | 只切到 END，正常收尾後主程式可能退出 0。 |
| Exit code 影響 | 正常完成收尾後，由 `global_error.number` 退出 **802**。 |

這是「開始 DUT 測試前的自檢時效門檻」，不同於後面的「執行 smoke test 本身失敗」。

### 4.3 第 192 行：沒有 DUT 時登記錯誤

[對應程式碼](../stations/audio/audio_station.py#L192)：

```python
self.global_error = robocal_errors.AudioCalibrationFailureError
```

| 面向 | 說明 |
|---|---|
| 修改意義 | ADB 找不到 DUT 時，登記站台層級失敗。 |
| 為何需要 | 沒有 DUT 就沒有 DUT failure 物件可供主程式檢查。 |
| 修改前問題 | 錯誤 log 不等於非零退出；只結束流程可能回傳 0。 |
| Exit code 影響 | 正常完成收尾後退出 **204**。 |

這裡存的是錯誤類別，沒有實例化或 `raise`；主程式直接讀取其類別屬性 `number`。

### 4.4 第 200 行：把實際 DUT 登記到清單

[對應程式碼](../stations/audio/audio_station.py#L200)：

```python
self.duts = [self.dut]
```

| 面向 | 說明 |
|---|---|
| 修改意義 | 讓主程式讀到的 DUT 清單包含本次實際使用的 DUT。 |
| 為何需要 | 主程式的 failure 彙整迴圈讀 `station.duts`，不是 `station.dut`。 |
| 修改前問題 | 實際 DUT 有 failures，但 `duts=[]`，因此誤算成沒有 DUT failure。 |
| Exit code 影響 | DUT failures 非空且沒有優先的 global error 時，退出 **1**。 |

清單內是同一個 DUT 物件，不是複製其當下狀態。因此之後新增至 `self.dut.failures` 的錯誤，也能從清單中的物件看到。

### 4.5 第 277 行：檢查 smoke test 回傳值

刪除原本：

```python
self.process.smoke_test()
```

改為 [第 277 行](../stations/audio/audio_station.py#L277)：

```python
if not self.process.smoke_test():
```

| 面向 | 說明 |
|---|---|
| 修改意義 | 保留執行自檢，但不再忽略結果；失敗時進入分支。 |
| 為何需要 | `False` 是普通回傳值，不會自動觸發外層例外處理或 process failure。 |
| 修改前問題 | 子流程失敗，但 station 未接收失敗訊號。 |
| Exit code 影響 | 本行不直接退出；需搭配下一行設定錯誤。 |

這個條件實際檢查的是 falsy 值，包括 `False`；直接拋出例外的情況則仍走既有例外處理路徑。

### 4.6 第 278 行：把自檢失敗轉成站台錯誤

[對應程式碼](../stations/audio/audio_station.py#L278)：

```python
self.global_error = robocal_errors.AudioSmokeTestError
```

| 面向 | 說明 |
|---|---|
| 修改意義 | 自檢回傳失敗時，將結果記錄在主程式可辨識的欄位。 |
| 為何需要 | Smoke-test 模式不依賴 DUT failures；必須有站台層級訊號。 |
| 修改前問題 | 主程式看不到自檢失敗，可能正常退出 0。 |
| Exit code 影響 | 正常完成收尾後退出 **802**。 |

第 277／278 行共同完成「布林失敗結果 → 站台錯誤狀態 → process 非零退出碼」的轉換。

### 4.7 第 281 行：成功才更新自檢時間

[對應程式碼](../stations/audio/audio_station.py#L281)：

```python
if self.global_error is None:
```

| 面向 | 說明 |
|---|---|
| 修改意義 | 為成功自檢時間的寫入增加條件。 |
| 為何需要 | 自檢結束不代表自檢成功。 |
| 修改前問題 | 失敗也刷新時間，後續可能把失敗當作有效自檢放行。 |
| Exit code 影響 | 不直接決定當次退出碼；當次失敗碼已由第 278 行決定。主要影響後續 run 的放行判斷。 |

### 4.8 第 282 行：時間寫入移到條件之內

[對應程式碼](../stations/audio/audio_station.py#L282)：

```python
self.config[constants.DYNAMIC][
```

- 選取 dynamic config。
- 表達式內容不變，變更在於增加縮排，納入第 281 行條件。
- 沒有獨立的退出碼影響。

### 4.9 第 283 行：成功時間欄位移到條件之內

[對應程式碼](../stations/audio/audio_station.py#L283)：

```python
    constants.LAST_SMOKE_TEST_RUN
```

- 指定最近成功自檢時間欄位。
- 這是跨行 assignment 的一部分，不是獨立執行的新指令。
- 除條件式控制外，欄位名稱與用途不變，沒有獨立退出碼影響。

### 4.10 第 284 行：僅符合條件才寫入目前時間

[對應程式碼](../stations/audio/audio_station.py#L284)：

```python
] = datetime.now()
```

- 與前兩行共同完成時間寫入。
- 原本每次走到 smoke-test END 都更新；現在只有沒有 global error 才更新。
- 本次不直接設定退出碼；後續 run 若缺少有效成功時間，會由第 181 行對應路徑退出 **802**。

重要限制：失敗時是「不更新」，不是「刪掉前一次成功時間」。如果先前仍有有效的成功自檢時間，這次修改本身不會強制使該時間失效。

## 5. 修改前後的行為比較

以下假設對應分支正常走完收尾，沒有另外發生檔案、設定保存或其他未處理例外。

| 情境 | 修改前可能結果 | 修改後程式預期 | 關鍵修改 |
|---|---|---|---|
| DUT 正常通過 | 0 | 0 | 保持正常行為 |
| DUT 已登記 failures | 0，因主程式讀到空清單 | **1** | 登記 `self.duts` |
| 最近成功 smoke test 不存在 | 0，只有結束流程 | **802** | 設定 smoke global error |
| 最近成功 smoke test 已過期 | 0，只有結束流程 | **802** | 同上 |
| ADB 沒有找到 DUT | 0，無 DUT 可提供 failures | **204** | 設定 calibration global error |
| 執行 smoke test 回傳 False | 0，且可能更新成功時間 | **802**，不刷新成功時間 | 檢查結果、設定錯誤、限制時間更新 |
| 執行 smoke test 成功 | 0，更新時間 | 0，更新時間 | 保持成功行為 |
| 子流程直接拋出未處理例外 | 由原本例外處理決定 | 仍由原本例外處理決定 | 不是本次新增的布林結果處理路徑 |

若 global error 與 DUT failures 同時存在，主程式優先使用 **global error 的編號**，不是 1。

## 6. 為什麼這些修改有必要

### 6.1 讓外部自動化取得正確結果

Station 內部顯示 FAIL，不代表呼叫它的程式知道失敗。若上層 bat、排程、測試系統或其他自動化以 process exit code 判斷結果，錯誤退出 0 就可能被當作成功。

本次修正讓已登記的失敗能透過既有退出碼介面回報；但上層是否正確保存／傳遞退出碼，仍是另一層需要驗證的事情。

### 6.2 避免未測就被當作成功

沒有 DUT、沒有有效自檢，都不是「測試合格」。即使流程沒有崩潰，也必須以失敗退出，才能讓上層分辨成功測試與被中止的測試。

### 6.3 保護自檢時間的語意

`LAST_SMOKE_TEST_RUN` 在此被當成最近成功自檢的放行依據。如果失敗也寫入，後續測試可能在沒有合格自檢的情況下繼續；因此不只需要修退出碼，也要同步修正時間紀錄。

## 7. 限制與驗證狀態

### 7.1 這份文件已確認的內容

- 已比對 commit 與其 parent 的實際差異。
- 已核對既有主程式的退出碼分支、基底類別 DUT 清單初始化與錯誤編號。
- 已逐一對照 10 行新增／替換後程式碼與原先 4 行刪除內容。
- 本文件是原始碼層級的行為分析，**不是重新執行硬體測試後的實測認證**。

### 7.2 不能因此宣稱的內容

- 不是所有 FAIL 字樣都一定導致非零退出；仍要有 failure 狀態登記、global error，或例外。
- 本 commit 只修正所列失敗路徑，不能據此保證整個 repo 所有錯誤路徑都已覆蓋。
- 當次收尾若另外拋出例外，最後退出碼可能改走例外處理，不一定是表中的 1／204／802。
- 802 是本 Windows 程式傳給 `sys.exit()` 的值；若外部 wrapper、遠端執行器或不同平台重新映射退出狀態，應以該執行環境實測確認，不能假設所有環境都原樣呈現 802。
- 不更新自檢時間，不代表自動取消之前仍有效的成功紀錄。
- 本次沒有更動 application code、沒有重新執行 DUT 校正，也未修改使用者提供的唯讀附件。

### 7.3 建議的執行驗收

| 驗收案例 | 應確認的 process exit code | 同時確認 |
|---|---:|---|
| DUT 正常通過 | 0 | 沒有 DUT failures／global error |
| 模擬 DUT failures 非空 | 1 | 實際 DUT 已存在於 `station.duts` |
| 自檢紀錄不存在／已過期 | 802 | 未執行 DUT 正式測量 |
| 無 DUT，其他前置條件符合 | 204 | 走到 no-device 分支 |
| Smoke test 回傳 False | 802 | 成功時間沒有刷新 |
| Smoke test 回傳 True | 0 | 成功時間有更新 |

可以用不接硬體的 mock 測試驗證狀態傳遞與退出碼；若要驗證部署 bat／執行器，則需另外確認其取得與向上回傳的 process exit code。以上為建議驗收案例，本次文件工作未執行這些故障注入測試。

## 8. 可供變更審查引用的摘要

> Audio Station 原本已有非零退出碼處理，但實際 DUT 未登記到主程式檢查的 DUT 清單，部分站台失敗亦未設定 global_error，且 smoke test 回傳值被忽略，導致失敗可能被誤判為正常結束並退出 0。本次將失敗狀態接回既有退出碼機制：DUT failures 回傳 1、自檢相關失敗回傳 802、無 DUT 回傳 204；並限制只有自檢成功才更新成功時間，避免失敗自檢刷新後續放行依據。此修改修復的是結果回報與放行邏輯，不改變聲學校正演算法或規格。


> Audio Station 原本就能通知上層系統「測試成功或失敗」，但因為部分失敗結果沒有正確傳遞，導致即使測試失敗、自檢不合格，或沒有連接待測裝置，上層系統仍可能收到「成功」的訊號。
這次修改讓這些情況能正確回報失敗，並用不同代碼區分原因：
1：待測裝置測試失敗。
802：站台自檢失敗，或缺少有效的自檢紀錄。
204：沒有連接待測裝置。
另外，只有自檢成功才會更新成功紀錄，避免失敗的自檢被當作已通過，讓後續測試被錯誤放行。