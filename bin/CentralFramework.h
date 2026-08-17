#pragma once
#include <windows.h>
#include <AudioSessionTypes.h>
#include "AudioTestFrameworkTypes.h"
#include "Utility.h"

#include <vector>
#include <string>
#include <map>

namespace AudioTestFramework
{

#ifdef SURFACE_AUDIO_TEST_FRAMEWORK_EXPORTS
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllexport)
#else
#define SURFACE_AUDIO_TEST_FRAMEWORK_API __declspec(dllimport)
#endif


    class SURFACE_AUDIO_TEST_FRAMEWORK_API CritSection : public CRITICAL_SECTION
    {
    public:
        CritSection()
        {
            InitializeCriticalSection(this);
        }

        ~CritSection()
        {
            DeleteCriticalSection(this);
        }

    private:
        // disable copy and assignment of CritSection
        CritSection(CritSection const&);
        CritSection& operator=(CritSection const&);
    };

    class AudioCaptureService;
    class AudioPlaybackService;
    class CWASAPILoopbackApp;
    class DriverDirectQueryClass;
    class CMTEMode;
    class SueCreekHidControl;
    class SmartAmpCalibration;
    class AudioServices;
    
    class SURFACE_AUDIO_TEST_FRAMEWORK_API AudioTestFrameworkClass
    {
    public:
        
        //! A static function to retrieve the singleton instance of the class.
        //! It will call the constructor if not already instantiated
        /*!
        \param jsonOutputMode a bool for if caller wants a json output (default is false)
        \\
        \return Singleton Instance pointer of AudioTestFrameworkClass
        \sa AudioTestFrameworkClass()
        */
        static AudioTestFrameworkClass* GetInstance(bool jsonOutputMode = false);

        //! A normal member function that will enumerate all endpoints of a given data flow
        /*!
        \param deviceType the data flow type of the device (render, capture, all)
        \param statusMask status bitmask for endpoints (ENDPOINT_STATE_ACTIVE, ENDPOINT_STATE_DISABLED, ENDPOINT_STATE_NOTPRESENT, ENDPOINT_STATE_UNPLUGGED)
        \\
        \return std::vector<std::wstring> a vector of endpoint names
        */
        std::vector<std::wstring> EnumerateEndpoints(EDataFlow deviceType, DWORD statusMask) const;

        //! A normal member function that take 8 args and creates a loopback thread.
        //! This is blocking until loopback is setup correctly and functioning.
        //! Once enabled completely, returns successfully. Will disable existing loopback
        //! if called while loopback already active.
        //! Currently will return error if loopback fails to start within 10s.
        /*!
        \param enable a BOOL switch to enable or disable the loopback
        \param CapturePath a friendly name substring value for capture endpoint
        \param RenderPath a friendly name substring value for render endpoint
        \param CaptureVolumePerc an FLOAT value between 0-100.0
        \param RenderVolumePerc an FLOAT value between 0-100.0
        \param exclusiveMode a BOOL value to set the loopback exclusivity mode
        \param returnDelayMs a UINT32 value to delay return feedback of the start operation
        \param channelSelect a INT32 array letting you sub select channels from a record/capture endpoint
        \param channelSelectCount an integer for selected channels (default is 0)
        \so that you can use render and capture endpoints with difference channel counts.
        \channelSelect[] must have 8 or more size
        \\
        \return HRESULT
        */
        HRESULT Loopback(
            BOOL enable,
            eEndpointCapture CapturePath,
            eEndpointRender RenderPath,
            FLOAT CaptureVolumePerc,   // 0~100
            FLOAT RenderVolumePerc,    // 0~100
            BOOL exclusiveMode,
            UINT32 returnDelayMs,
            INT32 channelSelect[],
            int channelSelectCount = 0);

        //! A normal member function that take 8 args and creates a loopback thread.
        //! This is blocking until loopback is setup correctly and functioning.
        //! Once enabled completely, returns successfully. Will disable existing loopback
        //! if called while loopback already active.
        //! Currently will return error if loopback fails to start within 10s.
        /*!
        \param enable a BOOL switch to enable or disable the loopback
        \param CapturePath a friendly name substring value for capture endpoint
        \param RenderPath a friendly name substring value for render endpoint
        \param CaptureVolumePerc an FLOAT value between 0-100.0
        \param RenderVolumePerc an FLOAT value between 0-100.0
        \param exclusiveMode a BOOL value to set the loopback exclusivity mode
        \param returnDelayMs a UINT32 value to delay return feedback of the start operation
        \param channelSelect a INT32 array letting you sub select channels from a record/capture endpoint
        \param channelSelectCount an integer for selected channels (default is 0)
        \param renderStreamCategory is the Windows stream category desired for the render endpoint (default is AudioCategory_Other)
        \param captureStreamCategory is the Windows stream category desired for the capture endpoint (default is AudioCategory_Other)
        \param renderRaw a BOOL value to set if the render should be in raw mode (default is TRUE)
        \param captureRaw a BOOL value to set if the capture should be in raw mode (default is TRUE)
        \so that you can use render and capture endpoints with difference channel counts.
        \channelSelect[] must have 8 or more size
        \\
        \return HRESULT
        */
        HRESULT Loopback(
            BOOL enable,
            LPCTSTR CapturePath,
            LPCTSTR RenderPath,
            FLOAT CaptureVolumePerc,   // 0~100
            FLOAT RenderVolumePerc,    // 0~100
            BOOL exclusiveMode,
            UINT32 returnDelayMs,
            INT32 channelSelect[],
            int channelSelectCount = 0,
            AUDIO_STREAM_CATEGORY renderStreamCategory = AudioCategory_Other,
            AUDIO_STREAM_CATEGORY captureStreamCategory = AudioCategory_Other,
            BOOL renderRaw = TRUE,
            BOOL captureRaw = TRUE
            );

        //! A normal member function taking 7 arguments to produce a continuous tone
        //! that will continue until calling StopTonePlayback().
        //! Non blocking once tone render starts.
        //! Must set Render Device first using BindToRenderDevice()
        /*!
        \param channels to specify Left, Right or Stereo playback
        \param toneType to specify Sine, Square, Saw (Sine only supported currently)
        \param frequencyCount to specify how many frequencies to play (must be > 0)
        \param frequencies[] to specify frequencies to play (such as chord)
        \param amplitude to specific amplitude between 0.0 and 1.0
        \param sampleRate to specific sampleRate in Hz (eg. 48000)
        \param bitsPerSample to specific bitrate (16 bit usually)
        \param bitsPerSampleContainer short to specify the size of the bits per sample container (default is 0)
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param exclusive a bool value to set the exclusivity mode
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa StopTonePlayback(), BindToRenderDevice()
        */
        HRESULT StartTonePlayback(
            AudioChannel channels,
            ToneType toneType,
            int frequencyCount,
            double frequencies[],
            double amplitude,
            int sampleRate,
            short bitsPerSample,
            short bitsPerSampleContainer = 0,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            bool exclusive = false,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function taking 7 arguments to produce a logrithmic chirp tone
        //! from f_start to f_stop that will continue until calling StopTonePlayback().
        //! Non blocking once tone render starts.
        //! Must set Render Device first using BindToRenderDevice()
        /*!
        \param channels to specify Left, Right or Stereo playback
        \param frequencyStart to specify f_start in Hz
        \param frequencyStop to specify f_stop in Hz
        \param amplitude to specific amplitude between 0.0 and 1.0
        \param sampleRate to specific sampleRate in Hz (eg. 48000)
        \param bitsPerSample to specific bitrate (16 bit normally)
        \param chirpSilenceBeforeLengthS to specify silence interval in seconds before chirp
        \param chirpIntervalLengthS to specify time interval in seconds for chirp
        \param chirpSilenceAfterLengthS to specify silence interval in seconds after chirp
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param exclusiveMode a bool value to set the exclusivity mode
        \param bitsPerSampleContainer short to specify the size of the bits per sample container (default is 0)
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa StopTonePlayback(), BindToRenderDevice()
        */
        HRESULT StartChirpPlayback(
            AudioChannel channels,
            int frequencyStart,
            int frequencyStop,
            double amplitude,
            int sampleRate,
            short bitsPerSample,
            double chirpSilenceBeforeLengthS,
            double chirpIntervalLengthS,
            double chirpSilenceAfterLengthS,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            bool exclusiveMode = false,
            short bitsPerSampleContainer = 0,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function taking a path to a normal wav file
        //! Stuff Happens
        /*!
        \param szFileName WCHAR* path to wav file to play
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        */
        HRESULT StartFilePlayback(
            WCHAR* szFileName,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int renderServiceKey = DEFAULT_KEY);


        //! A normal member function that stops either tone or chirp playback threads.
        /*!
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        */
        HRESULT StopTonePlayback(const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function taking two pointer arguments and returning an HRESULT
        //! to retrieve the headphone and headset jack status directly from the codec.
        /*!
        \param headphone a BOOL pointer that will be filled with status
        \param headset a BOOL pointer that will be filled with status
        \\
        \return HRESULT
        */
        HRESULT GetJackPresence(
            BOOL* headphone, 
            BOOL* headset);

        //! A normal member function taking no arguments that prints and logs volumes retrieved directly from codec.
        //! Currently Logging only command.
        /*!
        \return HRESULT
        */
        HRESULT GetDriverVolumes();

        //! A normal member function taking no arguments that prints and logs mte mode setting retrieved directly from codec.
        //! Currently Logging only command.
        /*!
        \return HRESULT
        */
        HRESULT CheckMteModeSettings();

        //! A normal member function taking no arguments that prints and logs external Realtek DSP settings retrieved directly from codec driver.
        //! Currently Logging only command.
        /*!
        \return HRESULT
        */
        HRESULT CheckDspSettings();

        //! A normal member function taking two args to capture audio to a Wave& object.
        //! Blocking call until recording is finished.
        //! Must set Capture Device first using BindToCaptureDevice().
        /*!
        \param sampleRate an integer argument to specify samplerate in Hz (eg 48000)
        \param numberOfSamples an integer arg to specify number of samples to return in retWave (numberOfSamples/sampleRate = recording time)
        \param numberOfSamplesToTrim an integer arg to specify number of samples to throw away at beginning of stream capture
        \param numberOfBits to specific bits per channel (usually 16)
        \param numberOfChannels a short that specifies number of channels
        \param retWave to return the Wave object data.  MUST preallocate retWave.Data.bytes to size of (2 channels * (numberOfBits/numberOfBitsPerByte) * numberOfSamples)
        \param pFormat a WAVEFORMATEX pointer to be provided to load the wave format information
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT PerformAudioCapture(
            const int sampleRate,
            const int numberOfSamples,
            const int numberOfSamplesToTrim,
            const short numberOfBits,
            const short numberOfChannels,
            Wave& retWave,
            WAVEFORMATEX* pFormat,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function taking two args to capture loopback audio to a Wave& object.
        //! Blocking call until recording is finished.
        //! Must set Loopback Render Device first using BindToCaptureLoopbackDevice().
        /*!
        \param sampleRate an integer argument to specify samplerate in Hz (eg 48000)
        \param numberOfSamples an integer arg to specify number of samples to return in retWave (numberOfSamples/sampleRate = recording time)
        \param numberOfSamplesToTrim an integer arg to specify number of samples to throw away at beginning of stream capture
        \param numberOfBits to specific bits per channel (usually 16)
        \param numberOfChannels a short that specifies number of channels
        \param retWave to return the Wave object data.  MUST preallocate retWave.Data.bytes to size of (2 channels * (numberOfBits/numberOfBitsPerByte) * numberOfSamples)
        \param pFormat a WAVEFORMATEX pointer to be provided to load the wave format information
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param captureLoopbackServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT PerformAudioCaptureLoopback(
            const int sampleRate,
            const int numberOfSamples,
            const int numberOfSamplesToTrim,
            const short numberOfBits,
            const short numberOfChannels,
            Wave& retWave,
            WAVEFORMATEX* pFormat,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int captureLoopbackServiceKey = DEFAULT_KEY);

        //! A normal member function capture audio to a filepath.  If the file already exists it will be appended to from the previous session.
        //! Async call that returns immediately upon successfully starting capture stream.
        //! Must set Capture Loopback Device first using BindToCaptureLoopbackDevice().
        /*!
        \param sampleRate an integer argument to specify samplerate in Hz (eg 48000)
        \param numberOfBits to specific bits per channel (usually 16)
        \param numberOfChannels a short that specifies number of channels
        \param filename wchar_t* of file path to capture recording to
        \param pFormat a WAVEFORMATEX pointer to be provided to load the wave format information
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param captureLoopbackServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureLoopbackDevice()
        */
        HRESULT PerformAudioCaptureLoopbackAsync(
            const int sampleRate,
            const short numberOfBits,
            const short numberOfChannels,
            const wchar_t* filename,
            WAVEFORMATEX* pFormat,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int captureLoopbackServiceKey = DEFAULT_KEY);

        //! A normal member function capture audio to a filepath.  If the file already exists it will be appended to from the previous session.
        //! Async call that returns immediately upon successfully starting capture stream.
        //! Must set Capture Device first using BindToCaptureDevice().
        /*!
        \param sampleRate an integer argument to specify samplerate in Hz (eg 48000)
        \param numberOfBits to specific bits per channel (usually 16)
        \param numberOfChannels a short that specifies number of channels
        \param filename wchar_t* of file path to capture recording to
        \param pFormat a WAVEFORMATEX pointer to be provided to load the wave format information
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT PerformAudioCaptureAsync(
            const int sampleRate,
            const short numberOfBits,
            const short numberOfChannels,
            const wchar_t* filename,
            WAVEFORMATEX* pFormat,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function capture audio to a Wave object.
        //! Async call that returns immediately upon successfully starting capture stream.
        //! Must set Capture Device first using BindToCaptureDevice().
        //! Must call free(retWave.Data.bytes); after Wave has been used/copied/returned in order to free the underlying allocated buffer
        /*!
        \param sampleRate an integer argument to specify samplerate in Hz (eg 48000)
        \param numberOfBits to specific bits per channel (usually 16)
        \param numberOfChannels a short that specifies number of channels
        \param retWave to return the Wave object data.  Must call free(retWave.Data.bytes); after Wave has been used/copied/returned in order to free the underlying allocated buffer
        \param pFormat a WAVEFORMATEX pointer to be provided to load the wave format information
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param streamCategory is the Windows stream category desired for the endpoint (default is AudioCategory_Other)
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT PerformAudioCaptureAsync(
            const int sampleRate,
            const short numberOfBits,
            const short numberOfChannels,
            Wave& retWave,
            WAVEFORMATEX* pFormat,
            bool rawMode = false,
            AUDIO_STREAM_CATEGORY streamCategory = AudioCategory_Other,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function that stops capture thread and flushes remaining samples to file.
        //! Must set Capture Device first using PerformAudioCaptureAsync().
        /*!
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa PerformAudioCaptureAsync()
        */
        HRESULT StopCapture(const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function that stops capture thread and flushes remaining samples to file.
        //! Must set Capture Loopback Device first using PerformAudioCaptureLoopbackAsync().
        /*!
        \param captureLoopbackServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa PerformAudioCaptureLoopbackAsync()
        */
        HRESULT StopCaptureLoopback(const int captureLoopbackServiceKey = DEFAULT_KEY);

        //! Member function to enable/disable MTE Mode. Currently MTE Mode will disabled Dolby
        //! processing, disable both Mic and Speaker EQ, and will turn off mic noise suppression.
        //! It will also disable the hidden mic boost gain.
        /*!
        \param enable a BOOL arg to enable/disable MTE Mode
        \\
        \return HRESULT
        */
        HRESULT SetMteMode(
            BOOL enable);

        //! A normal member function taking one argument for friendly name of render device.
        //! Required to be called before any render device commands.
        /*!
        \param endpointFriendlyName a wchar* string such as "Speakers" or "Headphone"
        \param statusMask status bitmask for endpoints (ENDPOINT_STATE_ACTIVE, ENDPOINT_STATE_DISABLED, ENDPOINT_STATE_NOTPRESENT, ENDPOINT_STATE_UNPLUGGED)
        \param pRenderServiceKey pointer to the key associated with a AudioPlaybackService instance (default is nullptr meaning no service key variable passed)
        \\
        \return HRESULT and will return ATF_ERR_DEVICE_NOT_FOUND if device is not in the list under the given/default status mask
        */
        HRESULT BindToRenderDevice(
            const WCHAR* endpointFriendlyName,
            const DWORD statusMask = ENDPOINT_STATE_ACTIVE,
            int* const pRenderServiceKey = nullptr);

        //! A normal member function taking one argument for friendly name of capture device.
        //! Required to be called before any capture device commands.
        /*!
        \param endpointFriendlyName a wchar* string such as "Microphone" or "Headset"
        \param statusMask status bitmask for endpoints (ENDPOINT_STATE_ACTIVE, ENDPOINT_STATE_DISABLED, ENDPOINT_STATE_NOTPRESENT, ENDPOINT_STATE_UNPLUGGED)
        \param pCaptureServiceKey pointer to the key associated with a AudioCaptureService instance (default is nullptr meaning no service key variable passed)
        \\
        \return HRESULT and will return ATF_ERR_DEVICE_NOT_FOUND if device is not in the list under the given/default status mask
        */
        HRESULT BindToCaptureDevice(
            const WCHAR* endpointFriendlyName,
            const DWORD statusMask = ENDPOINT_STATE_ACTIVE,
            int* const pCaptureServiceKey = nullptr);

        //! A normal member function taking one argument for friendly name of capture loopback device.
        //! Required to be called before any capture device commands for hw or sw loopback purposes.
        /*!
        \param endpointFriendlyName a wchar* string such as "Speakers" or "Headphones"
        \param statusMask status bitmask for endpoints (ENDPOINT_STATE_ACTIVE, ENDPOINT_STATE_DISABLED, ENDPOINT_STATE_NOTPRESENT, ENDPOINT_STATE_UNPLUGGED)
        \param isSecondaryLoopbackRequested bool that specifies if secondary loopback is requested
        \param pCaptureLoopbackServiceKey pointer to the key associated with a AudioCaptureService instance (default is nullptr meaning no service key variable passed)
        \\
        \return HRESULT and will return ATF_ERR_DEVICE_NOT_FOUND if device is not in the list under the given/default status mask
        */
        HRESULT BindToCaptureLoopbackDevice(
            const WCHAR* endpointFriendlyName,
            const DWORD statusMask = ENDPOINT_STATE_ACTIVE,
            const bool isSecondaryLoopbackRequested = false,
            int* const pCaptureLoopbackServiceKey = nullptr);

        //! A normal member function to set Boost volume on the currently bound capture device and returning an HRESULT
        /*!
        \param vol double value param in dBFS
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT SetCaptureBoostVolume(
            double vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get Boost volume on the currently bound render capture and returning an HRESULT
        /*!
        \param vol double value param in dBFS
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT GetCaptureBoostVolume(
            double* vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set capture volume on the currently bound capture device and returning an HRESULT
        /*!
        \param vol double value param pointer in dBFS
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT SetCaptureVolume(
            double vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set capture volume scalar on the currently bound capture device and returning an HRESULT
        /*!
        \param vol double value param in range [0, 1]
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetCaptureVolumeScalar()
        */
        HRESULT SetCaptureVolumeScalar(
            double vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture volume on the currently bound capture device and returning an HRESULT
        /*!
        \param vol double value param in dBFS
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT GetCaptureVolume(
            double* vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture volume scalar on the currently bound capture device and returning an HRESULT
        /*!
        \param vol double value param in range [0, 1.0]
        \param captureServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT GetCaptureVolumeScalar(
            double* vol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set playback volume on the currently bound render device and returning an HRESULT
        /*!
        \param vol double value param pointer in dBFS
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT SetPlaybackVolume(
            double vol,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to set playback volume on the currently bound render device and returning an HRESULT
        /*!
        \param vol double value param in range [0, 1]
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetPlaybackVolumeScalar()
        */
        HRESULT SetPlaybackVolumeScalar(
            double vol,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get playback volume on the currently bound render device and returning an HRESULT
        /*!
        \param vol double value param in dBFS
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT GetPlaybackVolume(
            double* vol,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get playback volume scalar on the currently bound render device and returning an HRESULT
        /*!
        \param vol double value param in range [0, 1.0]
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT GetPlaybackVolumeScalar(
            double* vol,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get playback volume ranges on the currently bound render device and returning an HRESULT
        /*!
        \param maxvol double value out param in dBFS
        \param minvol double value out param in dBFS
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT GetPlaybackVolumeRange(
            double* maxvol,
            double* minvol,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get playback format on the currently bound render device and returning an HRESULT
        /*!
        \param pFormat WAVEFORMATEX* value out param (must be freed by caller using CoTaskMemFree)
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa GetCaptureFormat()
        */
        HRESULT GetRenderFormat(
            WAVEFORMATEX** pFormat, 
            bool rawMode = false,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture format on the currently bound capture device and returning an HRESULT
        /*!
        \param pFormat WAVEFORMATEX* value out param (must be freed by caller using CoTaskMemFree)
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa GetCaptureFormat()
        */
        HRESULT GetCaptureFormat(
            WAVEFORMATEX** pFormat,
            bool rawMode = false,
            int const captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture format on the currently bound capture loopback device and returning an HRESULT
        /*!
        \param pFormat WAVEFORMATEX* value out param (must be freed by caller using CoTaskMemFree)
        \param rawMode bool to specify if playback should be using raw mode (default is false)
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa GetCaptureFormat()
        */
        HRESULT GetCaptureFormatLoopback(
            WAVEFORMATEX** pFormat,
            bool rawMode = false,
            const int captureLoopbackServiceKey = DEFAULT_KEY);

        //! A normal member function to set native capture format on the currently bound capture device and returning an HRESULT
        /*!
        \param sampleRateHz int for sample rate in Hz
        \param bitsPerSampleValid int for bits per sample
        \param bitsPerSampleContainer int for bits per sample container
        \param channelCount int for channel count to use
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetNativeRenderFormat()
        */
        HRESULT SetNativeCaptureFormat(
            __in int sampleRateHz,
            __in int bitsPerSampleValid,
            __in int bitsPerSampleContainer,
            __in int channelCount,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set playback format on the currently bound render device and returning an HRESULT
        /*!
        \param sampleRateHz int for sample rate in Hz
        \param bitsPerSampleValid int for bits per sample
        \param bitsPerSampleContainer int for bits per sample container
        \param channelCount int for channel count to use
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetNativeCaptureFormat()
        */
        HRESULT SetNativeRenderFormat(
            __in int sampleRateHz,
            __in int bitsPerSampleValid,
            __in int bitsPerSampleContainer,
            __in int channelCount,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture mute status on the currently bound capture device and returning an HRESULT
        /*!
        \param GetMuteEnableResult BOOL value param in dBFS
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT GetCaptureMuteEnable(
            BOOL* GetMuteEnableResult,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get playback mute status on the currently bound render device and returning an HRESULT
        /*!
        \param GetMuteEnableResult BOOL value param in dBFS
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT GetPlaybackMuteEnable(
            BOOL* GetMuteEnableResult,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get capture volume ranges on the currently bound capture device and returning an HRESULT
        /*!
        \param maxvol double value out param in dBFS
        \param minvol double value out param in dBFS
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT GetCaptureVolumeRange(
            double* maxvol,
            double* minvol,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set mute on the currently bound capture device and returning an HRESULT
        /*!
        \param enable boolean value to enable/disable mute
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetCaptureMuteEnable(bool)
        */
        HRESULT SetCaptureMuteEnable(
            bool enable,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to set mute on the currently bound render device and returning an HRESULT
        /*!
        \param enable boolean value to enable/disable mute
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa SetRenderMuteEndable(bool)
        */
        HRESULT SetPlaybackMuteEnable(
            bool enable,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to get connection status on the currently bound capture device and returning an HRESULT
        /*!
        \param connected boolean pointer value to return connection status
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToCaptureDevice()
        */
        HRESULT IsCaptureDeviceConnectedToJack(
            BOOL* connected,
            const int captureServiceKey = DEFAULT_KEY);

        //! A normal member function to get connection status on the currently bound render device and returning an HRESULT
        /*!
        \param connected boolean pointer value to return connection status
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        \sa BindToRenderDevice()
        */
        HRESULT IsPlaybackDeviceConnectedToJack(
            BOOL* connected,
            const int renderServiceKey = DEFAULT_KEY);

        //! A normal member function to retrieve static error string from HRESULT value
        /*!
        \param hr an HRESULT value to search for
        \\
        \return WCHAR* version of the error code
        */
        WCHAR* GetFaultString(
            HRESULT hr);

        //! A normal member function to insert a logging entry into the default logger for the purposes of adding messages
        //! like "Test X Begin" or "Test X End"
        /*!
        \param MessageToInsert is a Null terminated string to
        \\
        \return void
        */
        void
            LogInsert(
                WCHAR* MessageToInsert
            );

        //! A normal member function to configure/isolate specific amplifier channels for test purposes on SueCreek platforms
        //! requires InitHidAudioControlEndpoint() to be called first during session
        /*!
        \param AmpSelection is an enum available in types to represent different test configs expected
        \param channelGain is -128 to 127 signed BYTE dB gain value that will be applied to all channels (default is 0)
        \\
        \return HRESULT
        */
        HRESULT ConfigureStereoAmpSpeakerChannelOutput(
            AmpSelection ampSelection,
            BYTE channelGain = 0
        );

        //! A normal member function to configure/isolate specific amplifier channel mutes for test purposes using Realtek Smartamps
        /*!
        \param ampEnableBitmask is a bitmask when value of 0 is muted and 1 is unmuted/enabled.  Bit order is amp order amp0 is 0th lsb.
        \param ampChannelCount is the amount of channels being configured for this product config (eg. 2, 4, 6).
        \\
        \return HRESULT
        */
        HRESULT SetRealtekAmpEnableConfig(
            uint32_t ampEnableBitmask,
            uint32_t ampChannelCount
        );

        //! A normal member function to configure/isolate specific amplifier mutes for test purposes using Cirrus Smartamps
        /*!
        \param ampEnableBitmask is a bitmask when value of 0 is muted and 1 is unmuted/enabled.  Bit order is amp order amp0 is 0th lsb.
        \\
        \return HRESULT
        */
        HRESULT SetCirrusAmpEnableConfig(
            uint32_t ampEnableBitmask
        );

        //! A normal member function to retrieve the state of the cirrus amp driver
        /*!
        \param pAmpDriverState is a dword pointer that represents the state of the cirrus amp driver
        \\
        \return HRESULT
        */
        HRESULT GetCirrusAmpDriverStatus(
            uint32_t* pAmpDriverState
        );

        //! A normal member function to configure specific amplifier channel assignments for test purposes using Cirrus Smartamps
        /*!
        \param ampEnableBitmask is a bitmask when value of 0 is Left and 1 is Right.  Bit order is amp order amp0 is 0th lsb.
        \\
        \return HRESULT
        */
        HRESULT SetCirrusAmpChannelConfig(
            uint32_t ampEnableBitmask
        );

        //! A normal member function to configure/isolate specific mic arrays for test purposes
        //! requires InitHidAudioControlEndpoint() to be called first during session for use with SueCreek (Onward)
        //! requires Realtek driver instance for use with products like Centaurus
        /*!
        \param MicArraySelection is an enum available in types to represent different test configs expected
        \param channelGain is -128 to 127 signed BYTE dB gain value that will be applied to all channels (ignored on Cent) (default is 0)
        \\
        \return HRESULT
        */
        HRESULT ConfigureMicArray(
            MicArraySelection micSelection,
            BYTE channelGain = 0
        );

        //! A normal member function to initialize communications with a HID controlled audio device
        //! should be called once per session if HID configuration of Audio is necessary
        /*!
        \param none
        \\
        \return HRESULT
        */
        HRESULT InitHidAudioControlEndpoint();

        //! A normal member function to request FwVersion information from HID controlled audio device
        //! requires InitHidAudioControlEndpoint() to be called first during session
        /*!
        \param out params for header, version, property
        \\
        \return HRESULT
        */
        HRESULT GetAudioFwVersion(
            uint32_t* header,
            uint32_t* version,
            uint32_t* componentProperty
        );

        //! A normal member function to select HidDevice
        //! requires InitHidAudioControlEndpoint() to be called first during session
        /*!
        \param path is WCHAR* to path to set HidDevice to
        \\
        \return HRESULT
        */
        void SelectHidDevice(
            const WCHAR* path
        );

        //! A normal member function to request Built in self test information from HID controlled audio device
        //! requires InitHidAudioControlEndpoint() to be called first during session
        /*!
        \param bistresults[] is 256 byte buffer array 
        \param bistResultsSize is size of the buffer (must be 256 right now)
        \\
        \return HRESULT
        */
        HRESULT GetSelfTestResults(
            BYTE bistResults[],
            int bistResultsSize
        );

        //! A normal member function to request Built in self test information from HID controlled audio device
        //! requires InitHidAudioControlEndpoint() to be called first during session
        /*!
        \param failureFound gives true/false result for if a failure is found
        \param firstFailedIndex outputs the failed index
        \param firstFailedStatus outputs the test result for the given failed index
        \\
        \return HRESULT
        */
        HRESULT GetFirstFailure(
            bool* failureFound,
            int32_t* firstFailedIndex,
            uint32_t* firstFailedStatus
        );

        //! A normal member function to request load information for each smart amp channel (Realtek)
        /*!
        \param ampIndex audioChannel from 0-7
        \param tempC is the temperature of the closest sensor nearest to the speaker channel in question
        \param dOhmsLoad outputs the load in dOhms (Ohms x 10)
        \\
        \return HRESULT
        */
        HRESULT RetrieveSmartAmpLoad(
            uint32_t ampIndex,
            int32_t tempC,
            uint32_t* dOhmsLoad);

        //! A normal member function to request Qualcomm smart amp calibration status (Only for QC products)
        /*!
        \param status is the calibration status
        \\
        \return HRESULT
        */
        HRESULT GetQCSmartAmpCalibrationStatus(__out QCSmartAmpCalibrationStatus& status);

        //! A normal member function to request Qualcomm smart amp calibration parameters (Only for QC products)
        /*!
        \param audioChannel is the channel to retreive the calibration for. Must be less than MAX_QC_SMARTAMP_CHANNELS
        \param paramsis the calibration parameters
        \\
        \return HRESULT
        */
        HRESULT RetrieveQCSmartAmpCalibration(uint32_t audioChannel, __out QCSmartAmpCalibrationParameters& params);
        
        //! A normal member function to force Qualcomm smart amp recalibration (Only for QC products)
        /*!
        \return HRESULT
        */
        HRESULT ForceQCSmartAmpRecalibration();

        //! A normal member function to request load information for each smart amp channel (Cirrus)
        /*!
        \param ampIndex from 0-3
        \param tempC is the temperature of the closest sensor nearest to the speaker channel in question
        \param ohmsLoad outputs the load in Ohms (only valid if success status)
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        */
        HRESULT CalibrateCirrusSmartAmp(
            __in uint32_t ampIndex,
            __in int32_t tempC,
            __out float* ohmsLoad,
            const int renderServiceKey = DEFAULT_KEY);


        //! A normal member function to apply calibration settings for each smart amp channel (Cirrus)
        /*!
        \param ampIndex from 0-3
        \param ohmsLoad is the floating point load of the amp that was gathered during calibration
        \\
        \return HRESULT
        */
        HRESULT ApplyCalibrationCirrusSmartAmp(
            __in uint32_t ampIndex,
            __in float ohmsLoad);

        //! A test member function to select tuning settings for each smart amp channel (Cirrus)
        /*!
        \param ampIndex from 0-3
        \param tuning is a tuning profile enumeration value (must have tuning knowledge to make selection)
        \\
        \return HRESULT
        */
        HRESULT SelectTuningCirrusSmartAmp(
            __in uint32_t ampIndex,
            __in int32_t tuning);


        //! A normal member function to request information for each smart amp channel to verify connectivity (Cirrus)
        /*!
        \param ampIndex from 0-3
        \\
        \return HRESULT
        */
        HRESULT PingCirrusSmartAmp(
            __in uint32_t ampIndex);


        //! A normal member function to load a tuning file for each smart amp channel (Cirrus)
        /*!
        \param ampIndex from 0-3
        \param tuningFilepath is the filePath of the tuning file
        \\
        \return HRESULT
        */
        HRESULT ApplyNewTuningCirrusSmartAmp(
            __in uint32_t ampIndex,
            __in const WCHAR* tuningFilepath
        );

        //! A normal member function to request SAF EFX DSP Param
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \param paramId unsigned int for parameter ID
        \param paramBlock unsigned char for parameter block
        \param length unsigned int for length
        \\
        \return HRESULT
        */
        HRESULT GetSafEfxDspParam(
            __in const WCHAR* endpointFriendlyName,
            __in uint32_t paramId,
            __out unsigned char* paramBlock,
            __in uint32_t length
        );

        //! A normal member function to set SAF EFX DSP Param
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \param paramId unsigned int for parameter ID
        \param paramBlock unsigned char for parameter block
        \param length unsigned int for length
        \\
        \return HRESULT
        */
        HRESULT SetSafEfxDspParam(
            __in const WCHAR* endpointFriendlyName,
            __in uint32_t paramId,
            __in unsigned char* paramBlock,
            __in uint32_t length
        );

        //! A normal member function to request form factor from SID driver
        /*!
        \param pFormFactor returns enumeration for form factor
        \param async will start a thread and pend on change (default = false)
        \\
        \return HRESULT
        */
        HRESULT GetCurrentFormFactor(
            __in UINT32* pFormFactor, 
            __in bool async = false);

        //! A normal member function to end the form factor event thread from above
        /*!
        */
        void StopFormFactorEventThread();

        //! A normal member function to request posture from SID driver
        /*!
        \param pPosture returns enumeration for form factor unless async is true.  When Async, pPosture is updated at the time of call and not again
        \param pFormFactor returns enumeration for form factor if provided as INVALID, otherwise will attempt to populate
        \param async will start a thread and pend on change (note that pPosture is not relavent and async is used for printing only) (default = false)
        \\
        \return HRESULT
        */
        HRESULT GetCurrentPosture(
            __out UINT32* pPosture, 
            __inout UINT32* pFormFactor, 
            __in bool async = false);

        //! A normal member function to end the posture event thread from above
        /*!
        */
        void StopPostureEventThread();

        //! Member function to get MTE Mode for a SAF Enabled endpoint.
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \param isMteMode a BOOL* arg to return the enable/disable MTE Mode status
        \\
        \return HRESULT
        */
        HRESULT GetSafMteMode(const WCHAR* endpointFriendlyName, bool* isMteMode);

        //! Member function to set MTE Mode for a SAF Enabled endpoint.
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \param isMteMode a BOOL arg to enable/disable MTE Mode
        \\
        \return HRESULT
        */
        HRESULT SetSafMteMode(const WCHAR* endpointFriendlyName, bool isMteMode);

        //! Member function to get bypass state of Personalized Eq Block of SAF Enabled endpoint.
        /*!
        \param bypassState is pointer to dword bypass state to get
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::GetPersonalizedEqBypassState(DWORD* bypassState, const WCHAR* endpointFriendlyName);

        //! Member function to set bypass state for Personalized Eq Block of SAF Enabled endpoint.
        /*!
        \param bypassState is pointer to dword bypass state to get
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::SetPersonalizedEqBypassState(DWORD bypassState, const WCHAR* endpointFriendlyName);

        //! Member function to get gain values for a Personalized Eq Block of SAF Enabled endpoint.
        /*!
        \param gainValues is pointer to float gain values to get
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::GetPersonalizedEqGainValues(float* gainValues, const WCHAR* endpointFriendlyName);

        //! Member function to set gain values for a Personalized Eq Block of SAF Enabled endpoint.
        /*!
        \param gainValues is pointer to float gain values to set
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::SetPersonalizedEqGainValues(float* gainValues, const WCHAR* endpointFriendlyName);

        //! Member function to get/print JSON Config Path of SAF Enabled endpoint.
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::GetEnsembleJsonPath(const WCHAR* endpointFriendlyName);

        //! Member function to set/print JSON Config Path of SAF Enabled endpoint.
        /*!
        \param newPath to wchar_t* path to set
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \param backup is a bool
        \param reset is a bool
        \\
        \return HRESULT
        */
        HRESULT  AudioTestFrameworkClass::SetEnsembleJsonPath(WCHAR* newPath, const WCHAR* endpointFriendlyName, bool backup, bool reset);

        //! Member function to test alternating gain for SAF EFX
        /*!
        \param endpointFriendlyName is a wide string or substring of the endpoint friendly name
        \\
        \return HRESULT
        */
        HRESULT TestAlternatingGainSafEfx(__in const WCHAR* endpointFriendlyName);

        //! Member function to set FX on/off for a given endpoint during playback/capture
        /*!
        \param flow is the enum to specify playback or capture
        \param effect is the WCHAR saying the effect name to be controlled
        \param enable is a bool to say if we are enabling or disabling
        \param captureServiceKey the key associated with a AudioCaptureService instance (default is DEFAULT_KEY)
        \param renderServiceKey the key associated with a AudioPlaybackService instance (default is DEFAULT_KEY)
        \\
        \return HRESULT
        */
        HRESULT SetAudioEffect(
            __in const EDataFlow flow,
            __in const WCHAR* effect,
            __in bool enable,
            const int captureServiceKey = DEFAULT_KEY,
            const int renderServiceKey = DEFAULT_KEY);

        //! Member function to initiate a blocking 1320 amp recalibration routine for single stereo amp.
        /*!
        \param tempCh0 temperature of the 0th coil in degrees C
        \param tempCh1 temperature of the 1th coil in degrees C
        \param loadOhmsCh0 calculated channel impedance in Ohms
        \param loadOhmsCh1 calculated channel impedance in Ohms
        \param loadRawCh0 raw hex value of the speaker impedance
        \param loadRawCh1 raw hex value of the speaker impedance
        \\
        \return HRESULT
        */
        HRESULT PerformStereoAlc1320Calibration(
            __in const float tempCh0,
            __in const float tempCh1,
            __out float& loadOhmsCh0,
            __out float& loadOhmsCh1,
            __out uint32_t& loadRawCh0,
            __out uint32_t& loadRawCh1);

        //! Member function to retrieve previously calibrated R0 values from 1320 amp
        /*!
        \param loadOhmsCh0 calculated channel impedance in Ohms
        \param loadOhmsCh1 calculated channel impedance in Ohms
        \param loadRawCh0 raw hex value of the speaker impedance
        \param loadRawCh1 raw hex value of the speaker impedance
        \\
        \return HRESULT
        */
        HRESULT RetrieveAlc1320Impedance(
            __out float& loadOhmsCh0,
            __out float& loadOhmsCh1,
            __out uint32_t& loadRawCh0,
            __out uint32_t& loadRawCh1);

        //! Member function to initiate a blocking 1320 amp recalibration routine for dual stereo amp config.
        /*!
        \param tempLCh0 temperature of the L 0th coil in degrees C
        \param tempLCh1 temperature of the L 1th coil in degrees C
        \param tempRCh0 temperature of the R 0th coil in degrees C
        \param tempRCh1 temperature of the R 1th coil in degrees C
        \param loadOhmsLCh0 calculated channel impedance in Ohms
        \param loadOhmsLCh1 calculated channel impedance in Ohms
        \param loadOhmsRCh0 calculated channel impedance in Ohms
        \param loadOhmsRCh1 calculated channel impedance in Ohms
        \param loadRawLCh0 raw hex value of the speaker impedance
        \param loadRawLCh1 raw hex value of the speaker impedance
        \param loadRawRCh0 raw hex value of the speaker impedance
        \param loadRawRCh1 raw hex value of the speaker impedance
        \\
        \return HRESULT
        */
        HRESULT PerformDualStereoAlc1320Calibration(
            __in const float tempLCh0,
            __in const float tempLCh1,
            __in const float tempRCh0,
            __in const float tempRCh1,
            __out float& loadOhmsLCh0,
            __out float& loadOhmsLCh1,
            __out float& loadOhmsRCh0,
            __out float& loadOhmsRCh1,
            __out uint32_t& loadRawLCh0,
            __out uint32_t& loadRawLCh1,
            __out uint32_t& loadRawRCh0,
            __out uint32_t& loadRawRCh1);

        //! Member function to retrieve previously calibrated R0 values from Dual 1320 amps
        /*!
        \param loadOhmsLCh0 calculated channel impedance in Ohms
        \param loadOhmsLCh1 calculated channel impedance in Ohms
        \param loadOhmsRCh0 calculated channel impedance in Ohms
        \param loadOhmsRCh1 calculated channel impedance in Ohms
        \param loadRawLCh0 raw hex value of the speaker impedance
        \param loadRawLCh1 raw hex value of the speaker impedance
        \param loadRawRCh0 raw hex value of the speaker impedance
        \param loadRawRCh1 raw hex value of the speaker impedance
        \\
        \return HRESULT
        */
        HRESULT RetrieveDualAlc1320Impedance(
            __out float& loadOhmsLCh0,
            __out float& loadOhmsLCh1,
            __out float& loadOhmsRCh0,
            __out float& loadOhmsRCh1,
            __out uint32_t& loadRawLCh0,
            __out uint32_t& loadRawLCh1,
            __out uint32_t& loadRawRCh0,
            __out uint32_t& loadRawRCh1);

        //! Member function to check the eFuse on the 1320 amplifier
        /*!
        \return HRESULT
        */
        HRESULT Check1320eFuse();

        //! Member function to set the mte mode of the 1320 amp
        /*!
        \param enable to declare if you want the mte mode enabled or disabled true/false
        \\
        \return HRESULT
        */
        HRESULT SetMteMode1320amp(
            __in const bool enable);

        HRESULT SetDual1320AmpChannelEnable(
            __in const int32_t enableBitMask);

        //! Member function to set the mte mode of the 721 amp
        /*!
        \param enable to declare if you want the mte mode enabled or disabled true/false
        \\
        \return HRESULT
        */
        HRESULT SetMteMode721codec(
            __in const bool enable);

    private:
        //! A private member destructor (private to make it a singleton)
        /*!
        \sa GetInstance()
        */
        AudioTestFrameworkClass();

        //! A private member destructor
        /*!
        */
        ~AudioTestFrameworkClass();

        static AudioTestFrameworkClass* spAudioTestFramework;

        static CritSection instance_lock;

        CWASAPILoopbackApp* _LoopbackApp;

        SueCreekHidControl* _sueCreekHid;

        SmartAmpCalibration* _SmartAmpCalibrationService;

        CMTEMode* _mteModeService;

        int _serviceIdentifier;

        AudioServices* _audioServices;

        //! A private member function taking no arguments that prints and logs mte mode setting retrieved directly from codec.
        //! Currently Logging only command.
        /*!
        \param MteSettings pointer to MTE settings
        \\
        \return HRESULT
        */
        HRESULT CheckMteModeSettings(void* MteSettings);

        //! Private Member function to enable/disable MTE Mode for Cirrus Smart Amps
        /*!
        \param enable a BOOL arg to enable/disable MTE Mode
        \\
        \return HRESULT
        */
        HRESULT SetMteModeCirrus(
            __in BOOL enable);

        //! A normal member function to get the specific bound service (capture, render, or capture loopback) and returning an HRESULT
        /*!
        \param serviceKey - map key reference
        \param pService is a reference to pointer of an audio service
        \param pServiceMap is a map that contains serviceKeys and associated audio service
        \\
        \return HRESULT
        \sa BindTo - Capture, Render, CaptureLoopback
        */
    };
}