@file:Suppress("DATA_CLASS_COPY_VISIBILITY_WILL_BE_CHANGED_WARNING")

package com.htmliterator


/**
 * @author Miroslav Hýbler <br>
 * created on 27.11.2024
 * @since 1.0.0
 */
data class TagInfo internal constructor(
    val tag: String,
    val body: String,
    val attributes: Map<String, String>,
    val classes: List<String>,
    val isSingle: Boolean
) {


    /**
     * Tag body removed from string because it could mess up readability.
     */
    override fun toString(): String {
        return "TagInfo(tag='$tag', attributes=$attributes, classes=$classes, isSingle=$isSingle)"
    }
}