#pragma once

#include <mmdeviceapi.h>
#include <WebServices.h>
#include "FaultAggregator.h"
#include "AudioTestFrameworkTypes.h"
#include "TraceLogging.hpp"
#include <iostream>
#include <fstream>
using namespace std;

namespace AudioTestFramework
{
   

#ifdef SURFACE_AUDIO_TEST_FRAMEWORK_EXPORTS
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllexport)
#else
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllimport)
#endif

#define MIN_VOLUME_SCALE    (  0)  // 0~100 mapping to 0.0~1.0 scalar
#define MAX_VOLUME_SCALE    (100)  // 0~100 mapping to 0.0~1.0 scalar
#define MUTE_LEFT_CHANNEL   (0x1)
#define MUTE_RIGHT_CHANNEL  (0x2)

// definitions for audio service keys
#define UNASSIGNED_KEY -99
#define DEFAULT_KEY 0

    const WCHAR* GetCapturePath(eEndpointCapture capture);
    const WCHAR* GetRenderPath(eEndpointRender render);

    
    class SURFACE_AUDIO_TEST_FRAMEWORK_API CLogFile
    {
    public:
        static bool CreateLogger(bool jsonOutputMode);
        static TraceLogging::Logger* GetLogger();

        static void SetConsoleMode(bool printToConsole);

        static void LogTextAndHresult(TraceLogging::Level level, HRESULT hr, LPTSTR lpBuf);

    private:
        CLogFile() {}
        ~CLogFile() {}

        static TraceLogging::Logger* m_logger;
    };

#define WIDE2(x) L##x
#define WIDECHAR(x) WIDE2(x)
#define WIDE_FUNCTION WIDECHAR(__FUNCTION__)
#define LOG_AND_EXIT(s,d) {CLogFile::GetLogger()->LogMessage(TraceLogging::Level::Info, L"%ws: %ws: hr = 0x%X", WIDE_FUNCTION, s, d); goto Exit;}
#define SET_AND_EXIT(x) {hr = x; goto Exit;}
    
    typedef enum LoopbackPathDirection
    {
        LoopbackPathDirectionMicrophoneTest = 0,
        LoopbackPathDirectionSpeakerTest = 1,
    } LoopbackPathDirection;


    template <class T> void SafeRelease(T **ppT)
    {
        if (*ppT)
        {
            (*ppT)->Release();
            *ppT = NULL;
        }
    }

    template <typename T>

    // Refer to atlbase.h
    // c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\atlmfc\include\atlbase.h
    class AutoReleaseDevice
    {
        T* m_Device;

    public:
        operator T*() { return m_Device; }
        operator T*() const { return m_Device; }
        T** operator&() { return &m_Device; }
        const T** operator&() const { return &m_Device; }
        T* operator->() { return m_Device; }
        T* operator->() const { return m_Device; }

        AutoReleaseDevice<T>& operator=(AutoReleaseDevice<T>& p)
        {
            if (*this != p)
            {
                Free();
                Attach(p.Detach());
            }

            return *this;
        }

        void Attach(T* p) { m_Device = p; }

        T* Detach()
        {
            T* p;
            p = m_Device;
            m_Device = NULL;
            return p;
        }

        void Free()
        {
            if (m_Device != NULL)
            {
                m_Device->Release();
                m_Device = NULL;
            }
        }

        AutoReleaseDevice() : m_Device(NULL) {}
        ~AutoReleaseDevice() { Free(); }
    };

#define CHECK_GET_SERVICE_AND_REPORT(x) \
{\
    if (FAILED(x))\
    {\
        hr = x;\
        CLogFile::LogTextAndHresult(TraceLogging::Level::Error, hr, L"GetService");\
        return hr;\
    }\
}

#define CHECK_GET_MAP_AND_REPORT(x) \
{\
    if (FAILED(x))\
    {\
        hr = x;\
        CLogFile::LogTextAndHresult(TraceLogging::Level::Error, hr, L"GetMap");\
        return hr;\
    }\
}

#define CHECK_ADD_SERVICE_AND_REPORT(x) \
{\
    if (FAILED(x))\
    {\
        hr = x;\
        CLogFile::LogTextAndHresult(TraceLogging::Level::Error, hr, L"AddService");\
        return hr;\
    }\
}

#define CHECK_NULL_AND_REPORT(x) \
{\
    if (NULL == (x))\
    {\
        hr = ATF_ERR_NULL_PTR;\
        CLogFile::GetLogger()->LogMessage(TraceLogging::Level::Error, L"%ws: Null pointer: %ws\n",WIDE_FUNCTION,L#x);\
        return hr;\
    }\
}

#define CHECK_NULL_ENDPOINT_AND_REPORT(x) \
{\
    if (NULL == (x))\
    {\
        hr = ATF_ERR_DEVICE_NOT_BOUND;\
        CLogFile::GetLogger()->LogMessage(TraceLogging::Level::Error, L"%ws: %ws not set yet. Have you called BindToDevice yet?",WIDE_FUNCTION,L#x);\
        return hr;\
    }\
}

#define CHECK_DEVICE_NOT_FOUND(x) \
{\
    if (NULL == (x) && m_pwstrDeviceFriendlyName == NULL)\
    {\
        hr = ATF_ERR_DEVICE_NOT_BOUND;\
        CLogFile::GetLogger()->LogMessage(TraceLogging::Level::Error, L"%ws: %ws not set yet. Have you called BindToDevice yet?",WIDE_FUNCTION,L#x);\
        return hr;\
    }\
    else if (NULL == (x))\
    {\
        hr = ATF_ERR_DEVICE_NOT_FOUND;\
        CLogFile::GetLogger()->LogMessage(TraceLogging::Level::Error, L"%ws: %ws not found: %ws",WIDE_FUNCTION,L#x, m_pwstrDeviceFriendlyName);\
        return hr;\
    }\
}

    HRESULT WriteWaveFile(HANDLE FileHandle, const BYTE *Buffer, const size_t BufferSize, WAVEFORMATEX* pWaveFormat);
    HRESULT WriteWaveFile(HANDLE FileHandle, const BYTE *Buffer, const size_t BufferSize, WAVEFORMATEXTENSIBLE* pWaveFormat);
    HRESULT AppendWaveFile(HANDLE FileHandle, const BYTE* Buffer, const size_t BufferSize, WAVEFORMATEX* pWaveFormat);
    HRESULT AppendWaveFile(HANDLE FileHandle, const BYTE* Buffer, const size_t BufferSize, WAVEFORMATEXTENSIBLE* pWaveFormat);
    const WCHAR* GetEndpointStateStr(DWORD HeadphoneState);
    const WCHAR* GetAudioEffectName(GUID g);
    const GUID GetAudioEffectGuid(const WCHAR*);
    std::wstring ToString(const std::string& str);
}

