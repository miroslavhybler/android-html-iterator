///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include <android/log.h>
#include <string>

#ifndef ANDROID_HTML_ITERATOR_PLATFORMUTILS_H
#define ANDROID_HTML_ITERATOR_PLATFORMUTILS_H


/**
* True when logging is enabled, false otherwise. Used for development and debugging only,
* doesn't make sense to use logging in release mode.
* @since 1.0.0
*/
#if IS_LOGGING_ENABLED
inline constexpr bool isLoggingEnabled = true;
#else
inline constexpr bool isLoggingEnabled = false;
#endif


/**
 * Holds code specific to Android platform.
 * @since 1.0.0
 */
namespace platformUtils {


    /**
    * Logs message in android logcat. Keep in mind that logging should be used for development purposes
    * only, any release of library should not include much logs from processing because it's slowing
    * it down.
    * @param tag Tag of the message
    * @param message Message body
    * @param priority Priority of the log from @android_LogPriority
    * @since 1.0.0
    */
    void log(
            const char *tag,
            const std::string &message,
            android_LogPriority priority = ANDROID_LOG_DEBUG
    ) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"
        if (!isLoggingEnabled) {
            //This is not const, it depends on debug/development/release build type
            return;
        }
#pragma clang diagnostic pop

        __android_log_print(
                priority,
                tag,
                "%s",
                message.c_str()
        );
    }

    /**
    * Logs message in android logcat. Keep in mind that logging should be used for development purposes
    * only, any release of library should not include much logs from processing because it's slowing
    * it down.
    * @param message Message body
    * @param priority Priority of the log from @android_LogPriority
    * @since 1.0.0
    */
    void log(
            const std::string &message,
            android_LogPriority priority = ANDROID_LOG_DEBUG
    ) {
        log("HtmlIterator", message, priority);
    }

}

#endif //ANDROID_HTML_ITERATOR_PLATFORMUTILS_H
