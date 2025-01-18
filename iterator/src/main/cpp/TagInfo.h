///
/// Created by Miroslav Hýbler on 25.11.2024
///

#include <string>
#include <map>
#include "HtmlUtils.h"

#ifndef ANDROID_HTML_ITERATOR_TAGINFO_H
#define ANDROID_HTML_ITERATOR_TAGINFO_H

/**
 * Holding info of tag from html content
 * @since 1.0.0
 */
struct TagInfo {

private:
    std::map<std::string, std::string> attributes;
    std::vector<std::string> clazz;
    bool isSingle;
    std::string tag;
    std::string body;
    size_t pairContentStartIndex = 0;
    size_t pairContentEndIndex = 0;

public:
    TagInfo(
            std::string &newTag,
            std::string &newBody
    ) {
        this->tag = newTag;
        this->body = newBody;
        this->isSingle = htmlUtils::isSingleTag(this->body);
        htmlUtils::extractClasses(this->body, this->clazz);
        htmlUtils::getTagAttributes(this->body, this->attributes);

        if (this->isSingle) {
            std::string classesString = attributes["class"];
            if (!classesString.empty()) {
                htmlUtils::extractClasses(classesString, this->clazz);
            }
        }

    };

    TagInfo(
            std::string &newTag,
            std::string &newBody,
            std::map<std::string, std::string> &attributes
    ) {
        this->tag = newTag;
        this->body = newBody;
        this->attributes = attributes;
        this->isSingle = htmlUtils::isSingleTag(this->body);
    };

    ~TagInfo() = default;


    void setPairContent(
            const size_t start,
            const size_t end
    ) {
        this->pairContentStartIndex = start;
        this->pairContentEndIndex = end;
    }


    [[nodiscard]] std::string getTag() const {
        return this->tag;
    }


    [[nodiscard]] std::string getBody() const {
        return this->body;
    }


    [[nodiscard]] std::string getAttribute(const std::string &name) {
        return this->attributes[name];
    }


    [[nodiscard]] std::map<std::string, std::string> getOutMap() const {
        return this->attributes;
    }


    [[nodiscard]] std::vector<std::string> getClasses() const {
        return this->clazz;
    }


    [[nodiscard]] size_t getPairContentStartIndex() const {
        return pairContentStartIndex;
    }


    [[nodiscard]] size_t getPairContentEndIndex() const {
        return pairContentEndIndex;
    }


    /**
     *
     * @return True if holded @tag is single tag, false when tag is pair tag.
     * @since 1.0.0
     */
    [[nodiscard]] bool isSingleTag() const {
        return this->isSingle;
    }
};

#endif //ANDROID_HTML_ITERATOR_TAGINFO_H
