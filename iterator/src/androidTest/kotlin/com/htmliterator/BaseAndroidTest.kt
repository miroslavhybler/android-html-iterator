@file:Suppress("RedundantUnitReturnType")

package com.htmliterator

import android.R
import android.content.Context
import android.content.res.AssetManager
import androidx.test.platform.app.InstrumentationRegistry
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlin.io.bufferedReader
import kotlin.io.readText
import junit.framework.TestCase.assertEquals
import java.io.IOException


/**
 * Base class for Android tests. Don't forget to annotate child class with `@RunWith(AndroidJUnit4::class)`.
 * It's recommend to create `Results` object for each subclass so it's easier to find expected
 * results.
 * @author Miroslav Hýbler <br>
 * created on 05.08.2024
 * @since 1.0.0
 */
abstract class BaseAndroidTest public constructor() {

    protected val context: Context
        get() = InstrumentationRegistry.getInstrumentation().context


    protected val assets: AssetManager
        get() = context.assets


    val iterator = HtmlIterator.instance


    /**
     * @param fileName File name in assets folder without extension
     * @throws IOException when file is not found in androidTest assets folder.
     */
    protected fun loadAsset(
        fileName: String,
    ): String {
        val validFileName = fileName
            .takeIf { name -> name.endsWith(suffix = ".html") }
            ?: "${fileName}.html"

        return assets.open(validFileName)
            .bufferedReader()
            .readText()
    }


    /**
     * Kotlin implementation of [junit.framework.TestCase.assertEquals] to allow arguments names.
     * @param actual
     * @param expected
     * @param message
     */
    protected inline fun assertEquals(
        actual: Int,
        expected: Int,
        message: () -> String = { "" },
    ) {
        return junit.framework.TestCase.assertEquals(
            message(), expected, actual
        )
    }

    /**
     * Kotlin implementation of [junit.framework.TestCase.assertEquals] to allow arguments names.
     * @param actual
     * @param expected
     * @param message
     */
    protected inline fun assertEquals(
        actual: Boolean,
        expected: Boolean,
        message: () -> String = { "" },
    ): Unit {
        return junit.framework.TestCase.assertEquals(
            message(), expected, actual
        )
    }
}