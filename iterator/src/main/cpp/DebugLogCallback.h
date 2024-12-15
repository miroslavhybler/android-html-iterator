///
/// Created by Miroslav Hýbler on 26.11.2024
///

#include <string>
#include "PlatformUtils.h"
#include "HtmlIteratorCallback.h"

#ifndef ANDROID_HTML_ITERATOR_DEBUGCALLBACK_H
#define ANDROID_HTML_ITERATOR_DEBUGCALLBACK_H

/**
 * Debug callback just showing logs from content iteration.
 * @since 1.0.0
 */
class DebugLogCallback : public HtmlIteratorCallback {

public:
    void onContentText(std::string &text) override {
        std::string message =
                "DebugLogCallback -- onContentText() -- text: " + std::string("\"") + text + "\"";
        platformUtils::log(
                "HtmlIterator",
                message
        );
    }

    void onSingleTag(TagInfo &tag) override {
        platformUtils::log(
                "HtmlIterator",
                "DebugLogCallback -- onSingleTag() -- tag: " + tag.getTag()
        );
    }

    bool onPairTag(
            TagInfo &tag,
            size_t openingTagStartIndex,
            size_t openingTagEndIndex,
            size_t closingTagStartIndex,
            size_t closingTagEndIndex
    ) override {
        platformUtils::log(
                "HtmlIterator",
                "DebugLogCallback -- onPairTag() -- tag: " + tag.getTag()
        );

        return true;
    };

    void onLeavingPairTag(TagInfo &tag) override {
        platformUtils::log(
                "HtmlIterator",
                "DebugLogCallback -- onLeavingPairTag() -- tag: " + tag.getTag()
        );
    }
};

#endif //ANDROID_HTML_ITERATOR_DEBUGCALLBACK_H
