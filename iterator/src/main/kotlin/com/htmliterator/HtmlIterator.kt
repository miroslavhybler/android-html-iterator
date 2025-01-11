@file:Suppress(
    "unused",
    "RedundantConstructorKeyword",
    "RedundantUnitReturnType", "RedundantVisibilityModifier",
)

package com.htmliterator

import androidx.annotation.CallSuper
import androidx.annotation.RestrictTo
import java.util.Stack


/**
 * @author Miroslav Hýbler <br>
 * created on 22.11.2024
 * @since 1.0.0
 */
//TODO make object, doensn't make sense to have it as class
public class HtmlIterator private constructor() {


    public companion object {
        init {
            //Load native library
            System.loadLibrary("html-iterator")
        }


        /**
         * Default instance of [com.htmliterator.HtmlIterator]
         * @since 1.0.0
         */
        public val instance: HtmlIterator = HtmlIterator()
    }


    /**
     *
     */
    public val isContentFullHtmlDocument: Boolean
        get() = getIsContentFullHtmlDocument()


    /**
     * Sets content to native iterator. Don't forget to call [setContent] before [iterate].
     * @since 1.0.0
     */
    external fun setContent(
        content: String,
    ): Unit


    /**
     * Sets [callback] to iterator.
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
     * Use [isContentFullHtmlDocument].
     * @since 1.0.0
     */
    @RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
    external fun getIsContentFullHtmlDocument(): Boolean


    /**
     * @since 1.0.0
     */
    open class Callback constructor() {


        //TODO obtain stack from c++ instaead of creating another in KT
        val tagStack: Stack<TagInfo> = Stack()


        var currentPairTag: TagInfo? = null
            private set

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
        //TODO indexes maybe not necessary
        @CallSuper
        open fun onPairTag(
            tag: TagInfo,
            openingTagStartIndex: Int,
            openingTagEndIndex: Int,
            closingTagStartIndex: Int,
            closingTagEndIndex: Int,
        ): Boolean {
            tagStack.add(element = tag)
            currentPairTag = tag

            return true
        }


        /**
         * @since 1.0.0
         */
        open fun onLeavingPairTag(
            tag: TagInfo,
        ): Unit {
            if (tagStack.isNotEmpty()) {
                tagStack.pop()
                currentPairTag = if (tagStack.isNotEmpty()) tagStack.peek() else null
            }
        }


        /**
         * @since 1.0.0
         */
        open fun onScript(
            tag: TagInfo,
        ): Unit = Unit
    }
}