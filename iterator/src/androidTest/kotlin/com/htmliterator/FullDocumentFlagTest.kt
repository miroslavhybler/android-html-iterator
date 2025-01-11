package com.htmliterator

import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Test
import org.junit.runner.RunWith


/**
 * Checks if setting [HtmlIterator.isContentFullHtmlDocument] flag works correctly.
 * @author Miroslav Hýbler <br>
 * created on 11.01.2025
 * @since 1.0.0
 */
@RunWith(AndroidJUnit4::class)
class FullDocumentFlagTest : BaseAndroidTest() {


    val testFiles: List<String> = listOf(
        "full-doc-test1.html",
        "full-doc-test2.html",
        "full-doc-test3.html",
    )

    /**
     * Expected results
     * * **Key:** File name
     * * **Value:** Expected result
     */
    val results: Map<String, Boolean> = mapOf(
        testFiles[0] to true,
        testFiles[1] to false,
        testFiles[2] to true,
    )


    /**
     * Test for html tag.
     */
    @Test
    fun testHtmlTag() {
        testImpl(fileName = testFiles[0])
    }


    /**
     * Test for content starting with div tag.
     */
    @Test
    fun testDivTag() {
        testImpl(fileName = testFiles[1])
    }


    /**
     * Test for content starting with <!DOCTYPE HTML> tag.
     */
    @Test
    fun testDoctypeHtmlTag() {
        testImpl(fileName = testFiles[2])
    }


    /**
     * Performs test function, loads content by [fileName] and compares it to the expected result
     * from [results] map.
     * @param fileName Name of test file for the test, must be from [testFiles] list.
     */
    private fun testImpl(fileName: String) {
        iterator.setContent(
            content = loadAsset(fileName = fileName),
        )

        assertEquals(
            actual = iterator.isContentFullHtmlDocument,
            expected = results[fileName]!!,
        )
    }
}