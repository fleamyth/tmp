#pragma once


#define FACTILIY_ATF                1
#define FACILITY_MTEMODE            2
#define FACILITY_DIRECTDRIVERQUERY  3
#define FACILITY_LOOPBACK           4
#define FACILITY_DUMP               5
#define FACILITY_UNIT_TEST          6
#define FACILITY_HID_AUDIO          7
#define FACILITY_RTK_CALIBRATION    8
#define FACILITY_QCD                9
#define FACILITY_CIRRUS_AMP         0xA

#define SUCCESSFUL_NULL_SERVICE     0x1
#define SPECIFIED_KEY_EXISTS        0x2
#define SUCCESSFUL_ADD_TO_MAP       0x3
#define SUCCESSFUL_REMOVE_FROM_MAP  0x4

#define APIERR_SUCCESS                              0x0
#define ATF_ERR_GENERAL                             (0x87100001)
#define ATF_ERR_PARAM_INVALID                       (0x87100002)
#define ATF_ERR_NULL_PTR                            (0x87100003)
#define ATF_ERR_THREAD_CREATE                       (0x87100004)
#define ATF_ERR_MEMORY_ALLOC                        (0x87100005)
#define ATF_ERR_DEVICE_NOT_FOUND                    (0x87100006)
#define ATF_ERR_DEVICE_NOT_BOUND                    (0x87100007)
#define ATF_ERR_LOOPBACK_START_TIMEOUT              (0x87100008)
#define ATF_ERR_LOOPBACK_FAILURE                    (0x87100009)
#define ATF_ERR_LOOPBACK_BAD_FORMAT                 (0x8710000A)
#define ATF_ERR_FILE_HANDLE_INVALID                 (0x8710000B)
#define ATF_ERR_CAPTURE_THREAD_TIMEOUT_NO_SAMPLES   (0x8710000C)
#define ATF_ERR_CAPTURE_THREAD_TIMEOUT_LOW_SAMPLES  (0x8710000D)
#define ATF_ERR_NO_SERVICE_AVAILABLE                (0x8710000E)
#define ATF_ERR_SERVICE_UNSPECIFIED                 (0x8710000F)
#define ATF_ERR_SERVICE_KEY_NON_EXISTENT            (0x87100010)
#define ATF_ERR_CAPTURE_SERVICE_NOT_CREATED         (0x87100011)
#define ATF_ERR_ADD_TO_MAP_FAILURE                  (0x87100012)
#define ATF_ERR_RENDER_SERVICE_NOT_CREATED          (0x87100013)
#define ATF_ERR_REMOVE_FROM_MAP_FAILURE             (0x87100014)
#define ATF_ERR_GET_MAP_CAPTURE_FAILURE             (0x87100015)
#define ATF_ERR_GET_MAP_RENDER_FAILURE              (0x87100016)
#define ATF_ERR_SERVICE_NOT_CREATED_COMMAND         (0x87100017)
#define ATF_ERR_INVALID_STREAM_CAT                  (0x87100018)

#define UNIT_TEST_SETUP_FAILURE         (0x87600001)

#define ATF_HID_AUDIO_DEVICE_NOT_FOUND  (0x87700001)
#define ATF_HID_AUDIO_TOO_MANY_FOUND    (0x87700002)
#define ATF_HID_AUDIO_TIMED_OUT         (0x87700003)
#define ATF_HID_AUDIO_ILLEGAL_CMD       (0x87700004)

////
//// Create an HRESULT value from component pieces
////
//#define MAKE_ATF_HRESULT(fac,code) \
//    ((HRESULT)  ((unsigned long)(fac)<<16) | ((unsigned long)(code)))


wchar_t* CustomErrorToString(HRESULT hr);
