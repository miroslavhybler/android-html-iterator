////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Java Native Interface (JNI) for the parser library.
///
/// Created by Miroslav Hýbler on 22.11.2024
///
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <jni.h>
#include <string>
#include "HtmlIterator.h"
#include "DebugLogCallback.h"
#include "JniHtmlIteratorCallback.h"

//Caller jobject htmlIterator is almost never used bust must be declared for jni functions.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnusedParameter"

namespace jni {
    HtmlIterator *instance = new HtmlIterator();
}


extern "C" JNIEXPORT void JNICALL
Java_com_htmliterator_HtmlIterator_setContent(
        JNIEnv *environment,
        jobject htmlIterator,
        jstring content
) {
    jboolean isCopy = false;
    std::string input = environment->GetStringUTFChars(content, &isCopy);
    jni::instance->setContent(input);
}

extern "C" JNIEXPORT void JNICALL
Java_com_htmliterator_HtmlIterator_setCallback(
        JNIEnv *environment,
        jobject htmlIterator,
        jobject callback
) {
    jni::instance->setCallback(
            new JniHtmlIteratorCallback(
                    environment,
                    callback
            )
    );
}

extern "C" JNIEXPORT void JNICALL
Java_com_htmliterator_HtmlIterator_iterate(
        JNIEnv *environment,
        jobject htmlIterato
) {
    jni::instance->iterate();
}


extern "C" JNIEXPORT jboolean JNICALL
Java_com_htmliterator_HtmlIterator_iterateSingleStep(
        JNIEnv *environment,
        jobject htmlIterato
) {
    bool canIterate = jni::instance->iterateSingleIteration();
    return static_cast<jboolean>(canIterate);
}


extern "C" JNIEXPORT void JNICALL
Java_com_htmliterator_HtmlIterator_setContentAndIterateDebug(
        JNIEnv *environment,
        jobject htmlIterator,
        jstring content
) {
    auto *callback = new DebugLogCallback();
    jboolean isCopy = false;
    std::string input = environment->GetStringUTFChars(content, &isCopy);
    jni::instance->setContent(input);
    jni::instance->setCallback(callback);
    jni::instance->iterate();

    callback = nullptr;
    jni::instance->clear();
}


extern "C" JNIEXPORT jboolean JNICALL
Java_com_htmliterator_HtmlIterator_getIsContentFullHtmlDocument(
        JNIEnv *environment,
        jobject htmlIterator
) {
    return static_cast<jboolean>(jni::instance->isContentFullHtmlDocument());
}


#pragma clang diagnostic pop