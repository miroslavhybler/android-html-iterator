///
/// Created by Miroslav Hýbler on 22.11.2024
///

#include <string>

#ifndef ANDROID_HTML_ITERATOR_STRINGUTILS_H
#define ANDROID_HTML_ITERATOR_STRINGUTILS_H



/**
 * Namespace holding string utils for the library.
 * @author Miroslav Hýbler <br>
 * created on 22.11.2024
 * @since 1.0.0
 */
namespace stringUtils {


    /**
     * @since 1.0.0
     */
    std::function<bool(unsigned char)> trimPred = [](unsigned char ch) -> bool {
        return !std::isspace(ch);
    };

    /**
    *
    * @param ch
    * @return
     */
    bool isWhiteChar(char &ch) {
        //TODO check std::isSpace()
        return ch == ' ' || ch == '\n' || ch == '\t';
    }


    /**
     *
     * @param ch
     * @return
     */
    bool isNotWhiteChar(char &ch) {
        return !isWhiteChar(ch);
    }

    /**
     * Checks if strings s1 and s2 are same, or in other words, content of strings is same.
     * @param s1 String you want to compare with s2
     * @param s2 String you want to compare with s1
     * @return True if strings are considered being same. False otherwise.
     */
    bool fastCompare(
            const std::string_view &s1,
            const std::string_view &s2
    ) {
        if (s1.empty() && s2.empty()) {
            //Both strings are empty so they are considered same
            return true;
        }

        char ch1 = s1[0];
        char ch2 = s2[0];

        if (ch1 != ch2) {
            //First characters are different, strings are not same
            return false;
        }

        if (s1.size() != s2.size()) {
            //Size of strings differ so there is no way they are same
            return false;
        }

        return s1 == s2;
    }


    /**
    * Tries to find index of substring within input
    * @param input Input for searching substring
    * @param sub Substring you want to search
    * @param i Start index
    * @return index of first found substring, -1 if not found
    * @since 1.0.0
     */
    size_t indexOf(
            const std::string_view &input,
            const std::string &sub,
            const size_t &i
    ) {
        typename std::string_view::const_iterator sit = input.begin();
        std::advance(sit, i);
        typename std::string_view::const_iterator it = std::search(
                sit,
                input.end(),
                sub.begin(),
                sub.end()
        );

        if (it != input.end()) {
            return it - input.begin();
        }

        return std::string::npos;
    }


    /**
     * Implementation
     * @param input
     * @param ch
     * @param i
     * @return
     * @see https://cplusplus.com/reference/string/string/find/
     */
    size_t indexOf(
            const std::string_view &input,
            const char ch,
            const size_t &i
    ) {
        return input.find(ch, i);
    }


    /**
     *
     * @param input
     * @param requiredChar
     * @param i
     * @return
     */
    size_t nextNonWhiteCharRequired(
            const std::string_view &input,
            const char &requiredChar,
            const size_t &i
    ) {
        for (size_t j = i; j < input.length(); j++) {
            char ch = input[j];
            if (stringUtils::isWhiteChar(ch)) {
                continue;
            } else if (ch == requiredChar) {
                return j;
            } else if (ch != requiredChar) {
                return std::string::npos;
            }
        }
        return std::string::npos;
    }


    /**
     * Tries to find index of substring within input
     * @param input Input for searching substring
     * @param sub Substring you want to search
     * @param i Start index
     * @throws std::runtime_error when sub was not found within input from currentIndex
     * @return index of first found substring
     * @since 1.0.0
     */
    size_t indexOfOrThrow(
            const std::string_view &input,
            const std::string &sub,
            const size_t &i
    ) {
        size_t index = indexOf(input, sub, i);
        if (index == std::string::npos) {
            throw std::runtime_error(
                    "Substring \"" + sub +
                    "\"  was not found within input from index "
                    + std::to_string(i) + " from the input:\n"
                    + "=========================================\n"
                    + std::string(input)
                    + "\n=========================================\n"
            );
        }

        return index;
    }

    size_t indexOfOrThrow(
            const std::string_view &input,
            const char ch,
            const size_t &i
    ) {
        size_t index = indexOf(input, ch, i);
        if (index == std::string::npos) {
            throw std::runtime_error(
                    "Character \"" + std::string(1, ch) +
                    "\"  was not found within input from index "
                    + std::to_string(i) + " from the input:\n"
                    + "========================================="
                    + std::string(input)
                    + "========================================="
            );
        }

        return index;
    }


    /**
     * Trims input string @s from left (start) side
     * @param s Input string to be trimmed
     * @since 1.0.0
     */
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), trimPred));
    }


    /**
     * Trims input string @s from right (end) side
     * @param s Input string to be trimmed
     * @since 1.0.0
     */
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), trimPred).base(), s.end());
    }


    /**
     * Trims input string @s from both sides (start and end)
     * @param s Input string to be trimmed
     * @since 1.0.0
     */
    void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }


    /**
     * Splits input string @input by @separator and writes it into @outList
     * @param input Input string to be split
     * @param separator Separator character for split
     * @param outList Output list where result will be written.
     * @since 1.0.0
     */
    void split(
            std::string_view &input,
            const char &separator,
            std::vector<std::string_view> &outList
    ) {
        size_t s = 0;
        size_t i = 0;
        size_t l = input.length();

        outList.clear();

        if (input.find(separator, 0) == std::string_view::npos) {
            //Unable to split, separator was not found inside input
            outList.push_back(input);
            return;
        }

        while (i < l) {
            char ch = input[i];
            if (ch == separator) {
                std::string_view sub = input.substr(s, i - s);
                outList.push_back(sub);
                s = i + 1;
                i += 2;
            } else {
                i += 1;
            }
        }

        if (s != i && i - s > 0) {
            std::string_view sub = input.substr(s, i - s);
            outList.push_back(sub);
        }
    }


    void split(
            std::string &input,
            const char &separator,
            std::vector<std::string> &outList
    ) {
        size_t s = 0;
        size_t i = 0;
        size_t l = input.length();

        outList.clear();

        if (input.find(separator, 0) == std::string_view::npos) {
            //Unable to split, separator was not found inside input
            outList.push_back(input);
            return;
        }

        while (i < l) {
            char ch = input[i];
            if (ch == separator) {
                std::string sub = input.substr(s, i - s);
                outList.push_back(sub);
                s = i + 1;
                i += 2;
            } else {
                i += 1;
            }
        }

        if (s != i && i - s > 0) {
            std::string sub = input.substr(s, i - s);
            outList.push_back(sub);
        }
    }


    std::string listToString(std::vector<std::string_view> &list) {
        std::string output;


        for (size_t i = 0; i < list.size(); i++) {
            output += list[i];
            if (i != list.size() - 1) {
                output += ", ";
            }
        }


        return output;
    }


    /**
     *
     * @param input
     * @param start
     * @param end
     * @return
     */
    size_t nextWhiteChar(
            std::string_view input,
            size_t start,
            size_t end
    ) {
        char ch;
        for (size_t i = start; i < end; i++) {
            ch = input[i];
            if (stringUtils::isWhiteChar(ch)) {
                return i;
            }
        }

        return -1;
    }


    /**
     *
     * @param input
     * @param start
     * @param end
     * @return
     */
    size_t nextNonWhiteChar(
            std::string_view input,
            size_t start,
            size_t end
    ) {
        char ch;
        for (size_t i = start; i < end; i++) {
            ch = input[i];
            if (stringUtils::isNotWhiteChar(ch)) {
                return i;
            }
        }
        return std::string::npos;
    }
}


#endif //ANDROID_HTML_ITERATOR_STRINGUTILS_H
