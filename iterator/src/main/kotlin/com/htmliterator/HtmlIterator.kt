@file:Suppress(
    "unused",
    "RedundantConstructorKeyword",
    "RedundantUnitReturnType",
)

package com.htmliterator

import androidx.annotation.RestrictTo


/**
 * @author Miroslav Hýbler <br>
 * created on 22.11.2024
 * @since 1.0.0
 */
class HtmlIterator private constructor() {


    companion object {
        init {
            //Load native library
            System.loadLibrary("html-iterator")
        }


        /**
         * Default instance of [com.htmliterator.HtmlIterator]
         * @since 1.0.0
         */
        val instance: HtmlIterator = HtmlIterator()
    }


    /**
     * Sets content to native iterator. Don't forget to call [setContent] before [iterate].
     * @since 1.0.0
     */
    external fun setContent(
        content: String,
    ): Unit


    /**
     * Sets [Callback] to iterator.
     * @since 1.0.0
     */
    external fun <C : Callback> setCallback(
        callback: C,
    ): Unit


    /**
     * @since 1.0.0
     */
    external fun iterate(): Unit


    /**
     * @since 1.0.0
     */
    external fun iterateSingleStep(): Boolean


    /**
     * @since 1.0.0
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
    external fun setContentAndIterateDebug(
        content: String,
    ): Unit


    /**
     * @since 1.0.0
     */
    open class Callback constructor() {


        /**
         * @since 1.0.0
         */
        open fun onContentText(
            text: String,
        ): Unit = Unit


        /**
         * @since 1.0.0
         */
        open fun onSingleTag(
            tag: TagInfo,
        ): Unit = Unit


        /**
         * @since 1.0.0
         */
        open fun onPairTag(
            tag: TagInfo,
            openingTagStartIndex: Int,
            openingTagEndIndex: Int,
            closingTagStartIndex: Int,
            closingTagEndIndex: Int,
        ): Unit = Unit


        /**
         * @since 1.0.0
         */
        open fun onLeavingPairTag(tag: TagInfo): Unit = Unit
    }
}