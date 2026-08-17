// <copyright file="TraceLogging.hpp" company="Microsoft Corporation">
//
//     The TraceLogging library is a single header, thread safe library that enables subsystem binaries 
//     to integrate with DBv2. All binaries that use this library will be plugged into our telemetry system.
//
//     Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <mutex>
#include <unordered_set>
#include <map>
#include <utility>
#include <locale>
#include <codecvt>
#include <bitset>
#include <functional>
#include <stdarg.h>
#include <algorithm>
#include <type_traits>
#include <iomanip>

#define TL_PARAM(TYPE) #TYPE, TYPE
#define TL_FORMATSTR_MULTI 100

#define TL_CAUSE() \
    std::wstring{__LPREFIX(__FUNCTION__)} + std::wstring{ L" Line: " } + std::to_wstring(__LINE__) \

#define TL_CONST_SEPARATOR L"::"
#define TL_CONST_STARTEVT L"StartEvent"
#define TL_CONST_ENDEVT L"EndEvent"
#define TL_CONST_CHANGEEVT L"ChangedEvent"
#define TL_CONST_THROWN_EXCEPT L"ThrownException"
#define TL_CONST_HANDLE_EXCEPT L"HandledException"
#define TL_CONST_LEVEL_PROPERTY L"Level"
#define TL_CONST_MESSAGE_PROPERTY L"Message"
#define TL_CONST_MODULE_PROPERTY L"ModuleId"
#define TL_CONST_THREAD_PROPERTY L"ThreadId"
#define TL_CONST_TIMESTAMP_PROPERTY L"Timestamp"
#define TL_CONST_DATA_PROPERTY L"Data"
#define TL_CONST_ELHH_PROPERTY L"ElapsedHours"
#define TL_CONST_ELMM_PROPERTY L"ElapsedMinutes"
#define TL_CONST_ELSS_PROPERTY L"ElapsedSeconds"
#define TL_CONST_ELMS_PROPERTY L"ElapsedMilliseconds"
#define TL_CONST_ELUS_PROPERTY L"ElapsedMicroseconds"
#define TL_CONST_CAUSE_PROPERTY L"CausedBy"
#define TL_CONST_TYPE_PROPERTY L"Type"
#define TL_CONST_NAME_PROPERTY L"Name"
#define TL_CONST_VALUE_PROPERTY L"Value"
#define TL_CONST_FILE_ARTIFACT_TYPE L"logfile"

namespace TraceLogging
{
    using namespace std;

    enum Level : unsigned int
    {
        // Fatal issues that cause the process to exit.
        Fatal = 1u,

        // Errors.
        Error = 2u,

        // Warnings
        Warning = 4u,

        // Completion of an operation that was successful.
        Result = 8u,

        // Important details.
        Info = 16u,

        // Information that might be helpful to understand existing behavior. 
        Verbose = 32u,

        // Details that assist when debugging issues. 
        Debug = 64u,

        // When an event happens or ends.
        LogEvent = 128u,

        // When an exception is caught or thrown.
        LogException = 256u
    };

    // A map containing the log level names, so that appenders can print the level name in the logs.
    const map<Level, std::wstring> LevelMap
    {
        make_pair(Level::Fatal, L"Fatal"),
        make_pair(Level::Error, L"Error"),
        make_pair(Level::Warning, L"Warning"),
        make_pair(Level::Result, L"Result"),
        make_pair(Level::Info, L"Info"),
        make_pair(Level::Verbose, L"Verbose"),
        make_pair(Level::Debug, L"Debug"),
        make_pair(Level::LogEvent, L"LogEvent"),
        make_pair(Level::LogException, L"LogException")
    };

    // Gets a string timestamp that is used for all appenders.
    inline const std::wstring GetCurrentTimestamp(const std::wstring& format = L"%Y-%m-%d %H:%M:%S.")
    {
        auto now_or = chrono::system_clock::now();
        auto now = chrono::system_clock::to_time_t(now_or);
        wchar_t buffer[256];
        tm ltm;
        localtime_s(&ltm, &now);
        wcsftime(buffer, 256, format.c_str(), &ltm);
        return std::wstring{ buffer } +to_wstring(chrono::duration_cast<std::chrono::milliseconds>
            (now_or.time_since_epoch()).count() % 1000);
    }

    // Replace all occurences of find with replace in original.
    inline void ReplaceAll(wstring& original, wstring find, wstring replace)
    {
        auto nextPosition = original.find(find);

        while (nextPosition != wstring::npos)
        {
            original.replace(nextPosition, find.size(), replace);
            nextPosition = original.find(find, nextPosition + replace.size());
        }
    }

    template<typename T>
    static wstring ToString(const T data)
    {
        wstringstream ss;
        ss << data;
        return ss.str();
    }

    // A configuration for the logger, this enables you to specify which levels will be send to stdout, stderr, appender.
    struct LoggerConfiguration
    {
        LoggerConfiguration(

            // Format specifiers are {message}, {level}, {module}, {timestamp}, {threadid}
            const wstring consoleFormatStr = L"{message}",
            // Format specifiers are {name} and {value} and ... means next result.
            const wstring resultMsgFormatStr = L"{name}: {value}\n...",
            // Format specifiers are {action} {message}, {cause}
            const wstring exceptionMsgFormatStr = L"{cause}: {message}",
            // Format specifiers are {action} {name} {value} {ehours} {eminutes} {eseconds} {emseconds} {euseconds}
            const wstring eventMsgFormatStr = L"{action}: {name}",

            const unordered_set<Level>&& sendToStdout = { Fatal, Error, Warning, Info },
            const unordered_set<Level>&& sendToStderr = { Fatal, Error },
            const unordered_set<Level>&& sendToAppender = { Fatal, Error, Warning, Result, Info, Verbose, Debug, LogEvent, LogException }) :
            m_sendtostderr{ move(sendToStderr) },
            m_sendtoformatter{ move(sendToAppender) },
            m_sendtostdout{ move(sendToStdout) },
            m_consoleformatstr{ consoleFormatStr },
            m_resultmsgformatstr{ resultMsgFormatStr },
            m_exceptionmsgformatstr{ exceptionMsgFormatStr },
            m_eventmsgformatstr{ eventMsgFormatStr }
        {
        }

        LoggerConfiguration(LoggerConfiguration&& config) :
            m_sendtostderr{ move(config.m_sendtostderr) },
            m_sendtostdout{ move(config.m_sendtostdout) },
            m_sendtoformatter{ move(config.m_sendtoformatter) },
            m_consoleformatstr{ config.m_consoleformatstr },
            m_resultmsgformatstr{ config.m_resultmsgformatstr },
            m_exceptionmsgformatstr{ config.m_exceptionmsgformatstr },
            m_eventmsgformatstr{ config.m_eventmsgformatstr }
        {
        }

        LoggerConfiguration(LoggerConfiguration& config) :
            m_sendtostderr{ config.m_sendtostderr },
            m_sendtostdout{ config.m_sendtostdout },
            m_sendtoformatter{ config.m_sendtoformatter },
            m_consoleformatstr{ config.m_consoleformatstr },
            m_resultmsgformatstr{ config.m_resultmsgformatstr },
            m_exceptionmsgformatstr{ config.m_exceptionmsgformatstr },
            m_eventmsgformatstr{ config.m_eventmsgformatstr }
        {
        }

        // Returns the log levels that will be sent to stdout.
        unordered_set<Level>& StdoutLevels()
        {
            return m_sendtostdout;
        }

        // Returns the log levels that will be sent to stderr.
        unordered_set<Level>& StderrLevels()
        {
            return m_sendtostderr;
        }

        // Returns the log levels that will be sent to the custom appender.
        unordered_set<Level>& AppenderLevels()
        {
            return m_sendtoformatter;
        }

        // Returns the console format string.
        const wstring ConsoleFormatStr(const wstring& message,
            const Level logLevel,
            const thread::id threadId,
            const int module,
            const wstring& timestamp)
        {
            wstring formatStr = m_consoleformatstr;

            ReplaceAll(formatStr, L"{message}", message);
            ReplaceAll(formatStr, L"{level}", LevelMap.find(logLevel)->second);
            ReplaceAll(formatStr, L"{threadid}", ToString(threadId));
            ReplaceAll(formatStr, L"{module}", ToString(module));
            ReplaceAll(formatStr, L"{timestamp}", timestamp);

            return formatStr;
        }

        void ConsoleFormatStr(wstring formatStr)
        {
            m_consoleformatstr = formatStr;
        }

        // Returns the console result message format string.
        const wstring ResultMsgFormatStr(wstring& name, wstring& value)
        {
            wstring formatStr = m_resultmsgformatstr;
            ReplaceAll(formatStr, L"{name}", name);
            ReplaceAll(formatStr, L"{value}", value);

            return formatStr;
        }

        void ResultMsgFormatStr(wstring formatStr)
        {
            m_resultmsgformatstr = formatStr;
        }

        template<typename T>
        // Returns the console event message format string.
        const wstring EventMsgFormatStr(const wstring& name,
            const T value,
            const wstring& action,
            const int elapsedhh = 0,
            const int elapsedmm = 0,
            const int elapsedss = 0,
            const int elapsedms = 0,
            const int elapsedus = 0)
        {
            wstring formatStr = m_eventmsgformatstr;

            ReplaceAll(formatStr, L"{name}", name);
            ReplaceAll(formatStr, L"{value}", ToString(value));
            ReplaceAll(formatStr, L"{action}", action);
            ReplaceAll(formatStr, L"{ehours}", ToString(elapsedhh));
            ReplaceAll(formatStr, L"{eminutes}", ToString(elapsedmm));
            ReplaceAll(formatStr, L"{eseconds}", ToString(elapsedss));
            ReplaceAll(formatStr, L"{emseconds}", ToString(elapsedms));
            ReplaceAll(formatStr, L"{euseconds}", ToString(elapsedus));

            return formatStr;
        }

        void EventMsgFormatStr(wstring formatStr)
        {
            m_eventmsgformatstr = formatStr;
        }

        // Returns the console exception format string.
        const wstring ExceptionMsgFormatStr(const wstring& action, const wstring& message, const wstring& causedBy)
        {
            wstring formatStr = m_exceptionmsgformatstr;

            ReplaceAll(formatStr, L"{action}", action);
            ReplaceAll(formatStr, L"{message}", message);
            ReplaceAll(formatStr, L"{cause}", causedBy);

            return formatStr;
        }

        void ExceptionMsgFormatStr(wstring formatStr)
        {
            m_exceptionmsgformatstr = formatStr;
        }

    private:
        unordered_set<Level> m_sendtostdout;
        unordered_set<Level> m_sendtostderr;
        unordered_set<Level> m_sendtoformatter;
        wstring m_consoleformatstr;
        wstring m_resultmsgformatstr;
        wstring m_eventmsgformatstr;
        wstring m_exceptionmsgformatstr;
    };

    // The base logger appender.
    struct LoggerAppender
    {
        // A pure virtual function that is implemented by custom appenders. This is called by the logger.
        // every time a message is logged and the level is allowed to be sent to the appender, as specified
        // in the configuration passed to the logger. 
        virtual void LogMessage(const Level loglevel,
            const int module,
            const thread::id threadid,
            const std::wstring& timestamp,
            const std::wstring& message) = 0;

        // A pure virtual function that is implemented by custom appenders. This is
        // called by the Logger to get the name of the log artifact type, so that it can 
        // log details to stdout of how to retrieve the log artifact.
        virtual std::wstring LogArtifactType() = 0;

        // A pure virtual function that is implemented by custom appenders. This is
        // called by the Logger to get the location of the log artifact, so that it can 
        // log details to stdout of how to retrieve the log artifact.
        virtual std::wstring LogArtifactLocation() = 0;

        // A mutex that is abstracted from custom appenders to enable thread safety.
        constexpr mutex& Mutex()
        {
            return m_mutex;
        }

    private:
        mutex m_mutex;
    };

    // A custom file appender, this will generate a log file in the same directory.
    struct FileAppender : LoggerAppender
    {
        // Move constructor.
        FileAppender(FileAppender&& other) :
            m_logfile{ move(other.m_logfile) }
        {
        }

        // Instantiates a FileAppender.
        FileAppender(const std::wstring& modulename) : LoggerAppender(), m_logfile{}
        {
            m_file_location = modulename + L"_" +
                GetCurrentTimestamp(L"%Y-%m-%d_%H.%M.%S.") + L".log";
            m_logfile.open(m_file_location, std::wofstream::out | std::wofstream::app);
        }

        ~FileAppender()
        {
            m_logfile.close();
        }

        // Logs the message to log file.
        void LogMessage(Level level,
            const int module,
            const thread::id threadid,
            const std::wstring& timestamp,
            const std::wstring& message) override
        {
            m_logfile << timestamp << TL_CONST_SEPARATOR << threadid
                << TL_CONST_SEPARATOR << module << TL_CONST_SEPARATOR <<
                LevelMap.find(level)->second << TL_CONST_SEPARATOR << message << endl;
        }

        std::wstring LogArtifactType()
        {
            return TL_CONST_FILE_ARTIFACT_TYPE;
        }

        std::wstring LogArtifactLocation()
        {
            return m_file_location;
        }

    private:
        std::wofstream m_logfile;
        std::wstring m_file_location;
    };

    // Singleton for creating logger with default file appender.
    struct DefaultFileAppender : FileAppender
    {
        DefaultFileAppender(DefaultFileAppender&) = delete;
        void operator=(DefaultFileAppender const&) = delete;

        static DefaultFileAppender& Instance(const std::wstring& moduleName)
        {
            static DefaultFileAppender instance{ moduleName };
            return instance;
        }

    private:
        DefaultFileAppender(const std::wstring& moduleName) : FileAppender(moduleName) {}
    };

    static wstring EscapeJsonString(const wstring& jsonString)
    {
        std::wostringstream o{};
        for (auto c = jsonString.cbegin(); c != jsonString.cend(); c++)
        {
            switch (*c)
            {
            case L'"': o << L"\\\""; break;
            case L'\\': o << L"\\\\"; break;
            case L'\b': o << L"\\b"; break;
            case L'\f': o << L"\\f"; break;
            case L'\n': o << L"\\n"; break;
            case L'\r': o << L"\\r"; break;
            case L'\t': o << L"\\t"; break;
            default:
                if (L'\x00' <= *c && *c <= L'\x1f')
                {
                    o << L"\\u"
                        << std::hex << std::setw(4) << std::setfill(L'0') << (int)*c;
                }
                else
                {
                    o << *c;
                }
            }
        }

        return o.str();
    }

    // The logger.
    struct Logger
    {
        // Instantiates an instance of Logger.
        Logger(const std::wstring& moduleName, bool jsonMode = false) :
            m_config{ move(LoggerConfiguration{}) },
            m_formatter{ DefaultFileAppender::Instance(moduleName) },
            m_json_mode{ jsonMode },
            m_results{}
        {
            LogFormatterArtifactInfo();
        }

        // Instantiates an instance of Logger.
        Logger(Logger& other) :
            m_config{ other.m_config },
            m_formatter{ other.m_formatter },
            m_json_mode{ other.m_json_mode },
            m_results{}
        {
            LogFormatterArtifactInfo();
        }

        // Instantiates an instance of Logger with a custom config and formatter.
        Logger(LoggerConfiguration&& config, LoggerAppender& formatter, bool deviceBridgeMode = false) :
            m_config{ move(config) },
            m_formatter{ formatter },
            m_json_mode{ deviceBridgeMode },
            m_results{}
        {
            LogFormatterArtifactInfo();
        }

        // Logs a message with a single level in a specified module using C-style format specifiers.
        void LogMessage(const Level level, const int module, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(level, module, format, args);
            va_end(args);
        }

        // Logs a message with a single level in module 0 using C-style format specifiers.
        void LogMessage(const Level level, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(level, 0, format, args);
            va_end(args);
        }

        // Logs a message with to multiple levels in a specified module using C-style format specifiers.
        void LogMessage(const unsigned int levelFlags, const int module, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            ForLogLevels(levelFlags, [this, format, args, &module](Level level)
            {
                LogMessage(level, module, format, args);
            });
            va_end(args);
        }

        // Logs a message with to multiple levels in module 0 using C-style format specifiers.
        void LogMessage(const unsigned int levelFlags, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            ForLogLevels(levelFlags, [this, format, args](Level level)
            {
                LogMessage(level, 0, format, args);
            });
            va_end(args);
        }

        // Logs a message with a single level in a specified module using C-style format specifiers.
        void LogMessage(const Level level, const int module, const wchar_t* format, const va_list args)
        {
            size_t size = wcslen(format) * sizeof(wchar_t) * TL_FORMATSTR_MULTI;
            auto str = make_unique<wchar_t*>(new wchar_t[size]);
            vswprintf_s(*str.get(), size, format, args);
            LogMessage(level, module, std::wstring{ *str.get() });
        }

        // Logs a message with a single level in module 0 using C-style format specifiers.
        void LogMessage(Level level, const wchar_t* format, const va_list args)
        {
            LogMessage(level, 0, format, args);
        }

        // Logs a message with to multiple levels in module 0 using C-style format specifiers.
        void LogMessage(const unsigned int levelFlags, const wchar_t* format, const va_list args)
        {
            LogMessage(levelFlags, 0, format, args);
        }

        // Logs a message with to multiple levels in a specified module using C-style format specifiers.
        void LogMessage(const unsigned int levelFlags, const int module, const wchar_t* format, const va_list args)
        {
            ForLogLevels(levelFlags, [this, format, args, &module](Level level)
            {
                LogMessage(level, module, format, args);
            });
        }

        // Logs a string message to a single level in module 0.
        void LogMessage(const Level level, const std::wstring& message)
        {
            LogMessage(level, 0, message);
        }

        // Logs a string message to multiple levels in module 0.
        void LogMessage(const unsigned int levelFlags, const std::wstring& message)
        {
            LogMessage(levelFlags, message);
        }

        // Logs a string message to multiple levels in a specified module.
        void LogMessage(const unsigned int levelFlags, const int module, const std::wstring& message)
        {
            ForLogLevels(levelFlags, [this, &module, &message](Level level)
            {
                LogMessage(level, module, message);
            });
        }

        // Logs a string message to a single level in a specified module.
        void LogMessage(const Level level, const int module, const std::wstring& message)
        {
            static mutex cout_mutex;
            static mutex cerr_mutex;

            std::wstring timestamp = GetCurrentTimestamp();

            // Determines whether this level needs to be logged to the custom appender.
            bool logToAppender =
                m_config.AppenderLevels().find(level) != m_config.AppenderLevels().end();

            // Determines whether this level needs to be logged to stderr.
            bool logToStderr =
                m_config.StderrLevels().find(level) != m_config.StderrLevels().end();

            // Determines whether this level needs to be logged to stdout. 
            // If in device bridge mode, we always log to stdout.
            bool logToStdout = m_json_mode ||
                m_config.StdoutLevels().find(level) != m_config.StdoutLevels().end();

            if (logToAppender)
            {
                lock_guard<mutex> lock(m_formatter.Mutex());
                m_formatter.LogMessage(level, module, this_thread::get_id(), timestamp, message);
            }

            if (logToStdout)
            {
                wstring str = FormatLogMessage(level, module, this_thread::get_id(), timestamp, message);
                lock_guard<mutex> lock(cout_mutex);
                wcout << str << endl;
            }

            if (logToStderr)
            {
                wstring str = FormatLogMessage(level, module, this_thread::get_id(), timestamp, message);
                lock_guard<mutex> lock(cerr_mutex);
                wcerr << str << endl;
            }
        }

        // Logs a fatal message to module 0 using C-style format specifiers.
        void LogFatal(const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Fatal, 0, format, args);
            va_end(args);
        }

        // Logs a fatal message to a specified module using C-style format specifiers.
        void LogFatal(const int module, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Fatal, module, format, args);
            va_end(args);
        }

        // Logs a fatal message to module 0.
        void LogFatal(const std::wstring& message)
        {
            LogMessage(Level::Fatal, 0, message);
        }

        // Logs a fatal message to specified module.
        void LogFatal(const int module, const std::wstring& message)
        {
            LogMessage(Level::Fatal, module, message);
        }

        // Logs an error message to module 0 using C-style format specifiers.
        void LogError(const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Error, 0, format, args);
            va_end(args);
        }

        // Logs an error to specified module using C-style format specifiers.
        void LogError(const int module, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Fatal, module, format, args);
            va_end(args);
        }

        // Logs an error message to module 0.
        void LogError(const std::wstring& message)
        {
            LogMessage(Level::Error, 0, message);
        }

        // Logs an error message to specified module .
        void LogError(const int module, const std::wstring& message)
        {
            LogMessage(Level::Error, module, message);
        }

        // Logs a warning message to module 0.
        void LogWarning(const std::wstring& message)
        {
            LogMessage(Level::Warning, 0, message);
        }

        // Logs a warning message to module 0 using C-style format specifiers.
        void LogWarning(const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Warning, 0, format, args);
            va_end(args);
        }

        // Logs a warning message to specified module.
        void LogWarning(const int module, const std::wstring& message)
        {
            LogMessage(Level::Warning, module, message);
        }

        // Logs a warning message to specified module using C-style format specifiers.
        void LogWarning(const int module, const wchar_t* format, ...)
        {
            va_list args;
            va_start(args, format);
            LogMessage(Level::Warning, module, format, args);
            va_end(args);
        }

        // Base variadic function to log parameters to module 0.
        template<typename N, typename T>
        void LogParameters(const N name, const T val)
        {
            std::wstringstream ss;
            ss << name << L" = " << val << L";";
            LogMessage(Level::Debug, ss.str());
        }

        // Logs parameters to specified module.
        template<typename N, typename T>
        void LogParameters(const unsigned int module, const N name, const T val)
        {
            std::wstringstream ss;
            ss << name << L" = " << val << L";";
            LogMessage(Level::Debug, module, ss.str());
        }

        // Logs parameters to specified module 0.
        template<typename N, typename T, typename... TArgs>
        void LogParameters(const int module, const N name, const T param, const TArgs... params)
        {
            LogParameters(module, name, param);
            LogParameters(module, params...);
        }

        // Logs parameters to module 0.
        template<typename N, typename T, typename... TArgs>
        void LogParameters(const N name, const T param, const TArgs... params)
        {
            LogParameters(name, param);
            LogParameters(params...);
        }

        void LogJsonResult(const std::wstring& json)
        {
            LogMessage(Level::Result, json);
        }

        // Logs results to module 0
        template<typename... TArgs>
        void LogResult(TArgs&... results)
        {
            m_results = { ResultString(results)... };
            CompleteLogResult();
        }

        constexpr bool InJsonMode()
        {
            return m_json_mode;
        }

        void InJsonMode(bool value)
        {
            m_json_mode = value;
        }

        LoggerConfiguration& Config()
        {
            return m_config;
        }

    private:
        LoggerConfiguration m_config;
        LoggerAppender& m_formatter;
        vector<wstring> m_results;
        bool m_json_mode;

        // Converts to json string.
        template<typename T>
        wstring ResultString(T& result)
        {
            bool useQuote = !is_integral<decltype(result.Value())>::value &&
                !is_same<decltype(result.Value()), bool>::value &&
                !is_floating_point<decltype(result.Value())>::value;

            wstringstream ss;
            wstring value = ToString(result.Value());

            if (m_json_mode)
            {
                ss << L"\""
                    << EscapeJsonString(result.Name()) << L"\"" << L":";

                if (useQuote)
                {
                    ss << L"\"" << EscapeJsonString(value) << L"\"";
                }
                else
                {
                    ss << value;
                }
            }
            else
            {
                auto name = result.Name();
                wstring formatStr = m_config.ResultMsgFormatStr(name, value);
                ss << formatStr;
            }

            return ss.str();
        }

        // Calls applier for all levels that are in level flags.
        void ForLogLevels(const unsigned int levelFlags, const function<void(const Level)> applier)
        {
            unsigned int mask = Level::Debug;
            while (mask > 0)
            {
                if (levelFlags & mask)
                {
                    applier(static_cast<Level>(mask));
                }

                mask >>= 1;
            }
        }

        void CompleteLogResult()
        {
            if (m_results.empty())
            {
                return;
            }

            std::wstringstream ss;

            if (m_json_mode)
            {
                ss << L"{";

                for (size_t i = 0; i < m_results.size(); i++)
                {
                    ss << m_results[i];

                    if (i + 1 < m_results.size())
                    {
                        ss << L", ";
                    }
                }

                ss << L"}";
            }
            else
            {
                wstring resultsstr = L"...";

                for (size_t i = 0; i < m_results.size(); i++)
                {
                    ReplaceAll(resultsstr, L"...", m_results[i]);
                }

                ReplaceAll(resultsstr, L"...", L"");

                ss << resultsstr;
            }

            LogMessage(Level::Result, ss.str());
        }

        void LogFormatterArtifactInfo()
        {
            std::wstringstream ss;
            ss << m_formatter.LogArtifactType() << L" = " << m_formatter.LogArtifactLocation();
            LogMessage(Level::Debug, ss.str());
        }

        std::wstring FormatLogMessage(
            const Level loglevel,
            const int module,
            const thread::id threadid,
            const std::wstring& timestamp,
            const std::wstring& message)
        {
            std::wstringstream sstream;

            if (m_json_mode)
            {
                sstream << L"{"
                    << L"\"" << TL_CONST_LEVEL_PROPERTY << L"\"" << L":" << L"\"" << LevelMap.find(loglevel)->second << L"\"" << L","
                    << L"\"" << TL_CONST_MODULE_PROPERTY << L"\"" << L":" << module << L","
                    << L"\"" << TL_CONST_THREAD_PROPERTY << L"\"" << L":" << threadid << L","
                    << L"\"" << TL_CONST_TIMESTAMP_PROPERTY << L"\"" << L":" << L"\"" << timestamp << L"\"" << L","
                    << L"\"" << TL_CONST_MESSAGE_PROPERTY << L"\"" << L":";

                if (loglevel != Level::Result && loglevel != Level::LogEvent && loglevel != Level::LogException)
                {
                    sstream
                        << L"\"" << EscapeJsonString(message) << L"\"";
                }
                else
                {
                    sstream << message;
                }

                sstream << "}";
            }
            else
            {
                wstring formatStr = m_config.ConsoleFormatStr(message, loglevel, threadid, module, timestamp);
                sstream << formatStr;
            }

            return sstream.str();
        }
    };

    // A log result. This should only be used at the end of your program.
    template <typename T>
    struct LogResult
    {
        // Instantiates an instance of LogResult.
        LogResult(const std::wstring& name, const T value, const std::wstring& description) :
            m_name{ name },
            m_value{ value },
            m_description{ description }
        {
        }

        // Move constructor.
        LogResult(LogResult&& result) :
            m_name{ move(result.m_name) },
            m_value{ move(result.m_value) },
            m_description{ move(result.m_description) }
        {
        }

        ~LogResult()
        {
        }

        // Gets the name of the result.
        constexpr const std::wstring Name()
        {
            return m_name;
        }
        // Gets the description of the result.
        constexpr const std::wstring Description()
        {
            return m_description;
        }

        // Gets the type of the result.
        constexpr std::wstring TypeName()
        {
            return typeid(T).name();
        }

        // Gets the value of the result.
        constexpr T Value()
        {
            return m_value;
        }

    private:
        const std::wstring m_name;
        T m_value;
        const std::wstring m_description;
    };

    // Creates an instance of LoggerResult<T>. Should only be used at the end of the process.
    template <typename T>
    auto make_result(const std::wstring& name, const T value)
    {
        return LogResult<T>{name, value, L"TBD"};
    }

    // An event that records elapsed time.
    template <typename T>
    struct Event
    {
        // Instantiates an instance of Event.
        Event(Logger& logger, const std::wstring& name, const T data, int module) :
            m_logger{ logger },
            m_name{ name },
            m_data{ data },
            m_module{ module }
        {
            std::wstringstream ss;

            if (logger.InJsonMode())
            {
                auto value = ToString(data);

                ss << L"{"
                    << L"\"" << TL_CONST_TYPE_PROPERTY << L"\"" << L":" << L"\"" << TL_CONST_STARTEVT << L"\"" << L","
                    << L"\"" << TL_CONST_NAME_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(name) << L"\"" << L","
                    << L"\"" << TL_CONST_DATA_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(value) << L"\""
                    << L"}";
            }
            else
            {
                wstring formatStr = m_logger.Config().EventMsgFormatStr(name, data, TL_CONST_STARTEVT);
                ss << formatStr;
            }

            logger.LogMessage(Level::LogEvent, module, ss.str());
            m_startpoint = chrono::steady_clock::now();
        }

        ~Event()
        {
            log_event(TL_CONST_ENDEVT);
        }

        // Gets the name of the event.
        constexpr const std::wstring Name()
        {
            return m_name;
        }

        // Gets the data in the event.
        constexpr const T Data()
        {
            return m_data;
        }

        // Sets the data in the event.
        void Data(const T data)
        {
            m_data = data;
            log_event(TL_CONST_CHANGEEVT);
        }

    private:
        const std::wstring m_name;
        const int m_module;
        T m_data;
        Logger& m_logger;
        chrono::time_point<chrono::steady_clock> m_startpoint;

        constexpr void log_event(const std::wstring& action)
        {
            auto duration = chrono::steady_clock::now() - m_startpoint;

            auto elapsedhh = chrono::duration_cast<chrono::hours>(duration);
            duration -= elapsedhh;
            auto elapsedmm = chrono::duration_cast<chrono::minutes>(duration);
            duration -= elapsedmm;
            auto elapsedss = chrono::duration_cast<chrono::seconds>(duration);
            duration -= elapsedss;
            auto elapsedms = chrono::duration_cast<chrono::milliseconds>(duration);
            duration -= elapsedms;
            auto elapsedus = chrono::duration_cast<chrono::microseconds>(duration);

            std::wstringstream ss;

            if (m_logger.InJsonMode())
            {
                wstring value = ToString(m_data);

                ss << L"{"
                    << L"\"" << TL_CONST_TYPE_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(action) << L"\"" << L","
                    << L"\"" << TL_CONST_NAME_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(m_name) << L"\"" << L","
                    << L"\"" << TL_CONST_DATA_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(value) << L"\"" << L","
                    << L"\"" << TL_CONST_ELHH_PROPERTY << L"\"" << L":" << elapsedhh.count() << L","
                    << L"\"" << TL_CONST_ELMM_PROPERTY << L"\"" << L":" << elapsedmm.count() << L","
                    << L"\"" << TL_CONST_ELSS_PROPERTY << L"\"" << L":" << elapsedss.count() << L","
                    << L"\"" << TL_CONST_ELMS_PROPERTY << L"\"" << L":" << elapsedms.count() << L","
                    << L"\"" << TL_CONST_ELUS_PROPERTY << L"\"" << L":" << elapsedus.count()
                    << L"}";
            }
            else
            {
                wstring formatStr = m_logger.Config().EventMsgFormatStr(m_name, m_data, action, elapsedhh.count(), elapsedmm.count(),
                    elapsedss.count(), elapsedms.count(), elapsedus.count());
                ss << formatStr;
            }

            m_logger.LogMessage(Level::LogEvent, m_module, ss.str());
        }
    };

    // Creates an instance of an Event<T>
    template <typename T>
    auto make_event(Logger& logger, const std::wstring& name, const T data, const int module = 0)
    {
        return Event<T>{logger, name, data, module};
    }

    // A base struct that represents an exception.
    struct Exception
    {
        Exception(Exception& other) = delete;

        // Creates an instance of Exception, use TraceLogging::TL_CAUSE for causedBy to record the function and line number.
        Exception(Logger& logger, const std::wstring& message, const std::wstring& causedBy, int module = 0) :
            m_logger{ logger },
            m_message{ message },
            m_causedby{ causedBy },
            m_module{ module }
        {
            std::wstringstream ss;

            if (logger.InJsonMode())
            {
                ss << L"{"
                    << L"\"" << TL_CONST_TYPE_PROPERTY << L"\"" << L":" << L"\"" << TL_CONST_THROWN_EXCEPT << L"\"" << L","
                    << L"\"" << TL_CONST_MESSAGE_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(message) << L"\"" << L","
                    << L"\"" << TL_CONST_CAUSE_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(causedBy) << L"\""
                    << L"}";
            }
            else
            {
                wstring formatStr = m_logger.Config().ExceptionMsgFormatStr(TL_CONST_THROWN_EXCEPT, message, causedBy);
                ss << formatStr;
            }

            m_logger.LogMessage(Level::LogException, module, ss.str());
        }

        // Move constructor.
        Exception(Exception&& other) :
            m_logger{ other.m_logger },
            m_message{ move(other.m_message) },
            m_causedby{ move(other.m_causedby) },
            m_module{ other.m_module }
        {}

        ~Exception()
        {
            std::wstringstream ss;

            if (m_logger.InJsonMode())
            {
                ss << L"{"
                    << L"\"" << TL_CONST_TYPE_PROPERTY << L"\"" << L":" << L"\"" << TL_CONST_HANDLE_EXCEPT << L"\"" << L","
                    << L"\"" << TL_CONST_MESSAGE_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(m_message) << L"\"" << L","
                    << L"\"" << TL_CONST_CAUSE_PROPERTY << L"\"" << L":" << L"\"" << EscapeJsonString(m_causedby) << L"\""
                    << L"}";
            }
            else
            {
                wstring formatStr = m_logger.Config().ExceptionMsgFormatStr(TL_CONST_HANDLE_EXCEPT, m_message, m_causedby);
                ss << formatStr;
            }

            m_logger.LogMessage(Level::LogException, m_module, ss.str());
        }

        // Gets the exception message.
        std::wstring Message()
        {
            return m_message;
        }

        // Gets the cause of the exception.
        std::wstring CausedBy()
        {
            return m_causedby;
        }

    private:
        Logger& m_logger;
        const std::wstring m_message;
        const std::wstring m_causedby;
        const int m_module;
    };

    inline std::wstring ascii_to_wide(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }
}