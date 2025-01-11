///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include <string>
#include <stack>
#include "HtmlIteratorCallback.h"
#include "StringUtils.h"
#include "TagInfo.h"
#include "PlatformUtils.h"

#ifndef ANDROID_HTML_ITERATOR_HTMLITERATOR_H
#define ANDROID_HTML_ITERATOR_HTMLITERATOR_H

/**
 * <h3>Usual Flow during processing</h3>
 * After setting both content and callback, program goes like this until it reaches end of the
 * content: <br>
 * 1. iterate()<br>
 * 2. iterateSingleIteration()<br>
 * 3. moveIndexToNextTag()<br>
 * 4. onTag()<br>
 * @since 1.0.0
 * @author Miroslav Hýbler <br>
 * created on 22.11.2024
 */
class HtmlIterator {

private:

    /**
     * Holding current html content text. Is set by @setContent. Can be whole html document content
     * most likely wrapped in <html> tag or can be a clip of html styled content to be processed.
     * @since 1.0.0
     */
    std::string content;


    /**
     * Holding current html content length. Is set by @setContent.
     * @since 1.0.0
     */
    size_t contentLength = 0;


    /**
     * Holding kotlinTagInfoStack of html <b>pair</b> tags as they are iterated. Tags are pushed when iterator detects
     * and enters pair tag and are popped out when iterator moves next behind the closing tag.
     * @since 1.0.0
     */
    std::stack<TagInfo> tagStack;


    /**
     * Callback used to deliver steps of iterations. Must be set by @setCallback, otherwise iterator
     * will not continue because there is no where to deliver result, so whole process would be useless.
     * @since 1.0.0
     */
    HtmlIteratorCallback *callback = nullptr;


    /**
     * Current index pointing to content.
     * @since 1.0.0
     */
    size_t currentIndex = 0;


    /**
     * @since 1.0.0
     */
    std::string currentSharedContent;


    /**
     * True when iterator is somewhere inside <pre> tag which is changing handling of white chars
     * in content. Inside <pre> tag all white chars are included in output.
     * @since 1.0.0
     */
    bool isPreContext = false;


    /**
     * True when iterator has gone through <head> tag of html content. This is for performance reason
     * because with boolean flag iterator doesn't have to compare current tag with <head> tag.
     * @since 1.0.0
     */
    bool isHeadIterated = false;


    /**
     * True when @content is full html document content wrapped in <html> tag. False when content is
     * just clip of content, e.g. short styled html text.
     * @since 1.0.0
     */
    bool isFullHtmlDocument = false;


    /**
     * Holds last character before currentIndex went into <pre> tag context to format output text correctly. By
     * html rules there cannot be two white chars displayed, only in <pre> tag.
     * @since 1.0.0
     */
    char lastCharBeforeFoundTag = 0;



    ////////////////////////////////////////////////////////////////////////////////////////////////
    /////
    /////   Public interface (constructors and functions)
    /////
    ////////////////////////////////////////////////////////////////////////////////////////////////


public:
    HtmlIterator() = default;

    ~HtmlIterator() {
        clear();
    }


    /**
     * Sets a new content for iterator, should be called together with @setCallback before @iterate.
     * Also all variables are reseted to initial state and all previous results are lost.
     * @param newContent
     * @since 1.0.0
     */
    void setContent(std::string &newContent) {
        clear();
        this->content.append(newContent);
        this->contentLength = newContent.length();
        this->isFullHtmlDocument = moveIndexToInitialPosition();
    }


    /**
     * Sets new callback, should be called together with @setContent before @iterate. Iterator
     * will not continue without callback.
     * @param newCallback
     * @since 1.0.0
     */
    void setCallback(HtmlIteratorCallback *newCallback) {
        this->callback = newCallback;
    }


    /**
     * Clear the resources used to release memory and resets all the variables into initial state.
     * @since 1.0.0
     */
    void clear() {
        this->content.clear();
        //  this->currentTagInfo.clear();
        this->currentSharedContent.clear();
        this->contentLength = 0;
        this->contentLength = 0;

        while (!this->tagStack.empty()) {
            this->tagStack.pop();
        }

        lastCharBeforeFoundTag = 0;
        currentIndex = 0;
        contentLength = 0;
        isPreContext = false;
        isHeadIterated = false;
        isFullHtmlDocument = false;
    }


    /**
     * Starts iteration through whole content which was set by @setContent before. Results are delivered
     * via @callback which was set by @setCallback before.
     * @since 1.0.0
     */
    //TODO more docs about whole process
    void iterate() {
        platformUtils::log("HtmlIterator", "HtmlIterator::iterate()");
        bool canIterate;
        do {
            canIterate = iterateSingleIteration();
        } while (canIterate);
        platformUtils::log("HtmlIterator", "HtmlIterator::iterate() -- done");
    }


    /**
     *
     * @return True if next iteration is possible, false otherwise.
     * @since 1.0.0
     */
    [[nodiscard]] bool iterateSingleIteration() {
        bool isTag = moveIndexToNextTag();
        if (isTag) {
            //Incoming sequence is html tag, need to obtain information about it
            onTag();
        }
        return currentIndex < contentLength;
    }


    /**
     *
     * @return True if content set by setContent() is full html document, false when content is only
     * html styled content.
     * @since 1.0.0
     */
    [[nodiscard]] bool isContentFullHtmlDocument() const {
        return this->isFullHtmlDocument;
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////
    /////
    /////   Private Functions
    /////
    ////////////////////////////////////////////////////////////////////////////////////////////////


private:


    /**
     * Tries to move currentIndex into next html tag. Technically it moves to the next '<' character
     * and checks if its tag or not. Also queries all text content depend on context.
     * @return True when sequence starting at i is tag to be processed, false otherwise
     * @since 1.0.0
     */
    [[nodiscard]] bool moveIndexToNextTag() {
        char ch = content[currentIndex];
        while (ch != '<' && currentIndex < contentLength) {
            tryAppendCharToContent(ch);
            lastCharBeforeFoundTag = ch;
            ch = content[++currentIndex];
        }
        //In this line, current char is < meaning that we are probably at the start of tag
        size_t outIndex;
        bool isTag = canProcessIncomingSequence(contentLength, currentIndex, outIndex);

        if (!isTag) {
            currentIndex = outIndex + 1;
        }
        return isTag;
    }


    /**
     * Called from moveIndexToNextTag when currentIndex is pointing to '<' char and sequence after it is valid
     * html tag (validated by canProcessIncomingSequence). Extracts current tag info into currentTagInfo and
     * call callback method to deliver result.
     * @since 1.0.0
     */
    //TODO create new function for processing tag and delivering result
    void onTag() {
        size_t tagEndIndex;
        try {
            tagEndIndex = stringUtils::indexOfOrThrow(content, ">", currentIndex);
        } catch (std::runtime_error &e) {
            platformUtils::log(
                    "Unable to find char '>' in content from index: "
                    + std::to_string(currentIndex) + ", content is not containing another tag",
                    ANDROID_LOG_ERROR
            );
            clear();
            return;
        }
        // -1 to remove '>' at the end
        size_t tagBodyLength = tagEndIndex - currentIndex - 1;
        //tag body within <>, currentIndex + 1 to remove '<'
        std::string currentTagBody = content.substr(
                currentIndex + 1,
                tagBodyLength
        );
        std::string tag = htmlUtils::getTagName(currentTagBody);
        bool isClosing = currentTagBody[0] == '/';

        if (!isHeadIterated) {
            //Skipping head tag
            if (stringUtils::equals(tag, "head")) {
                isHeadIterated = true;
                //index of < of closing tag
                size_t closingTagStartIndex;
                try {
                    closingTagStartIndex = findClosingTag(
                            tag,
                            tagEndIndex + 1,
                            contentLength,
                            contentLength
                    );
                } catch (std::runtime_error &e) {
                    //Html content can have syntax errors like unclosed pair tags or others,
                    //so library should keep going, browsers are also ignoring these errors
                    //Just keep parsing, just keep parsing
                    currentIndex = tagEndIndex + 1;
                    platformUtils::log("HtmlIterator", "Error: " + std::string(e.what()));
                    return;
                }
                // + 1 + 1 is or "/>" at the end of closing tag
                currentIndex = closingTagStartIndex + tag.length() + 1 + 1;
                return;
            }
        }

        //Extracts tag info from current tag body
        TagInfo info = TagInfo(tag, currentTagBody);

        if (callback != nullptr) {
            if (!currentSharedContent.empty()) {
                if (!(currentSharedContent.length() == 1
                      && stringUtils::isWhiteChar(currentSharedContent[0]))
                        ) {
                    //We can call onContentText only when currentSharedContent is not only one white char
                    callback->onContentText(currentSharedContent);
                }
                currentSharedContent.clear();
            }

            if (isClosing && !tagStack.empty()) {
                TagInfo lastTag = tagStack.top();
                callback->onLeavingPairTag(lastTag);

                if (stringUtils::equals(tag, "/pre")) {
                    isPreContext = false;
                }

                tagStack.pop();
                currentIndex = tagEndIndex + 1;
                return;
            }


            if (info.isSingleTag()) {
                callback->onSingleTag(info);
            } else {
                //TODO unit test
                if (stringUtils::equals(tag, "pre")) {
                    isPreContext = true;
                }

                //closing tag start index
                size_t closingTagStartIndex;
                try {
                    closingTagStartIndex = findClosingTag(
                            tag,
                            tagEndIndex + 1,
                            contentLength,
                            contentLength
                    );
                } catch (std::runtime_error &e) {
                    //Html content can have syntax errors like unclosed pair tags or others,
                    //so library should keep going, browsers are also ignoring these errors
                    //Just keep parsing, just keep parsing
                    currentIndex = tagEndIndex + 1;
                    platformUtils::log("HtmlIterator", "Error: " + std::string(e.what()));
                    return;
                }

                //Closing tag end index
                //+ 2 because of chars '/' and >;
                size_t closingTagEndIndex = closingTagStartIndex + tag.length() + 2;

                //TODO unit test
                if (stringUtils::equals(tag, "script")) {
                    callback->onScript(info);
                    currentIndex = closingTagStartIndex + 1;
                    return;
                } else {
                    //Adding tag into stack after closing tag is found succesfully to manage consistency
                    tagStack.push(info);

                    bool stepInto = callback->onPairTag(
                            info,
                            currentIndex,
                            tagEndIndex,
                            closingTagStartIndex,
                            closingTagEndIndex
                    );

                    if (stepInto) {
                        currentIndex = tagEndIndex + 1;
                    } else {
                        currentIndex = closingTagStartIndex + 1;
                    }
                }
                return;
            }
        } else {
            platformUtils::log("HtmlIterator", "Callback is null!!");
        }

        currentIndex = tagEndIndex + 1;
    }


    /**
     * Tries to append ch into sharedContent string if it's possible. Html content is full of white
     * characters that needs to be processed correctly before being appended into visible content.
     * <li>
     * <ul>
     * <b>Outside &lt;pre&gt; tag</b> - There can't be two white characters next to each other, so
     * when char is white, it's compared to last appended char if it can be connected.
     * </ul>
     * <ul>
     * <b>Inside &lt;pre&gt; tag:</b> - All white chars is appended to sharedContent to be visible.
     * </ul>
     * @param ch Char to be appended into sharedContent
     * @since 1.0.0
     */
    void tryAppendCharToContent(char ch) {
        if (isPreContext) {
            //Iterator is inside of <pre> tag somewhere, so every char (including all white chars)
            //has to be appended.
            currentSharedContent += ch;
            return;
        }

        bool isCharWhite = stringUtils::isWhiteChar(ch);
        if (!isCharWhite) {
            currentSharedContent += ch;
        } else {
            if (!currentSharedContent.empty()) {
                //We are outside of <pre> tag, always one white char is enabled to be visible between
                //other charters.
                size_t lastIndex = currentSharedContent.size() - 1;
                if (currentSharedContent[lastIndex] != ' ' && ch == ' ') {
                    //currentSharedContent's last char is not white, so we can append new char
                    currentSharedContent += ch;
                }
            }
        }
    }


    /**
     * Moves currentIndex to first tag of content.
     * @return True when content is full html document wrapped in <html> tag or some equivalent, false
     * when content is just some clipped html content (some <div> code or some formatted text).
     * @since 1.0.0
     */
    //TODO when first non white char is not <  return false.
    //TODO think about all the cases
    [[nodiscard]] bool moveIndexToInitialPosition() {
        size_t firstI = stringUtils::nextNonWhiteChar(content, 0, contentLength);
        if (firstI == std::string::npos) {
            //Content is "blank" (empty), containing only white chars
            this->currentIndex = contentLength;
            return false;
        }

        if (content[firstI] != '<') {
            return false;
        }

        this->currentIndex = firstI;


        while (!moveIndexToNextTag() && currentIndex < contentLength) {
            //Waiting for move index to the first tag of content
        }

        //TODO better implementation
        if ((currentIndex + 4) < contentLength) {
            std::string sub;
            size_t il = currentIndex + 1 + 4;
            try {
                sub = content.substr(currentIndex + 1, 4);
            } catch (std::out_of_range &e) {
                return false;
            }


            if (stringUtils::equals(sub, "html")) {
                this->currentIndex = il;
                return true;
            }
        }
        if ((currentIndex + 14) < contentLength) {
            std::string sub;
            size_t il = currentIndex + 1 + 13;
            try {
                sub = content.substr(currentIndex + 1, 13);
            } catch (std::out_of_range &e) {
                return false;
            }


            if (stringUtils::equalsCaseInsensitive(sub, "!doctype html")) {
                this->currentIndex = il;
                return true;
            }
        }

        return false;
    }


    /**
     * Called from @moveIndexToNextTag when @i is pointing to < char and iterator needs to know if
     * string sequence after < is valid tag or not.
     * @param input Input string.
     * @param l Length of @input
     * @param s Start index of incoming sequence, should always point at '<' character.
     * @param outIndex Index where end of the sequence will be written.
     * @return True if string being '<' ic actual tag, false otherwise.
     * @since 1.0.0
     */
    [[nodiscard]] bool canProcessIncomingSequence(
            const size_t &l,
            const size_t &s,
            size_t &outIndex
    ) {

        if (s >= l) {
            return false;
        }


        size_t i = s;
        outIndex = i;
        if ((i + 3) < l) {
            std::string sub;
            size_t il = i + 3;
            try {
                sub = content.substr(i + 1, 3);
            } catch (std::out_of_range &e) {
                return false;
            }

            if (stringUtils::equals(sub, "!--")) {
                //In this case next sequence after < is comment,skipping at the end of comment
                size_t ei;
                try {
                    ei = stringUtils::indexOfOrThrow(content, "-->", il);
                } catch (std::runtime_error &e) {
                    return false;
                }

                outIndex = ei + 3;
                return false;
            }
        }
        if (i + 12 < l) {
            std::string sub;
            size_t il = i + 12;
            try {
                sub = content.substr(i + 1, 12);
            } catch (std::out_of_range &e) {
                return false;
            }

            if (stringUtils::equals(sub, "/![cdata[//>")) {
                outIndex = il;
                return false;
            }
        }

        return true;
    }


    /**
    * Tries to find the right closing tag for tag. When tag contains same tags within like <p><p></p></p>
    * it will folds every same inner tag into a kotlinTagInfoStack and then popping it out. When the tag is found
    * and kotlinTagInfoStack for inner tags is empty, found tag is considered being right closing tag.
    *
    * Make sure to clip content or set index correctly. The content in which you are going to search
    * has to be with clipped of the start tag. Otherwise the opening tag would be pushed into kotlinTagInfoStack
    * too and program fails.
    * E.g:
    * searching for <p> must be in clipped content ..... </p>
    *
    * Note: There is no validation of pair tags inside, you are responsible for searching the proper
    * pair tag.
    * @param input Input string in which closing tag will be searched
    * @param tag Lowercase pair tag name you are searching for
    * @param s Start index
    * @param e End index. Optional, if value is less than 0, input.length() will be used.
    * @throw std::runtime_error When closing tag was not found within content
    * @return Index if start of the closing tag, index of '<' char
    * @since 1.0.0
    */
    [[nodiscard]] size_t findClosingTag(
            std::string &searchedTag,
            size_t s,
            size_t e,
            size_t length
    ) {
        size_t i = s;
        size_t outI = i;

        size_t tempWorkingNumber = 0;

        size_t end = e > 0 ? e : length;
        while (i < end) {
            char ch = content[i];
            if (ch != '<' && i < end) {
                i += 1;
                continue;
            }

            //char is '<'
            if (!canProcessIncomingSequence(length, i, outI)) {
                //Unable to process
                if (i == outI) {
                    i += 1;
                } else {
                    i = outI;
                }
                continue;
            }

            //TagType closing index, index of next '>'
            size_t tei = stringUtils::indexOfOrThrow(content, ">", i);
            // -1 to remove '>' at the end
            size_t tagBodyLength = tei - i - 1;
            //tag body within <>, currentIndex + 1 to remove '<'
            std::string tagBody = content.substr(i + 1, tagBodyLength);
            std::string rawTagName = htmlUtils::getTagName(tagBody);
            bool isClosingTag = rawTagName[0] == '/';

            if (isClosingTag) {
                std::string tagName = content.substr(i + 2, rawTagName.length() - 1);
                if (stringUtils::equals(tagName, searchedTag)) {
                    if (tempWorkingNumber > 0) {
                        //Stack is not empty, means that we found closing of inner same tag
                        tempWorkingNumber -= 1;
                    } else {
                        return i;
                    }
                }
            } else {
                if (stringUtils::equals(searchedTag, rawTagName)) {
                    //Push because inside tag is another one, like p in p -> <p><p>...</p></p>
                    tempWorkingNumber += 1;
                }
            }
            i = tei + 1;
        }

        throw std::runtime_error(
                "Unable to find closing tag for: " + std::string(searchedTag)
        );
    }


};


#endif //ANDROID_HTML_ITERATOR_HTMLITERATOR_H