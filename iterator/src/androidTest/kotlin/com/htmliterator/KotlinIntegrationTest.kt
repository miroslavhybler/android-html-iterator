package com.htmliterator

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Test
import org.junit.runner.RunWith

/**
 * Tests that verifies integration and availability of iterator library in kotlin. In this class all
 * cases which including calling java/kotlin methods directly from c++:
 * * Construction of TagInfo data class
 * * Settings [HtmlIterator.Callback] jobject reference in c++
 * * Calling [HtmlIterator.Callback] functions with results
 * Then the test is done by retrieved data and compare to expected [Results]
 * @author Miroslav Hýbler <br>
 * created on 27.11.2024
 */
@RunWith(AndroidJUnit4::class)
class KotlinIntegrationTest : BaseAndroidTest() {


    /**
     * Results for integration test from kotlin-integration-test.html file.
     */
    data object Results {
        const val SINGLE_TAGS_COUNT: Int = 3
        const val PAIR_TAGS_COUNT: Int = 9
        const val TEXT_CONTENT: Int = 7
    }


    /**
     * Custom callback for testing, increasing values as methods getting called. All counts has to be
     * bigger than 0
     */
    class KotlinIntegrationTestCallback : HtmlIterator.Callback() {
        var singleTagsCount = 0
        var textsCount = 0
        var pairTagsCount = 0
        var pairTagsLeft = 0

        override fun onContentText(text: String) {
            textsCount += 1
        }

        override fun onSingleTag(tag: TagInfo) {
            singleTagsCount += 1
        }

        override fun onPairTag(
            tag: TagInfo,
            openingTagStartIndex: Int,
            openingTagEndIndex: Int,
            closingTagStartIndex: Int,
            closingTagEndIndex: Int,
        ): Boolean {
            super.onPairTag(
                tag=tag,
                openingTagStartIndex=openingTagStartIndex,
                openingTagEndIndex=openingTagEndIndex,
                closingTagStartIndex=closingTagStartIndex,
                closingTagEndIndex=closingTagEndIndex,
            )
            pairTagsCount += 1
            return true
        }

        override fun onLeavingPairTag(tag: TagInfo) {
            super.onLeavingPairTag(tag=tag)
            pairTagsLeft += 1
        }
    }

    val customCallback = KotlinIntegrationTestCallback()


    /**
     *
     */
    @Test
    fun checkKotlinDefinition() {

        iterator.setCallback(callback = customCallback)
        iterator.setContent(content = loadAsset(fileName = "kotlin-integration-test.html"))
        iterator.iterate()

        assertEquals(
            actual = customCallback.singleTagsCount,
            expected = Results.SINGLE_TAGS_COUNT,
        )

        assertEquals(
            actual = customCallback.pairTagsCount,
            expected = Results.PAIR_TAGS_COUNT,
        )
        assertEquals(
            actual = customCallback.textsCount,
            expected = Results.TEXT_CONTENT,
        )
        //Count of pair tags left must be equal to pair tags count
        assertEquals(
            actual = customCallback.pairTagsLeft,
            Results.PAIR_TAGS_COUNT,
        )
    }
}