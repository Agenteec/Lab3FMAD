package com.game.lab3fmad

import android.app.NativeActivity
import android.os.Bundle
import android.view.View
import android.view.WindowManager
import android.graphics.Color
import android.os.Build

class MainActivity : NativeActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setupFullscreenMode()
    }

    override fun onResume() {
        super.onResume()
        setupFullscreenMode()
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            setupFullscreenMode()
        }
    }

    private fun setupFullscreenMode() {
        try {
            val decorView = window.decorView

            var uiOptions = (View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    or View.SYSTEM_UI_FLAG_FULLSCREEN)

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                uiOptions = uiOptions or View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            }

            decorView.systemUiVisibility = uiOptions

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                window.navigationBarColor = Color.TRANSPARENT
                window.statusBarColor = Color.TRANSPARENT

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    decorView.systemUiVisibility = uiOptions or
                            View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
                }
            }

            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    companion object {
        init {
            System.loadLibrary("main")
        }
    }
}