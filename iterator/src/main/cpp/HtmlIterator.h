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
     * Holding current html content text. Is set by <code>setContent</code>. Can be whole html
     * document content most likely wrapped in <html> tag or can be a clip of html styled content
     * to be processed.
     * @since 1.0.0
     */
    std::string content;


    /**
     * Holding current html content length. Is set by <code>setContent</code>.
     * @since 1.0.0
     */
    size_t contentLength = 0;


    /**
     * Holding TagInfo of <b>pair</b> tags as they are iterated. Tags are pushed when iterator detects
     * and enters pair tag and are popped out when iterator moves next behind the closing tag.
     * @since 1.0.0
     */
    std::stack<TagInfo> tagStack;


    /**
     * Holding TagInfo of <b>pair</b> tags as they are iterated. Unlike tagStack, tags are not popped
     * out when iterator leaves pair tag.
     * @since 1.0.0
     */
    std::stack<TagInfo> tagSequence;


    /**
     * Holds list of text content queried throught the process normalized based on context.
     * @since 1.0.0
     */
    std::stack<std::string> textNodes;


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
     * Holds text that have been queried between tags before it's pushed into [textNodes] stack.
     * @since 1.0.0
     */
    std::string currentTextNode;


    /**
     * True when iterator is somewhere inside <pre> tag which is changing handling of white chars
     * in content. Inside <pre> tag all white chars are included in output.
     * @since 1.0.0
     */
    bool isPreContext = false;


    /**
     * Helper flag, true when iterator has gone through <head> tag of html content. This is for
     * performance reason because with boolean flag iterator doesn't have to compare current
     * tag with <head> tag.
     * @since 1.0.0
     */
    bool isHeadIterated = false;


    /**
     * Helper flag, true when content is full html document content wrapped in <html> tag.
     * False when content is just clip of content, e.g. short styled html text or just plain text.
     * @since 1.0.0
     */
    bool isFullHtmlDocument = false;


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
        this->currentTextNode.clear();
        this->contentLength = 0;
        this->contentLength = 0;

        while (!this->tagStack.empty()) {
            this->tagStack.pop();
        }
        while (!this->tagSequence.empty()) {
            this->tagSequence.pop();
        }

        while (!this->textNodes.empty()) {
            this->textNodes.pop();
        }

        this->currentIndex = 0;
        this->contentLength = 0;
        this->isPreContext = false;
        this->isHeadIterated = false;
        this->isFullHtmlDocument = false;
    }


    /**
     * Starts iteration through whole content which was set by @setContent before. Results are delivered
     * via @callback which was set by @setCallback before.
     * @since 1.0.0
     */
    //TODO more docs about whole process
    void iterate() {
        platformUtils::log("HtmlIterator", "HtmlIterator::iterate()");

        if (callback == nullptr) {
            platformUtils::log("HtmlIterator", "Unable to iterate, callback is null!");
            return;
        }

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
     * Called from moveIndexToNextTag() when currentIndex is pointing to '<' char and sequence after it is valid
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

        if (isFullHtmlDocument && !isHeadIterated) {
            //Skipping head tag
            //TODO maybe remove skipping head tag
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


        if (isClosing && !tagStack.empty()) {
            TagInfo lastTag = tagStack.top();
            callback->onLeavingPairTag(lastTag);

            trySendContentText(lastTag);

            if (stringUtils::equals(tag, "/pre")) {
                isPreContext = false;
            }

            tagStack.pop();
            currentIndex = tagEndIndex + 1;
            return;
        }

        //Extracts tag info from current tag body
        TagInfo info = TagInfo(tag, currentTagBody);
        trySendContentText(info);

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
                info.setPairContent(
                        tagEndIndex + 1,
                        closingTagStartIndex
                );

                //Adding tag into stack after closing tag is found succesfully to manage consistency
                //of the stack.
                tagStack.push(info);
                tagSequence.push(info);

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
            currentTextNode += ch;
            return;
        }

        bool isCharWhite = stringUtils::isWhiteChar(ch);
        if (!isCharWhite) {
            currentTextNode += ch;
        } else {
            if (!currentTextNode.empty()) {
                //We are outside of <pre> tag, always one white char is enabled to be visible between
                //other charters.
                size_t lastIndex = currentTextNode.size() - 1;
                if (currentTextNode[lastIndex] != ' ' && ch == ' ') {
                    //currentTextNode's last char is not white, so we can append new char
                    currentTextNode += ch;
                }
            } else {
                currentTextNode += ch;
            }
        }
    }


    /**
     *
     * @param tag Pair tag in which is currentTextNode located
     */
    //TODO unit test on white chars and spaces handling
    void trySendContentText(
            TagInfo &tag
    ) {
        bool canBeSend = adjustSharedContentContextually(tag);

        if (canBeSend) {
            callback->onContentText(currentTextNode);
            //using emplace instead of push to get copy of currentTextNode string
            textNodes.emplace(currentTextNode);
        }
        currentTextNode.clear();


        //TODO move this functionality inside adjustSharedContentContextually if needed
        /*
        if (tagStack.empty() && !isFullHtmlDocument) {
            //When sequence is empty text can be send only if content is not full document
            callback->onContentText(currentTextNode);
            textNodes.emplace(currentTextNode);
            currentTextNode.clear();
            return;
        }

        //Pushing first text into stack, no need to do check with last entry
        callback->onContentText(currentTextNode);
        //using emplace instead of push to get copy of currentTextNode string
        textNodes.emplace(currentTextNode);

        currentTextNode.clear();
         */
    }


    /**
     * Adjusts <code>currentTextNode</code> content based on context, uses previous and current tag
     * to normalize white spaces in queried text.
     * <hr/>
     * <hr/>
     * <h3>Cases</h3>
     * <br/>
     * <h4><code>currentTextNode</code> is empty</h4>
     * <code>false</code> is returned as there is nothing to adjust or send to the <code>callback</code>.
     * <hr/>
     * <br/>
     * <h4>Inside <code>&lt;pre&gt;</code> tag</h4>
     * If <code>currentTextNode</code> is somewhere within <code>&lt;pre&gt;</code> tag context, no
     * adjustment is made as pre tag should preserve white spaces of the content. If there is no pre
     * context, <code>currentTextNode</code> is then normalized.
     * <hr/>
     * <br/>
     * <h4>Between container tags</h4>
     * After normalization <code>currentTextNode</code> needs to be adjusted based on context of
     * contain and surrounding tags. If <code>currentTextNode</code> is inside container tag or in
     * between container tags and starts with space (' '), the space must be erased.
     * Returning <code>false</code> only when <code>currentTextNode</code> is empty after adjustment.
     * <hr/>
     * <br/>
     * <h4>Between inline tags</h4>
     * After normalization <code>currentTextNode</code> needs to be adjusted based on context of
     * contain and surrounding tags. When <code>currentTextNode</code> is between inline tags and
     * starts with space (' ') its checked that if last text node also ends with space. If so,
     * space is erased, otherwise it can be kept in <code>currentTextNode</code>.
     * Returning <code>false</code> only when <code>currentTextNode</code> is empty after adjustment.
     *
     * <hr/>
     * <hr/>
     * @since 1.0.0
     * @return True when currentTextNode is valid non empty and can be sent to callback, false otherwise.
     */
    bool adjustSharedContentContextually(TagInfo &tag) {
        if (currentTextNode.empty()) {
            //Nothing to adjust when currentTextNode is empty
            //We don't have anything to adjust and to be send, this check is here so it's not
            // necessary to do check outside.
            return false;
        }

        if (isPreContext) {
            //No need adjustments inside <pre> tag, content should be passed as it is
            return true;
        }

        //All white chars can be included only inside <pre> tag, otherwise text must be normalized
        htmlUtils::normalizeText(currentTextNode);


        if (tagSequence.empty() || textNodes.empty()) {
            //No tag was found before, also any text, we don't know how to adjust content so we
            //just apply trim.
            stringUtils::trim(currentTextNode);

            if (currentTextNode.empty()) {
                return false;
            }
            return true;
        }


        TagInfo previousTag = tagSequence.top();
        bool isLastTagInline = htmlUtils::isInlineTag(previousTag.getTag());
        bool isTagInline = htmlUtils::isInlineTag(tag.getTag());

        std::string previousText = textNodes.top();

        if (!isTagInline || !isLastTagInline || stringUtils::endsWith(previousText, ' ')) {
            if (stringUtils::startsWith(currentTextNode, ' ')) {
                //We are outside of <pre> and tag is not inline, currentTextNode was normalized
                // and can have only one space at the beginning
                currentTextNode.erase(0, 1);
            }

            if (currentTextNode.empty()) {
                //Last tetNode ending with space (' '),  currentTextNode is empty, nothing to send.
                return false;
            }
        }

        if (isLastTagInline && isTagInline) {
            //In this case text is normalized, and adjusted, if both tags are inline tags,
            //currentTextNode can be send.
            return true;
        }

        //currentTextNode has passed, can be send.
        return true;
    }


    /**
     * Moves currentIndex to first tag of content.
     * @return True when content is full html document wrapped in <html> tag or some equivalent, false
     * when content is just some clipped html content (some <div> code or some formatted text).
     * @since 1.0.0
     */
    //TODO think about all stupid cases like random text before <html tag
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
     * Called from <code>moveIndexToNextTag</code> when index is pointing to < char and iterator needs to know if
     * string sequence after < is valid tag or not.
     * @param input Input string.
     * @param l Length of <code>input</code>
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