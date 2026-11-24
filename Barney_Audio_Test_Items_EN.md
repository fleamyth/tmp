# Barney Audio Test Item List

Prepared on: 2026-11-24

## Scope

This document summarizes the user-provided Barney calibration and test inventory. No tests were rerun, and the original source code or logs were not independently verified. Acoustic acceptance criteria are based on the SoundCheck Barney recipe described in that material; they do not imply identical specifications across historical versions, AudioCalEngine (ACE), or DAAT.

The IDs below are organizational labels, not test IDs from the original software. Measurements, calibration, retests, station self-tests, and deliverables are listed separately. Their counts must not be added together as a count of calibration types.

## 1. Microphone Tests

Test subjects: four DUT microphones: front left (FL), front right (FR), rear left (RL), and rear right (RR).

| ID | Test Item | Purpose / Content | Acceptance Criteria or Use |
|---|---|---|---|
| MIC-01 | Frequency response measurement | Measure each microphone's response across frequencies | Common range in saved SoundCheck data: 100-8,000 Hz; completeness is not guaranteed for every record |
| MIC-02 | Low/mid-frequency channel consistency | Compare the maximum point-by-point difference D among four microphones over 400-2,000 Hz | D <= 2.5 dB: pass; 2.5 < D <= 6.5 dB: calibration required; D > 6.5 dB: fail |
| MIC-03 | High-frequency channel consistency | Compare the maximum point-by-point difference D among four microphones over 2,000-4,000 Hz | D <= 4.4 dB: pass; 4.4 < D <= 8.4 dB: calibration required; D > 8.4 dB: fail |
| MIC-04 | Sensitivity measurement | Measure each microphone's sensitivity | Python checks that values are not None/NaN; complete acoustic performance limits were not provided |
| MIC-05 | Gain reference measurement | Obtain the 1 kHz reference value for each microphone | Used to calculate gain compensation |
| MIC-06 | THD measurement | Record DUT microphone distortion data | Diagnostic use; complete PASS/FAIL limits have not been confirmed |

## 2. Speaker Tests

Test subjects: two DUT speakers: left and right.

| ID | Test Item | Purpose / Content | Acceptance Criteria or Use |
|---|---|---|---|
| SPK-01 | Frequency response measurement | Measure left/right speaker response across frequencies | Common range in saved SoundCheck data: 100-20,000 Hz; completeness is not guaranteed for every record |
| SPK-02 | Left/right channel consistency | Compare the point-by-point difference D between speakers over 400-3,150 Hz | D <= 2.0 dB: pass; 2.0 < D <= 6.0 dB: calibration required; D > 6.0 dB: fail |
| SPK-03 | Sensitivity measurement | Measure each speaker's sensitivity | Python checks that values are not None/NaN and are > 0; this is not a complete acoustic performance specification |
| SPK-04 | Gain reference measurement | Calculate the arithmetic mean of responses at specified frequencies | Used to calculate left/right gain compensation; reference frequencies differ across historical configurations |
| SPK-05 | THD measurement | Record speaker distortion curves | Diagnostic use; complete PASS/FAIL limits have not been confirmed |
| SPK-06 | Rub & Buzz measurement | Analyze speaker abnormal noise and mechanical distortion | Diagnostic use; complete PASS/FAIL limits have not been confirmed |
| SPK-07 | ePRB analysis | Record additional abnormal-noise analysis curves | Curves are available in SoundCheck; the ACE fields in the supplied data are empty |

The current recipe uses speaker gain reference frequencies of 400, 500, 630, 800, 1,000, 1,250, 1,600, 2,000, 2,500, 3,150, and 4,000 Hz. Historical configurations also used 1 kHz and 2 kHz; this setting must not be assumed identical for all runs.

## 3. Calibration and Post-Calibration Verification

| ID | Item | Action | Acceptance Criteria or Limitations |
|---|---|---|---|
| CAL-01 | Microphone gain calibration | Adjust individual gains for four microphones | Maximum compensation magnitude: 2 dB per channel |
| CAL-02 | Speaker gain calibration | Adjust individual gains for left/right speakers | Maximum compensation magnitude: 3 dB per channel |
| CAL-03 | Microphone post-calibration retest | Recheck frequency response consistency across four channels | 400-2,000 Hz: D <= 2.5 dB; 2,000-4,000 Hz: D <= 4.4 dB |
| CAL-04 | Speaker post-calibration retest | Recheck left/right frequency response consistency | Over 400-3,150 Hz: D <= 2.0 dB meets the primary limit; 2.0 < D <= 3.0 dB may be accepted under the secondary criterion, provided microphones pass |
| CAL-05 | Calibration parameter / profile delivery | Generate, push, and save compensation parameters and the audio profile | A workflow deliverable, not an acoustic performance test |

### Decision Rules

- D(f) = maximum channel response minus minimum channel response at the same frequency.
- Compare all four microphones with one another; compare the two speakers left versus right.
- Acceptance bands include their endpoints. Checks are point-by-point, not based only on an average or on 1 kHz.
- The 2 kHz point belongs to both microphone bands and must satisfy the stricter 2.5 dB limit.
- OK_GG: microphones pass and speakers meet the primary 2 dB limit.
- OK_SS: microphones pass and the speaker retest meets the secondary 3 dB limit.
- Gain calibration applies one scalar gain per channel. It is not frequency-dependent EQ calibration and does not imply calibration of THD, noise, polarity, or delay.
- Matching channels at 1 kHz does not guarantee full-band compliance. Scalar gain may not correct differences in frequency response shape.

## 4. Fixture / Reference Equipment Self-Tests

Test subjects: six fixture reference microphones, not the DUT's four microphones and two speakers.

| ID | Test Item | Content | Criteria in the Supplied SoundCheck Recipe |
|---|---|---|---|
| STA-01 | 1 kHz level self-test | Check measured levels of reference microphones | mic1-4: 93 < Level < 96; mic5-6: 98 < Level < 100 |
| STA-02 | Self-noise self-test | Check reference microphone self-noise | All six: 28 < Noise < 39 |
| STA-03 | Polarity recording | Record reference microphone polarity | No separate acceptance threshold in Python |
| STA-04 | Frequency response recording | Save frequency response curves for six reference microphones | Recording / diagnostic use |
| STA-05 | Self-test validity check | Confirm a recent successful self-test | Normal runs require no more than 23 hours since the last successful self-test |

Level and Noise are SoundCheck self-test outputs. The source material describes Noise as SPLA based on its field/use; the exact Level unit should be checked against the sequence. Self-test limits use strict inequalities: equality at either boundary does not pass. Other engines have different saved self-test limits; this table must not be used to reclassify all historical results.

## 5. Pre-Test Checks

| ID | Check Item | Content |
|---|---|---|
| PRE-01 | DUT identification | Serial, HW ID, firmware build, station ID, SKU, and hardware revision |
| PRE-02 | Battery level | Must be > 50%; exactly 50% is not accepted |
| PRE-03 | Device connectivity / MCU status | ADB, port forwarding, MCU stop/start, ping, and status checks |
| PRE-04 | Test environment preparation | Remove old files, prepare playback and recording, disable EQ, set speaker volume to 100, and prevent sleep; restore EQ/SystemUI and release the wake lock after completion |

A pre-test failure should not automatically be classified as a DUT acoustic performance defect.

## 6. Other OK2Cal Tests: Barney Execution Not Confirmed

These categories describe test methods present in the repository, not tests confirmed as executed or passed on Barney.

| Category | Method Count | Main Test Content |
|---|---:|---|
| Audio | 2 | Playback, capture, and format checks; not SoundCheck gain calibration |
| Camera | 6 | Data generation, metadata, rolling shutter, frame drops, refresh rate, and VGA format |
| General | 2 | Direct-channel support and data collection tools |
| Accelerometer | 9 | Raw data; main/sub IMU resolution, measurement range, refresh rate, and drops |
| Gyroscope | 9 | Raw data; main/sub IMU resolution, measurement range, refresh rate, and drops |
| Magnetometer | 4 | Raw data, measurement range, refresh rate, and drops |
| IMU temperature | 3 | Raw data and main/sub refresh rates |
| Stability | 1 | Service stop/start and ADB connection stability |
| Timestamp | 2 | Sensor timestamps and corresponding camera-frame delays |
| **Total** | **38** | Method existence does not prove execution; some methods may be skipped conditionally |

Complete Barney execution results for these 38 methods were not available. There is also insufficient evidence to list other camera, IMU, or display workflows as completed Barney calibrations.

## 7. Additional Explicit Limits and Settings

This section includes additional numeric values explicitly listed in the source inventory. Acceptance thresholds, compensation calculation settings, and data format/acquisition settings are different types of requirements and must be distinguished.

### 7.1 Gain Compensation Calculation Settings

| Parameter | Value | Purpose / Limitations |
|---|---|---|
| abs_max_mic_gain | 2.0 dB | Maximum compensation magnitude per microphone channel |
| abs_max_spk_gain | 3.0 dB | Maximum compensation magnitude per speaker channel |
| max_mic_diff | 2.5 dB | Residual difference target for microphone gain-reference compensation calculations; does not replace full-band cal-pass checks |
| max_spk_diff | 3.0 dB | Residual difference target for speaker gain-reference compensation calculations; does not replace full-band cal-pass checks or the primary 2 dB first-test limit |

### 7.2 Camera / General / Timestamp OK2Cal Limits

The following values come from repository OK2Cal methods/configuration. Barney execution results were not available; these must not be represented as verified Barney specifications.

| Category | Test Item | Explicit Limit / Setting | Notes |
|---|---|---|---|
| Camera | Rolling shutter skew | Maximum 33 ms | The source material does not explicitly specify whether equality at the limit is accepted |
| Camera | Frame-drop rate | Maximum 4% | Expected frames use a 20-second, 10 Hz configuration; equality behavior requires confirmation |
| Camera | Refresh rate | 10 Hz with 3% tolerance | Test setting; may be overridden by testbed/method configuration |
| Camera | VGA format | 640 x 480; PPM max value 255 | Format validation, not a calibration parameter |
| General | Direct channel / collection tools | No numeric limit provided in the source material | Functionality / tool availability checks |
| Timestamp | Sensor timestamp validation | Maximum validation difference: 1 second | Equality behavior requires confirmation |
| Timestamp | Corresponding camera-frame delay | No numeric limit provided in the source material | May be skipped depending on data conditions |

### 7.3 IMU / Sensor OK2Cal Limits and Settings

These are overridable configuration/method settings, not a confirmed Barney sensor inventory or post-calibration parameters.

| Sensor | Resolution / Format | Measurement Range Setting | Sensitivity / Scale Setting | Refresh Rate Setting | Expected Files |
|---|---|---|---|---|---:|
| Main/sub accelerometer | 16-bit | +/-8 g | At least 2,048 LSB/g | 200 Hz | 2 |
| Main/sub gyroscope | 16-bit | 1,000 dps; signed range not explicitly described in the source material | At least 16 LSB/dps | 200 Hz | 2 |
| Magnetometer | 16-bit | 900 uT; signed range not explicitly described in the source material | 0.15 uT/LSB | 100 Hz | 1 |
| IMU temperature | Not provided | Not provided | Not provided | 1 Hz | 1 |

| Test Item | Explicit Limit / Setting | Notes |
|---|---|---|
| Accelerometer / gyroscope / magnetometer drop rate | Factory build: 3%; Google build: 5% | Source code uses different thresholds by build type; equality behavior requires confirmation |
| Stability | Three rounds, with two service stop/start cycles per round, plus ADB connection checks | Execution count setting, not a numeric performance limit |
| OK2Cal audio capture | 16 kHz, mono, 16-bit | Recording format setting, not an acoustic acceptance threshold |

### 7.4 Self-Test Configuration Differences Between Engines

| Engine / Configuration Source | mic1-4 Level | mic5-6 Level | Notes |
|---|---|---|---|
| Supplied SoundCheck recipe | 93 < Level < 96 | 98 < Level < 100 | Strict inequalities are confirmed |
| Saved ACE recipes | [92, 96] | [90, 98] | Configuration values listed in the source; brackets do not establish verified ACE boundary acceptance behavior |

SoundCheck self-test limits must not be used directly to reclassify historical ACE results. Disabled spec check/gain push in ACE recipes does not establish that station self-tests were also disabled.

### 7.5 Items Without Complete Supplied Limits

Complete acceptance criteria were not available for THD, Rub & Buzz, ePRB, DUT sensitivity performance limits, reference polarity, reference frequency response, or camera-frame delay. "Not provided" does not mean that no specification exists or that the item automatically passes; confirmation from the relevant specification/engine owner is required.
