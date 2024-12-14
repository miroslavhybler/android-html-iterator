///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include "TagInfo.h"

#ifndef ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H
#define ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H

/**
 * @since 1.0.0
 */
class HtmlIteratorCallback {

public:


    /**
     * Called from HtmlIterator when plain text is found.
     * @param text
     * @since 1.0.0
     */
    virtual void onContentText(std::string &text) = 0;


    /**
     *
     * @param tag
     * @since 1.0.0
     */
    virtual void onSingleTag(TagInfo &tag) = 0;


    /**
     * Called from HtmlIterator when pair tag is found.
     * @param tag
     * @return True when iterator should iterate through content of pair tag, false otherwise.
     * @since 1.0.0
     */
    virtual bool onPairTag(
            TagInfo &tag,
            size_t openingTagStartIndex,
            size_t openingTagEndIndex,
            size_t closingTagStartIndex,
            size_t closingTagEndIndex
    ) = 0;


    /**
     * Called from HtmlIterator when leaving pair tag and tag is popped out from kotlinTagInfoStack.
     */
    virtual void onLeavingPairTag(TagInfo &tag) = 0;

};

#endif //ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H