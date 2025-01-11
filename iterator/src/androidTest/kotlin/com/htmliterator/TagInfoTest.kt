package com.htmliterator

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import kotlin.String


/**
 * Test that verifies TagInfo data class including attributes and classes parsing from html content.
 * All parsed attributes and classes should be without white chars and should be in lower case.
 * @author Miroslav Hýbler <br>
 * created on 11.12.2024
 */
@RunWith(AndroidJUnit4::class)
class TagInfoTest : BaseAndroidTest() {


    /**
     * Results definitions based on tag-info-test.html
     */
    object Results {

        /**
         * Defines expected result for [TagInfo]
         */
        data class TagResult constructor(
            val tag: String,
            val id: String = "",
            val attributes: Map<String, String> = emptyMap(),
            val classes: List<String> = emptyList(),
        ) {
            fun compareByValues(tagInfo: TagInfo): Boolean {
                //Classes are compared in classes, in attributes there is raw value inlcuding all the
                //white chars, almost impossible to compare it there.
                val attributesMatch = attributes.filter { it.key != "class" } ==
                        tagInfo.attributes.filter { it.key != "class" }

                return this.tag == tagInfo.tag
                        && this.id == (tagInfo.attributes["id"] ?: "")
                        && attributesMatch
                        && this.classes == tagInfo.classes
            }
        }


        val mainContentDiv: TagResult = TagResult(
            tag = "div",
            id = "main-content",
            attributes = mapOf(
                "align" to "center",
                "id" to "main-content",
            ),
            classes = listOf(
                "main",
                "container",
                "holder"
            )
        )

        val headerDiv: TagResult = TagResult(
            tag = "div",
            id = "header",
            attributes = mapOf(
                "id" to "header",
            ),
            classes = listOf(
                "header",
                "container"
            )
        )

        val contentDiv: TagResult = TagResult(
            tag = "div",
            id = "content",
            attributes = mapOf(
                "id" to "content",
            ),
            classes = listOf(
                "content",
                "container",
                "holder",
                "content-body"
            )
        )

        val a: TagResult = TagResult(
            tag = "a",
            id = "",
            attributes = mapOf(
                "href" to "https://www.example.com",
            ),
            classes = listOf(
                "image-link"
            )
        )

        val img: TagResult = TagResult(
            tag = "img",
            id = "content-image",
            attributes = mapOf(
                "src" to "https://www.example.com",
                "alt" to "Alternative text for image",
                "id" to "content-image",
            ),
            classes = listOf(
                "image"
            )
        )
    }


    /**
     *
     */
    class TagInfoTestCallback : HtmlIterator.Callback() {

        var mainContentDivResult: Boolean = false
        var headerDivResult: Boolean = false
        var contentDivResult: Boolean = false
        var aResult: Boolean = false
        var imgResult: Boolean = false

        var mainContentDivTagInfo: TagInfo? = null
        var headerDivTagInfo: TagInfo? = null
        var contentDivTagInfo: TagInfo? = null
        var aTagInfo: TagInfo? = null
        var imgTagInfo: TagInfo? = null

        override fun onSingleTag(tag: TagInfo) {
            when (tag.tag) {
                "img" -> {
                    imgTagInfo = tag
                    imgResult = Results.img.compareByValues(tagInfo = tag)
                }

                else -> throw IllegalStateException("Unknown tag presented in test")
            }
        }

        override fun onPairTag(
            tag: TagInfo,
            openingTagStartIndex: Int,
            openingTagEndIndex: Int,
            closingTagStartIndex: Int,
            closingTagEndIndex: Int
        ): Boolean {
            when (tag.tag) {
                "div" -> when (tag.attributes["id"]) {
                    "main-content" -> {
                        mainContentDivTagInfo = tag
                        mainContentDivResult = Results.mainContentDiv
                            .compareByValues(tagInfo = tag)
                    }

                    "header" -> {
                        headerDivTagInfo = tag
                        headerDivResult = Results.headerDiv
                            .compareByValues(tagInfo = tag)
                    }

                    "content" -> {
                        contentDivTagInfo = tag
                        contentDivResult = Results.contentDiv
                            .compareByValues(tagInfo = tag)
                    }
                }

                "a" -> {
                    aTagInfo = tag
                    aResult = Results.a.compareByValues(tagInfo = tag)
                }
            }
            return true
        }
    }


    val customCallback: TagInfoTestCallback = TagInfoTestCallback()


    @Test
    fun checkAllValues() {
        val content = loadAsset(fileName = "tag-info-test.html")
        iterator.setContent(content = content)
        iterator.setCallback(callback = customCallback)
        iterator.iterate()


        assertEquals(
            actual = customCallback.mainContentDivResult,
            expected = true,
            message = {
                failMessage(
                    label = "Main content div",
                    expected = Results.mainContentDiv,
                    given = customCallback.mainContentDivTagInfo,
                )
            }
        )
        assertEquals(
            actual = customCallback.headerDivResult,
            expected = true,
            message = {
                failMessage(
                    label = "Header div",
                    expected = Results.headerDiv,
                    given = customCallback.headerDivTagInfo
                )
            }
        )
        assertEquals(
            actual = customCallback.contentDivResult,
            expected = true,
            message = {
                failMessage(
                    label = "Content div",
                    expected = Results.contentDiv,
                    given = customCallback.contentDivTagInfo
                )
            }
        )
        assertEquals(
            actual = customCallback.aResult,
            expected = true,
            message = {
                failMessage(
                    label = "a",
                    expected = Results.a,
                    given = customCallback.aTagInfo,
                )
            }
        )
        assertEquals(
            actual = customCallback.imgResult,
            expected = true,
            message = {
                failMessage(
                    label = "img",
                    expected = Results.img,
                    given = customCallback.imgTagInfo
                )
            }
        )
    }


    private fun failMessage(
        label: String,
        expected: Results.TagResult,
        given: TagInfo?,
    ): String {
        return "$label test failed:\n " +
                "Expected: ${expected}\n" +
                "Given: ${given}"
    }
}