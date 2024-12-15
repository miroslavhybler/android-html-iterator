///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include <string>
#include <map>
#include <iterator>
#include "StringUtils.h"
#include "PlatformUtils.h"

#ifndef ANDROID_HTML_ITERATOR_HTMLUTILS_H
#define ANDROID_HTML_ITERATOR_HTMLUTILS_H

namespace htmlUtils {


    /**
     * List of standard single tags.
     * @since 1.0.0
     */
    std::vector<std::string> singleTags = {
            "img",
            "input",
            "br",
            "hr",
            "meta",
            "link",
            "area",
            "base",
            "col",
            "source",
            "track",
            "wbr"
    };

    auto singleTagsIteratorBegin = singleTags.begin();
    auto singleTagsIteratorEnd = singleTags.end();


    /**
     * Extracts all attributes from tagBody and writes it into outMap. When tag attribute has no value,
     * attribute value is set to empty string.
     * @param tagBody Body of tag inside brackets without brackets '<' body '>'.
     * @param outMap Mutable map for holding extracted attributes.
     * @since 1.0.0
     */
    void getTagAttributes(
            const std::string &tagBody,
            std::map<std::string, std::string> &outMap
    ) {
        size_t length = tagBody.length();
        size_t i = 0;
        size_t lastIndexBeforeWhiteChar = 0;
        char ch;

        i = stringUtils::nextWhiteChar(tagBody, i, length);

        if (i == std::string::npos) {
            //There are not any attributes in tag
            return;
        }

        while (i < length) {
            ch = tagBody[i];
            auto isWhiteChar = stringUtils::isWhiteChar(ch);

            while (isWhiteChar && i < length) {
                i += 1;
                ch = tagBody[i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            // int ni = stringUtils::indexOf(tagBody, " ", i);
            if (i >= length) {
                //Tag has no attributes defined within it's body or we read all attributes already
                return;
            }
            size_t attributeNameStartIndex = i;
            bool isEqualSignRequired = false;

            while ((ch != '=') && i < length) {
                if (!isWhiteChar) {
                    if (isEqualSignRequired) {
                        //Attribute has no value
                        break;
                    }
                    lastIndexBeforeWhiteChar = i;
                }

                if (isWhiteChar && !isEqualSignRequired) {
                    //Character is white, end of attribute name
                    isEqualSignRequired = true;
                }

                ch = tagBody[++i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            bool isEqualSign = ch == '=';

            size_t attributeNameEndIndex = lastIndexBeforeWhiteChar;
            std::string attributeName = tagBody.substr(
                    attributeNameStartIndex,
                    attributeNameEndIndex -
                    attributeNameStartIndex + 1
            );

            if (isEqualSignRequired && !isEqualSign) {
                //In this case, attribute has no value
                outMap[attributeName] = "";
                i += 1;
                continue;
            } else {
                //Attribute has value

                //i + 1 because tagBody[i] == '='
                size_t attributeValueStartIndex = i + 1;
                size_t nextNonWhiteCharIndex = stringUtils::nextNonWhiteChar(
                        tagBody,
                        attributeValueStartIndex,
                        length
                );

                char nextNonWhiteChar = tagBody[nextNonWhiteCharIndex];
                size_t attributeValueEndIndex;
                if (nextNonWhiteChar == '"') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            tagBody,
                            "\"",
                            attributeValueStartIndex + 1
                    );

                } else if (nextNonWhiteChar == '\'') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            tagBody,
                            "\'",
                            attributeNameEndIndex + 1
                    );
                } else {
                    //Closing double quote or apostrophe not found, probably error in syntax, continue
                    i += 1;
                    continue;
                }

                //Plus 1 and minus 1 to remove " or ' from attribute value edges "value" -> value
                std::string attributeValue = tagBody.substr(
                        attributeValueStartIndex + 1,
                        attributeValueEndIndex - attributeValueStartIndex - 1
                );

                //TODO minimaze usage of trim
                stringUtils::trim(attributeName);
                stringUtils::trim(attributeValue);
                outMap[attributeName] = attributeValue;
                i = attributeValueEndIndex + 1;
            }
        }
    }


    void getTagAttributes(
            const std::string &content,
            std::map<std::string, std::string> &outMap,
            size_t s,
            size_t e
    ) {
        size_t i = s;
        size_t lastIndexBeforeWhiteChar = s;
        char ch;

        i = stringUtils::nextWhiteChar(content, i, e);

        if (i == std::string::npos) {
            //There are not any attributes in tag
            return;
        }

        while (i < e) {
            ch = content[i];
            auto isWhiteChar = stringUtils::isWhiteChar(ch);

            while (isWhiteChar && i < e) {
                i += 1;
                ch = content[i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            // int ni = stringUtils::indexOf(content, " ", i);
            if (i >= e) {
                //Tag has no attributes defined within it's body or we read all attributes already
                return;
            }
            size_t attributeNameStartIndex = i;
            bool isEqualSignRequired = false;

            while ((ch != '=') && i < e) {
                if (!isWhiteChar) {
                    if (isEqualSignRequired) {
                        //Attribute has no value
                        break;
                    }
                    lastIndexBeforeWhiteChar = i;
                }

                if (isWhiteChar && !isEqualSignRequired) {
                    //Character is white, end of attribute name
                    isEqualSignRequired = true;
                }

                ch = content[++i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            bool isEqualSign = ch == '=';

            size_t attributeNameEndIndex = lastIndexBeforeWhiteChar;
            std::string attributeName = content.substr(
                    attributeNameStartIndex,
                    attributeNameEndIndex -
                    attributeNameStartIndex + 1
            );

            if (isEqualSignRequired && !isEqualSign) {
                //In this case, attribute has no value
                outMap[attributeName] = "";
                continue;
            } else {
                //Attribute has value

                //i + 1 because content[i] == '='
                size_t attributeValueStartIndex = i + 1;
                size_t nextNonWhiteCharIndex = stringUtils::nextNonWhiteChar(
                        content,
                        attributeValueStartIndex,
                        e
                );

                char nextNonWhiteChar = content[nextNonWhiteCharIndex];
                size_t attributeValueEndIndex;
                if (nextNonWhiteChar == '"') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            content,
                            "\"",
                            attributeValueStartIndex + 1
                    );

                } else if (nextNonWhiteChar == '\'') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            content,
                            "\'",
                            attributeNameEndIndex + 1
                    );
                } else {
                    //Closing double quote or apostrophe not found, probably error in syntax, continue
                    continue;
                }

                //Plus 1 and minus 1 to remove " or ' from attribute value edges "value" -> value
                std::string attributeValue = content.substr(
                        attributeValueStartIndex + 1,
                        attributeValueEndIndex - attributeValueStartIndex - 1
                );

                stringUtils::trim(attributeName);
                stringUtils::trim(attributeValue);
                outMap[attributeName] = attributeValue;
                i = attributeValueEndIndex + 1;
            }
        }
    }


    /**
     * Extracts single attribute value for attribute name @attributeName from @tagBody
     * @param tagBody
     * @param name
     * @return
     * @since 1.0.0
     */
    std::string getTagAttributeValue(
            const std::string &tagBody,
            const std::string &name
    ) {
        size_t length = tagBody.length();
        size_t i = 0;
        size_t lastIndexBeforeWhiteChar = 0;
        char ch;

        i = stringUtils::nextWhiteChar(tagBody, i, length);

        if (i == std::string::npos) {
            //There are not any attributes in tag
            return "";
        }

        while (i < length) {
            ch = tagBody[i];
            auto isWhiteChar = stringUtils::isWhiteChar(ch);

            while (isWhiteChar && i < length) {
                i += 1;
                ch = tagBody[i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            // int ni = stringUtils::indexOf(tagBody, " ", i);
            if (i >= length) {
                //Tag has no attributes defined within it's body or we read all attributes already
                return "";
            }
            size_t attributeNameStartIndex = i;
            bool isEqualSignRequired = false;

            while ((ch != '=') && i < length) {
                if (!isWhiteChar) {
                    if (isEqualSignRequired) {
                        //Attribute has no value
                        break;
                    }
                    lastIndexBeforeWhiteChar = i;
                }

                if (isWhiteChar && !isEqualSignRequired) {
                    //Character is white, end of attribute name
                    isEqualSignRequired = true;
                }

                ch = tagBody[++i];
                isWhiteChar = stringUtils::isWhiteChar(ch);
            }

            bool isEqualSign = ch == '=';

            size_t attributeNameEndIndex = lastIndexBeforeWhiteChar;
            std::string attributeName = tagBody.substr(
                    attributeNameStartIndex,
                    attributeNameEndIndex -
                    attributeNameStartIndex + 1
            );

            bool isSearched = attributeName == name;

            if (isEqualSignRequired && !isEqualSign) {
                if (isSearched) {
                    //In this case, attribute has no value
                    return "";
                }
                continue;
            } else {
                //Attribute has value

                //i + 1 because tagBody[i] == '='
                size_t attributeValueStartIndex = i + 1;
                size_t nextNonWhiteCharIndex = stringUtils::nextNonWhiteChar(
                        tagBody,
                        attributeValueStartIndex,
                        length
                );

                char nextNonWhiteChar = tagBody[nextNonWhiteCharIndex];
                size_t attributeValueEndIndex;
                if (nextNonWhiteChar == '"') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            tagBody,
                            "\"",
                            attributeValueStartIndex + 1
                    );

                } else if (nextNonWhiteChar == '\'') {
                    attributeValueEndIndex = stringUtils::indexOf(
                            tagBody,
                            "\'",
                            attributeNameEndIndex + 1
                    );
                } else {
                    //Closing double quote or apostrophe not found, probably error in syntax, continue
                    return "";
                }

                //Plus 1 and minus 1 to remove " or ' from attribute value edges "value" -> value
                std::string attributeValue = tagBody.substr(
                        attributeValueStartIndex + 1,
                        attributeValueEndIndex - attributeValueStartIndex - 1
                );

                stringUtils::trim(attributeName);
                stringUtils::trim(attributeValue);
                return attributeValue;
            }
        }

        return "";
    }


    /**
     * Extracts name of tag from tagBody. This doesnt means name attribute but tag itself,
     * e.g. <code>&lt;p&gt; -> p</code>
     * @param tagBody Tag body without '<' and '>' chars to extract name from.
     * @return Name of the tag, value of "name" parameter.
     * @since 1.0.0
     */
    std::string getTagName(const std::string &tagBody) {
        std::string name = std::string(tagBody);

        if (tagBody.find(' ')) {
            size_t ei = stringUtils::indexOf(tagBody, " ", 0);
            if (ei > 0) {
                name = tagBody.substr(0, ei);
            }
        }
        // TODO improve
        stringUtils::trim(name);
        return name;
    }


    /**
    *
     * @param input Value of "class" attribute without wrapper chars (start/end "/' has to be removed
     * before).
    * @param outList Mutable list for holding extracted classes.
     * @since 1.0.0
    */
    void extractClassesFromString(
            const std::string_view &input,
            std::vector<std::string> &outList
    ) {
        size_t length = input.length();

        if (!outList.empty()) {
            outList.clear();
        }

        size_t s = std::string::npos;
        size_t e = std::string::npos;
        for (size_t i = 0; i < length; ++i) {
            char ch = input[i];

            if (stringUtils::isWhiteChar(ch)) {
                if (s != e && s != std::string::npos && e != std::string::npos) {
                    //+1 because is set on last non white char, needs to include it on substring length too
                    outList.emplace_back(input.substr(s, e - s + 1));
                    s = std::string::npos;
                    e = std::string::npos;
                }
            } else {
                if (s == std::string::npos) {
                    s = i;
                }
                e = i;
            }
        }


        //Checking if there is not class left
        if (s != e && s != std::string::npos && e != std::string::npos) {
            //+1 because is set on last non white char, needs to include it on substring length too
            outList.emplace_back(input.substr(s, e - s + 1));
        }
    }


    /**
     * Extracts values of class attribute from tagBody and writes them into outList.
     * @param tagBody Tag body without '<' and '>' chars to extract classes from.
     * @param outList Output list for holding extracted classes.
     * @since 1.0.0
     */
    void extractClasses(
            const std::string_view &tagBody,
            std::vector<std::string> &outList
    ) {
        std::string separator = "class=";
        size_t s = stringUtils::indexOf(tagBody, separator, 0);
        if (s == std::string::npos) {
            //Tag has no clazz
            return;
        }

        if (!outList.empty()) {
            outList.clear();
        }

        //s + 6 is next char after class=
        size_t valueStartIndex = stringUtils::nextNonWhiteChar(
                tagBody,
                s + 6,
                tagBody.length()
        );

        //Can be " or '
        char valueContainerChar = tagBody[valueStartIndex];

        //searching for valueContainerChar ending char
        size_t valueEndIndex = stringUtils::indexOfOrThrow(
                tagBody,
                valueContainerChar,
                valueStartIndex + 1
        );

        //-1 to remove "/'on the end
        std::string_view classValue = tagBody.substr(
                valueStartIndex + 1,
                valueEndIndex - valueStartIndex - 1
        );
        //+1 to skip "/'on the beginning
        return extractClassesFromString(classValue, outList);
    }


    /**
     * Checks whatever tag given by tagBody is single tag or pair tag.
     * @param tagBody Body of tag inside brackets '<' body '>'
     * @return True if tag from @tagBody is single tag, false when tag is pair tag.
     * @since 1.0.0
     */
    bool isSingleTag(std::string &tagBody) {
        size_t lastIndex = tagBody.length() - 1;
        bool hasClosing = tagBody.find('/', lastIndex) != std::string::npos;
        if (hasClosing) {
            return true;
        }

        std::string tagName = htmlUtils::getTagName(tagBody);
        auto result = std::find(
                singleTagsIteratorBegin,
                singleTagsIteratorEnd,
                tagName
        );

        if (result != singleTagsIteratorEnd) {
            return true;
        } else {
            return false;
        }
    }
}

#endif //ANDROID_HTML_ITERATOR_HTMLUTILS_H