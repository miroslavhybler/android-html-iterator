///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include "TagInfo.h"

#ifndef ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H
#define ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H


/**
 * Callback interface for HtmlIterator to deliver results. Can be implemented directly in c++ or
 * in java using JniHtmlIteratorCallback to deliver results into kotlin HtmlIterator#Callback.
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
     * Called from HtmlIterator when single tag is found.
     * @param tag
     * @since 1.0.0
     */
    virtual void onSingleTag(TagInfo &tag) = 0;


    /**
     * Called from HtmlIterator when script tag is found
     * @param tag
     */
    virtual void onScript(TagInfo &tag) = 0;


    /**
     * Called from HtmlIterator when pair tag is found.
     * @param tag
     * @return True when iterator should iterate through content of pair tag, false otherwise, allowing
     * to add some custom filtering logic.
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
     * Called from HtmlIterator when leaving pair tag.
     */
    virtual void onLeavingPairTag(TagInfo &tag) = 0;

};

#endif //ANDROID_HTML_ITERATOR_HTMLITERATORCALLBACK_H