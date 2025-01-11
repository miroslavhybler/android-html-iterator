package com.htmliterator.example

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.lifecycleScope
import com.htmliterator.HtmlIterator
import com.htmliterator.TagInfo
import com.htmliterator.example.ui.theme.HtmlIteratorTheme
import kotlinx.coroutines.launch

/**
 * @author Miroslav Hýbler <br>
 * created on 22.11.2024
 */
class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        val callback = object : HtmlIterator.Callback() {

            override fun onSingleTag(tag: TagInfo) {
                super.onSingleTag(tag)
            }

            override fun onPairTag(
                tag: TagInfo,
                openingTagStartIndex: Int,
                openingTagEndIndex: Int,
                closingTagStartIndex: Int,
                closingTagEndIndex: Int
            ): Boolean {
                super.onPairTag(
                    tag = tag,
                    openingTagStartIndex = openingTagStartIndex,
                    openingTagEndIndex = openingTagEndIndex,
                    closingTagStartIndex = closingTagStartIndex,
                    closingTagEndIndex = closingTagEndIndex,
                )
                return true
            }

            override fun onLeavingPairTag(tag: TagInfo) {
                super.onLeavingPairTag(tag = tag)
            }

            override fun onContentText(text: String) {
                super.onContentText(text)
            }
        }


        lifecycleScope.launch {
            HtmlIterator.instance.let {
                it.setCallback(callback)
                it.setContent(
                    content = loadAsset(fileName = "test.html")
                )
                it.iterate()
            }
        }




        setContent {
            HtmlIteratorTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->

                    Box(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(innerPadding)
                    ) {

                        Box(
                            modifier = Modifier
                                .size(size = 50.dp)
                                .align(alignment = Alignment.Center)
                                .background(color = MaterialTheme.colorScheme.primary)
                        )
                    }
                }
            }
        }
    }

    protected fun loadAsset(fileName: String): String {
        val validFileName = fileName
            .takeIf { name -> name.endsWith(suffix = ".html") }
            ?: "${fileName}.html"

        return assets.open(validFileName)
            .bufferedReader()
            .readText()
    }
}