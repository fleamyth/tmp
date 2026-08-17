#pragma once

#include <mmdeviceapi.h>
#include <mmreg.h>

enum _AUDIO_STREAM_CATEGORY;  //forward declare stream cat enum

namespace AudioTestFramework
{
#ifdef SURFACE_AUDIO_TEST_FRAMEWORK_EXPORTS
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllexport)
#else
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllimport)
#endif

    const int TOTAL_SELF_TEST_RESULTS = 256;

#define ENDPOINT_STATE_ACTIVE DEVICE_STATE_ACTIVE
#define ENDPOINT_STATE_DISABLED DEVICE_STATE_DISABLED
#define ENDPOINT_STATE_NOTPRESENT DEVICE_STATE_NOTPRESENT
#define ENDPOINT_STATE_UNPLUGGED DEVICE_STATE_UNPLUGGED
#define ENDPOINT_STATEMASK_ALL DEVICE_STATEMASK_ALL

    typedef
        enum SURFACE_AUDIO_TEST_FRAMEWORK_API _EndpointCapture
    {
        CAPTURE_NONE = 0,
        DMIC_CH0 = 0x1,
        DMIC_CH1 = 0x2,
        DMIC_STEREO = (DMIC_CH0 | DMIC_CH1),    // internal microphone, No JD
        HEADSET_MIC = 0x10,                     // external microphone, Has JD
        CAPTURECONSOLE = 0x10000,
        CAPTUREMULTIMEDIA = 0x20000,
        CAPTURECOMMUNICATION = 0x40000,
        CAPTUREDEFAULTMASK =    CAPTURECONSOLE | 
                                CAPTUREMULTIMEDIA | 
                                CAPTURECOMMUNICATION,
        CAPTURE_USB_EXTERNAL_L = 0x100000,
        CAPTURE_USB_EXTERNAL_R = 0x200000,
        CAPTURE_USB_EXTERNAL_STEREO =   CAPTURE_USB_EXTERNAL_L | 
                                        CAPTURE_USB_EXTERNAL_R,
        MIC_ARRAY_CH0 = 0x1000000,
        MIC_ARRAY_CH1 = 0x2000000,
        MIC_ARRAY_CH2 = 0x4000000,
        MIC_ARRAY_CH3 = 0x8000000,
        MIC_ARRAY_CH4 = 0x10000000,
        MIC_ARRAY_CH5 = 0x20000000,
        MIC_ARRAY_CH6 = 0x40000000,
        MIC_ARRAY_CH7 = 0x80000000,
        MIC_ARRAY_ALL = MIC_ARRAY_CH0 |
                        MIC_ARRAY_CH1 |
                        MIC_ARRAY_CH2 |
                        MIC_ARRAY_CH3 |
                        MIC_ARRAY_CH4 |
                        MIC_ARRAY_CH5 |
                        MIC_ARRAY_CH6 |
                        MIC_ARRAY_CH7,
    } eEndpointCapture;

    typedef
        enum SURFACE_AUDIO_TEST_FRAMEWORK_API _EndpointRender
    {
        RENDER_NONE = 0,
        SPEAKER_LEFT = 0x1,
        SPEAKER_RIGHT = 0x2,
        SPEAKER_STEREO = (SPEAKER_LEFT | SPEAKER_RIGHT),
        HEADPHONE_LEFT = 0x10,
        HEADPHONE_RIGHT = 0x20,
        HEADPHONE_STEREO = (HEADPHONE_LEFT | HEADPHONE_RIGHT),
        RENDERCONSOLE = 0x10000,
        RENDERMULTIMEDIA = 0x20000,
        RENDERCOMMUNICATION = 0x40000,
        RENDERDEFAULTMASK = RENDERCONSOLE | RENDERMULTIMEDIA | RENDERCOMMUNICATION,
        RENDER_USB_EXTERNAL_L = 0x100000,
        RENDER_USB_EXTERNAL_R = 0x200000,
        RENDER_USB_EXTERNAL_STEREO = RENDER_USB_EXTERNAL_L | RENDER_USB_EXTERNAL_R,
    } eEndpointRender;

    typedef
        enum SURFACE_AUDIO_TEST_FRAMEWORK_API _SpeakerSelection
    {
        SPEAKER_NONE = 0,
        SPEAKER_TOP = 0x1,
        SPEAKER_BOTTOM = 0x2,
        SPEAKER_FULL = (SPEAKER_TOP | SPEAKER_BOTTOM)
    } eSpeakerSelection;

    typedef
        enum SURFACE_AUDIO_TEST_FRAMEWORK_API AmpSelection
    {
        AMP0 = 0,
        AMP_LF1 = AMP0,     //Only Subwoofer amp
        AMP1,
        AMP_HF1 = AMP1,     //Only Top Left amp
        AMP2,
        AMP_HF2 = AMP2,     //Only Bottom Left amp
        AMP3,
        AMP_HF3 = AMP3,     //Only Top Right amp
        LANDSCAPE_DEFAULT,  //Left signal to top left, right signal to top right, stereo to subs
        PORTRAIT_DEFAULT,   //Left signal to bottom left, right signal to top left, stereo to subs
        ALL_AMPS_STEREO     //Stereo to all amps
    } eAmpSelection;

    // Centaurus amp bitmask values
#define AMP_LA_NORTH 0x1
#define AMP_LA_SOUTH 0x2
#define AMP_NY_SOUTH 0x4
#define AMP_NY_NORTH 0x8

    typedef
        enum SURFACE_AUDIO_TEST_FRAMEWORK_API MicArraySelection
    {
        MIC_ARRAY_0 = 0,
        MIC_ARRAY_PORTRAIT = MIC_ARRAY_0,
        MIC_ARRAY_NY = MIC_ARRAY_0, // Centaurus Mic array selection
        MIC_ARRAY_1,
        MIC_ARRAY_LANDSCAPE = MIC_ARRAY_1,
        MIC_ARRAY_LA = MIC_ARRAY_1, // Centaurus Mic array selection
        DEFAULT_MIC_ARRAY = MIC_ARRAY_LANDSCAPE,
    } eMicArraySelection;

#define MAKE_STRING_CASE(VAR) case VAR : return (L#VAR)


    inline const wchar_t* MicArraySelectionToString(uint32_t selection)
    {
        switch (selection)
        {
            MAKE_STRING_CASE(MIC_ARRAY_PORTRAIT);
            MAKE_STRING_CASE(MIC_ARRAY_LANDSCAPE);
        default: return L"UNKNOWN_MIC_SELECTION";
        }
    }

    inline const wchar_t* AmpSelectionToString(uint32_t selection)
    {
        switch (selection)
        {
            MAKE_STRING_CASE(AMP_LF1);
            MAKE_STRING_CASE(AMP_HF1);
            MAKE_STRING_CASE(AMP_HF2);
            MAKE_STRING_CASE(AMP_HF3);
            MAKE_STRING_CASE(LANDSCAPE_DEFAULT);
            MAKE_STRING_CASE(PORTRAIT_DEFAULT);
            MAKE_STRING_CASE(ALL_AMPS_STEREO);
        default: return L"UNKNOWN_AMP_SELECTION";
        }
    }

    enum SelfTestStates
    {
        PENDING = 0,
        PASS,
        FAIL,
        BLOCKED,
        RESERVED,
        OBSOLETE,
    };

    inline const wchar_t* SelfTestResultToString(uint32_t selection)
    {
        switch (selection)
        {
            MAKE_STRING_CASE(PENDING);
            MAKE_STRING_CASE(PASS);
            MAKE_STRING_CASE(FAIL);
            MAKE_STRING_CASE(BLOCKED);
            MAKE_STRING_CASE(RESERVED);
            MAKE_STRING_CASE(OBSOLETE);
        default: return L"UNKNOWN_SELF_TEST_RESULT";
        }
    }

    enum SelfTestsSueCreek
    {
        LF1_I2C_COMM_FAILURE = 0,
        LF1_AUDIO_FAULT_PIN,
        LF1_CLK_INVALID,
        LF1_XSMUTE_V_ABOVE_0PT7,
        LF1_XSMUTE_V_ABOVE_0PT3,
        LF1_LEFT_CHANNEL_SHORT,
        LF1_RIGHT_CHANNEL_SHORT,
        LF1_LEFT_CHANNEL_IMPEDANCE_FLG,
        LF1_RIGHT_CHANNEL_IMPEDANCE_FLG,
        LF1_VDD_UNDER_2PT7V,
        LF1_CLK_ERR,
        LF1_CLK_MISSING,
        LF1_CLK_HALT_DETECTED,

        HF1_I2C_COMM_FAILURE = 16,
        HF1_AUDIO_FAULT_PIN,
        HF1_CLK_INVALID,
        HF1_XSMUTE_V_ABOVE_0PT7,
        HF1_XSMUTE_V_ABOVE_0PT3,
        HF1_LEFT_CHANNEL_SHORT,
        HF1_RIGHT_CHANNEL_SHORT,
        HF1_LEFT_CHANNEL_IMPEDANCE_FLG,
        HF1_RIGHT_CHANNEL_IMPEDANCE_FLG,
        HF1_VDD_UNDER_2PT7V,
        HF1_CLK_ERR,
        HF1_CLK_MISSING,
        HF1_CLK_HALT_DETECTED,

        HF2_I2C_COMM_FAILURE = 32,
        HF2_AUDIO_FAULT_PIN,
        HF2_CLK_INVALID,
        HF2_XSMUTE_V_ABOVE_0PT7,
        HF2_XSMUTE_V_ABOVE_0PT3,
        HF2_LEFT_CHANNEL_SHORT,
        HF2_RIGHT_CHANNEL_SHORT,
        HF2_LEFT_CHANNEL_IMPEDANCE_FLG,
        HF2_RIGHT_CHANNEL_IMPEDANCE_FLG,
        HF2_VDD_UNDER_2PT7V,
        HF2_CLK_ERR,
        HF2_CLK_MISSING,
        HF2_CLK_HALT_DETECTED,

        HF3_I2C_COMM_FAILURE = 48,
        HF3_AUDIO_FAULT_PIN,
        HF3_CLK_INVALID,
        HF3_XSMUTE_V_ABOVE_0PT7,
        HF3_XSMUTE_V_ABOVE_0PT3,
        HF3_LEFT_CHANNEL_SHORT,
        HF3_RIGHT_CHANNEL_SHORT,
        HF3_LEFT_CHANNEL_IMPEDANCE_FLG,
        HF3_RIGHT_CHANNEL_IMPEDANCE_FLG,
        HF3_VDD_UNDER_2PT7V,
        HF3_CLK_ERR,
        HF3_CLK_MISSING,
        HF3_CLK_HALT_DETECTED,

        SPI_SMC_COMMS = 64,

        CLKGEN_I2C_COMMS = 72,
        HW_PLL_I2C_COMMS = 73,
        SELF_TEST_END
    };

    inline const wchar_t* SelfTestSueCreekCodeToString(uint32_t selection)
    {
        switch (selection)
        {
            MAKE_STRING_CASE(LF1_I2C_COMM_FAILURE);
            MAKE_STRING_CASE(LF1_AUDIO_FAULT_PIN);
            MAKE_STRING_CASE(LF1_CLK_INVALID);
            MAKE_STRING_CASE(LF1_XSMUTE_V_ABOVE_0PT7);
            MAKE_STRING_CASE(LF1_XSMUTE_V_ABOVE_0PT3);
            MAKE_STRING_CASE(LF1_LEFT_CHANNEL_SHORT);
            MAKE_STRING_CASE(LF1_RIGHT_CHANNEL_SHORT);
            MAKE_STRING_CASE(LF1_LEFT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(LF1_RIGHT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(LF1_VDD_UNDER_2PT7V);
            MAKE_STRING_CASE(LF1_CLK_ERR);
            MAKE_STRING_CASE(LF1_CLK_MISSING);
            MAKE_STRING_CASE(LF1_CLK_HALT_DETECTED);

            MAKE_STRING_CASE(HF1_I2C_COMM_FAILURE);
            MAKE_STRING_CASE(HF1_AUDIO_FAULT_PIN);
            MAKE_STRING_CASE(HF1_CLK_INVALID);
            MAKE_STRING_CASE(HF1_XSMUTE_V_ABOVE_0PT7);
            MAKE_STRING_CASE(HF1_XSMUTE_V_ABOVE_0PT3);
            MAKE_STRING_CASE(HF1_LEFT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF1_RIGHT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF1_LEFT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF1_RIGHT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF1_VDD_UNDER_2PT7V);
            MAKE_STRING_CASE(HF1_CLK_ERR);
            MAKE_STRING_CASE(HF1_CLK_MISSING);
            MAKE_STRING_CASE(HF1_CLK_HALT_DETECTED);

            MAKE_STRING_CASE(HF2_I2C_COMM_FAILURE);
            MAKE_STRING_CASE(HF2_AUDIO_FAULT_PIN);
            MAKE_STRING_CASE(HF2_CLK_INVALID);
            MAKE_STRING_CASE(HF2_XSMUTE_V_ABOVE_0PT7);
            MAKE_STRING_CASE(HF2_XSMUTE_V_ABOVE_0PT3);
            MAKE_STRING_CASE(HF2_LEFT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF2_RIGHT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF2_LEFT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF2_RIGHT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF2_VDD_UNDER_2PT7V);
            MAKE_STRING_CASE(HF2_CLK_ERR);
            MAKE_STRING_CASE(HF2_CLK_MISSING);
            MAKE_STRING_CASE(HF2_CLK_HALT_DETECTED);

            MAKE_STRING_CASE(HF3_I2C_COMM_FAILURE);
            MAKE_STRING_CASE(HF3_AUDIO_FAULT_PIN);
            MAKE_STRING_CASE(HF3_CLK_INVALID);
            MAKE_STRING_CASE(HF3_XSMUTE_V_ABOVE_0PT7);
            MAKE_STRING_CASE(HF3_XSMUTE_V_ABOVE_0PT3);
            MAKE_STRING_CASE(HF3_LEFT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF3_RIGHT_CHANNEL_SHORT);
            MAKE_STRING_CASE(HF3_LEFT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF3_RIGHT_CHANNEL_IMPEDANCE_FLG);
            MAKE_STRING_CASE(HF3_VDD_UNDER_2PT7V);
            MAKE_STRING_CASE(HF3_CLK_ERR);
            MAKE_STRING_CASE(HF3_CLK_MISSING);
            MAKE_STRING_CASE(HF3_CLK_HALT_DETECTED);

            MAKE_STRING_CASE(SPI_SMC_COMMS);

            MAKE_STRING_CASE(CLKGEN_I2C_COMMS);
            MAKE_STRING_CASE(HW_PLL_I2C_COMMS);
        default: return L"UNKNOWN_SELF_TEST";
        }
    }

    constexpr int LOOPBACK_MAX_CHANNELS = 32;
    typedef struct SURFACE_AUDIO_TEST_FRAMEWORK_API _LoopBackCmdOptions
    {
        BOOL fLowLatency;
        BOOL fExclusiveMode;
        LPCTSTR CaptureDevice;
        LPCTSTR RenderDevice;
        LPCTSTR SpeakerSelection;
        eEndpointCapture CapturePath;
        eEndpointRender RenderPath;
        eSpeakerSelection SpeakerPath;
        BOOL isDefaultCaptureDevice;
        ERole eCaptureRole;
        BOOL isDefaultRenderDevice;
        ERole eRenderRole;
        FLOAT CaptureVolume;   // 0~100
        FLOAT RenderVolume;    // 0~100
        BOOL CaptureSwap;
        UCHAR RenderChannelMute;
        INT32 ChannelSelectArray[LOOPBACK_MAX_CHANNELS]; //assumed right now that 32 channel capture/render endpoint is the largest possible
        INT32 ChannelSelectCount;
        _AUDIO_STREAM_CATEGORY RenStreamCategory;
        _AUDIO_STREAM_CATEGORY CapStreamCategory;
        BOOL RenderRaw;
        BOOL CaptureRaw;
    } LoopBackCmdOptions;
    

    typedef enum SURFACE_AUDIO_TEST_FRAMEWORK_API AudioChannel
    {
        AudioChannelLeft = 0,
        AudioChannelRight = 1,
        AudioChannelStereo = 2,
    } AudioChannel;

    typedef enum SURFACE_AUDIO_TEST_FRAMEWORK_API ToneType
    {
        ToneTypeSawtooth = 0,
        ToneTypeSine = 1,
        ToneTypeSquare = 2,
        ToneTypeTriangle = 3,
        ToneTypeChirp = 4
    } ToneType;

    typedef struct SURFACE_AUDIO_TEST_FRAMEWORK_API BufferData
    {
        ULONG length;
        BYTE* bytes;
    } BufferData;

    typedef struct SURFACE_AUDIO_TEST_FRAMEWORK_API Wave
    {
        BufferData Data;
        int NumberOfChannels;
        int NumberOfSamples;
        struct AnalogReferenceInformation* ReferenceInformation; // optional
        int Resolution;
        int SampleRateInHertz;
    } Wave;

    class SURFACE_AUDIO_TEST_FRAMEWORK_API Parser
    {
    public:
        static void MyParseCommandLine(int argc, TCHAR* argv[], LoopBackCmdOptions& LoopBackCmdOptions);
    };

    HRESULT SURFACE_AUDIO_TEST_FRAMEWORK_API SaveRawWaveData(BYTE *CaptureBuffer, size_t BufferSize, const WCHAR* Filename);
    HRESULT SURFACE_AUDIO_TEST_FRAMEWORK_API SaveWaveData(BYTE *CaptureBuffer, size_t BufferSize, WAVEFORMATEX* pWaveFormat, const WCHAR* Filename);
    HRESULT SURFACE_AUDIO_TEST_FRAMEWORK_API SaveWaveData(BYTE *CaptureBuffer, size_t BufferSize, WAVEFORMATEXTENSIBLE* pWaveFormat, const WCHAR* Filename);
    HRESULT SURFACE_AUDIO_TEST_FRAMEWORK_API SaveWaveDataAppend(BYTE* CaptureBuffer, size_t BufferSize, WAVEFORMATEX* pWaveFormat, const WCHAR* Filename);
    HRESULT SURFACE_AUDIO_TEST_FRAMEWORK_API SaveWaveDataAppend(BYTE* CaptureBuffer, size_t BufferSize, WAVEFORMATEXTENSIBLE* pWaveFormat, const WCHAR* Filename);

    enum class QCSmartAmpCalibrationStatus
    {
        Uncalibrated,
        Failed,
        Calibrated
    };
    
    struct QCSmartAmpCalibrationParameters
    {
        float r0;
        float t0;
    };

}