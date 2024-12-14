///
/// Created by Miroslav Hýbler on 27.11.2024
///

#include <jni.h>
#include "HtmlIteratorCallback.h"
#include <codecvt>
#include <locale>

#ifndef ANDROID_HTML_ITERATOR_JNIHTMLITERATORCALLBACK_H
#define ANDROID_HTML_ITERATOR_JNIHTMLITERATORCALLBACK_H


/**
 *
 */
class JniHtmlIteratorCallback : public HtmlIteratorCallback {

private:

    /**
     * Jni environment used to call kotlin methods.
     * @since 1.0.0
     */
    JNIEnv *environment;


    /**
     * Holding safe global reference to kotlin callback object. Needs to be deleted when iterator is
     * no longer used.
     * @since 1.0.0
     */
    jobject callbackRef;


    /**
     * Stack of instances of Tag info on java side. Every jobject in stack is global reference and
     * needs to be deleted after leavingPairTag is called.
     * @since 1.0.0
     */
    std::stack<jobject> kotlinTagInfoStack = std::stack<jobject>();


public:
    JniHtmlIteratorCallback(
            JNIEnv *environment,
            jobject callback
    ) {
        this->environment = environment;
        //Original object lives only within caller function, needs to create global reference
        this->callbackRef = environment->NewGlobalRef(callback);
    }

    ~JniHtmlIteratorCallback() {
        environment = nullptr;
    }


    void onContentText(std::string &text) override {
        jmethodID methodId = environment->GetMethodID(
                environment->FindClass("com/htmliterator/HtmlIterator$Callback"),
                "onContentText",
                "(Ljava/lang/String;)V"
        );


        if (methodId == nullptr) {
            platformUtils::log(
                    "Unable to find method 'onContentText' in kotlin callback class.",
                    ANDROID_LOG_ERROR
            );
            return;
        }

        jstring jText = environment->NewStringUTF(text.c_str());
        environment->CallVoidMethod(callbackRef, methodId, jText);
        environment->DeleteLocalRef(jText);
    }


    void onSingleTag(TagInfo &tag) override {
        jmethodID methodId = environment->GetMethodID(
                environment->GetObjectClass(callbackRef),
                "onSingleTag",
                "(Lcom/htmliterator/TagInfo;)V"
        );

        if (methodId == nullptr) {
            platformUtils::log(
                    "Unable to find method 'onSingleTag' in kotlin callback class.",
                    ANDROID_LOG_ERROR
            );
            return;
        }

        jobject tagInfoKotlin = createKotlinTagInfo(tag);
        if (tagInfoKotlin == nullptr) {
            platformUtils::log(
                    "Error creating Kotlin TagInfo object!! Check createKotlinTagInfo() method implementation.",
                    ANDROID_LOG_ERROR
            );
            return;
        }

        environment->CallVoidMethod(callbackRef, methodId, tagInfoKotlin);
        environment->DeleteGlobalRef(tagInfoKotlin);

    }


    bool onPairTag(
            TagInfo &tag,
            size_t openingTagStartIndex,
            size_t openingTagEndIndex,
            size_t closingTagStartIndex,
            size_t closingTagEndIndex
    ) override {
        jmethodID methodId = environment->GetMethodID(
                environment->GetObjectClass(callbackRef),
                "onPairTag",
                "(Lcom/htmliterator/TagInfo;IIII)V"
        );

        if (methodId == nullptr) {
            platformUtils::log(
                    "Unable to find method 'onPairTag' in kotlin callback class.",
                    ANDROID_LOG_ERROR
            );
            return true;
        }

        jobject tagInfoKotlin = createKotlinTagInfo(tag);
        if (tagInfoKotlin == nullptr) {
            platformUtils::log(
                    "Error creating Kotlin TagInfo object!! Check createKotlinTagInfo() method implementation.",
                    ANDROID_LOG_ERROR
            );
            return true;
        }
        kotlinTagInfoStack.push(tagInfoKotlin);
        jint jOpeningTagStartIndex = static_cast<jint>(openingTagEndIndex);
        jint jOpeningTagEndIndex = static_cast<jint>(openingTagEndIndex);
        jint jClosingTagStartIndex = static_cast<jint>(closingTagStartIndex);
        jint jClosingTagEndIndex = static_cast<jint>(closingTagEndIndex);

        environment->CallVoidMethod(
                callbackRef,
                methodId,
                tagInfoKotlin,
                jOpeningTagStartIndex,
                jOpeningTagEndIndex,
                jClosingTagStartIndex,
                jClosingTagEndIndex
        );

        return true;
    }


    void onLeavingPairTag(TagInfo &tag) override {
        jmethodID methodId = environment->GetMethodID(
                environment->GetObjectClass(callbackRef),
                "onLeavingPairTag",
                "(Lcom/htmliterator/TagInfo;)V"
        );

        if (methodId == nullptr) {
            platformUtils::log(
                    "Unable to find method 'onLeavingPairTag' in kotlin callback class.",
                    ANDROID_LOG_ERROR
            );
            return;
        }

        //TODO store refs, this is making tagInfo twice (enter/exit)

        jobject tagInfoKotlin = kotlinTagInfoStack.top();
        environment->CallVoidMethod(callbackRef, methodId, tagInfoKotlin);
        environment->DeleteGlobalRef(tagInfoKotlin);
        kotlinTagInfoStack.pop();
    }


private:

    /**
     *
     * @param tagInfo
     * @return jobject if TagInfo was constructed onc java code successfully, nullptr otherwise
     * @since 1.0.0
     */
    jobject createKotlinTagInfo(TagInfo &tagInfo) {
        // Find the Kotlin TagInfo class
        jclass tagInfoClass = environment->FindClass("com/htmliterator/TagInfo");
        if (!tagInfoClass) {
            return nullptr; // Handle error
        }

        // Find the Kotlin TagInfo constructor
        jmethodID constructor = environment->GetMethodID(
                tagInfoClass,
                "<init>",
                // "(Ljava/lang/String;Ljava/lang/String;Z)V"
                "(Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;Ljava/util/List;Z)V"
        );
        if (!constructor) {
            return nullptr;
        }

        // Convert C++ fields to JNI types
        jstring tag = environment->NewStringUTF(tagInfo.getTag().c_str());
        jstring body = environment->NewStringUTF(tagInfo.getBody().c_str());

        // Convert attributes (std::map<std::string, std::string>) to Java Map
        jclass hashMapClass = environment->FindClass("java/util/HashMap");
        jmethodID hashMapConstructor = environment->GetMethodID(
                hashMapClass,
                "<init>",
                "()V"
        );
        jobject hashMap = environment->NewObject(hashMapClass, hashMapConstructor);

        jmethodID putMethod = environment->GetMethodID(
                hashMapClass,
                "put",
                "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;"
        );


        for (const auto &[key, value]: tagInfo.getOutMap()) {
            jstring jKey = environment->NewStringUTF(key.c_str());
            jstring jValue = environment->NewStringUTF(value.c_str());

            environment->CallObjectMethod(hashMap, putMethod, jKey, jValue);
            //    environment->DeleteLocalRef(jKey);
            //   environment->DeleteLocalRef(jValue);

        }

        // Convert clazz (std::vector<std::string_view>) to Java List
        jclass arrayListClass = environment->FindClass("java/util/ArrayList");
        jmethodID arrayListConstructor = environment->GetMethodID(
                arrayListClass,
                "<init>",
                "()V"
        );
        jobject arrayList = environment->NewObject(
                arrayListClass,
                arrayListConstructor
        );

        jmethodID addMethod = environment->GetMethodID(
                arrayListClass,
                "add",
                "(Ljava/lang/Object;)Z"
        );

        for (const auto &cls: tagInfo.getClasses()) {
            jstring tagClass = environment->NewStringUTF(cls.data());
            environment->CallBooleanMethod(arrayList, addMethod, tagClass);
            //   environment->DeleteLocalRef(tagClass);
        }

        // Create the Kotlin TagInfo object
        jobject kotlinTagInfo = environment->NewObject(
                tagInfoClass,
                constructor,
                tag,
                body,
                hashMap,
                arrayList,
                static_cast<jboolean>( tagInfo.isSingleTag())
        );

        return environment->NewGlobalRef(kotlinTagInfo);
    }
};

#endif //ANDROID_HTML_ITERATOR_JNIHTMLITERATORCALLBACK_H
