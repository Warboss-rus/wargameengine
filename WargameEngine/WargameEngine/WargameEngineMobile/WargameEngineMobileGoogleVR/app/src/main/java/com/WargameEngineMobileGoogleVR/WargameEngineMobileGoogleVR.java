package com.WargameEngineMobileGoogleVR;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import com.google.vr.ndk.base.AndroidCompat;
import com.google.vr.ndk.base.GvrLayout;
import android.os.Vibrator;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.InputDevice;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class WargameEngineMobileGoogleVR extends Activity
{
    static {
    System.loadLibrary("gvr");
    System.loadLibrary("gvr_audio");
    System.loadLibrary("WargameEngineMobile");
  }

  // Opaque native pointer to the native app instance.
  private long nativeApp;

  private GvrLayout gvrLayout;
  private GLSurfaceView surfaceView;

  // Note that pause and resume signals to the native renderer are performed on the GL thread,
  // ensuring thread-safety.
  private final Runnable pauseNativeRunnable =
      new Runnable() {
        @Override
        public void run() {
          nativeOnPause(nativeApp);
        }
      };

  private final Runnable resumeNativeRunnable =
      new Runnable() {
        @Override
        public void run() {
          nativeOnResume(nativeApp);
        }
      };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // Ensure fullscreen immersion.
    setImmersiveSticky();
    getWindow()
        .getDecorView()
        .setOnSystemUiVisibilityChangeListener(
            new View.OnSystemUiVisibilityChangeListener() {
              @Override
              public void onSystemUiVisibilityChange(int visibility) {
                if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
                  setImmersiveSticky();
                }
              }
            });

    // Initialize GvrLayout and the native renderer.
    gvrLayout = new GvrLayout(this);

    // Add the GLSurfaceView to the GvrLayout.
    surfaceView = new GLSurfaceView(this);
    surfaceView.setEGLContextClientVersion(3);
    surfaceView.setEGLConfigChooser(8, 8, 8, 0, 0, 0);
    surfaceView.setPreserveEGLContextOnPause(true);
    surfaceView.setRenderer(
        new GLSurfaceView.Renderer() {
          @Override
          public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            nativeInitializeGl(nativeApp);
          }

          @Override
          public void onSurfaceChanged(GL10 gl, int width, int height) {}

          @Override
          public void onDrawFrame(GL10 gl) {
            nativeDrawFrame(nativeApp);
          }
        });
    surfaceView.setOnTouchListener(
        new View.OnTouchListener() {
          @Override
          public boolean onTouch(View v, final MotionEvent event) {
			surfaceView.queueEvent(
				new Runnable() {
                    @Override
                    public void run() {
                      nativeOnTouchEvent(nativeApp, event.getAction(), event.getX(), event.getY());
                    }
                });
            return true;
          }
        });
	surfaceView.setOnGenericMotionListener(
		new View.OnGenericMotionListener() {
			@Override
			public boolean onGenericMotion(View v, final MotionEvent event)	{
				if (((event.getSource() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)) {
					//gamepad axis
				}
				return true;
			}
		});
    gvrLayout.setPresentationView(surfaceView);

    // Add the GvrLayout to the View hierarchy.
    setContentView(gvrLayout);

    // Enable scan line racing.
    if (gvrLayout.setAsyncReprojectionEnabled(true)) {
      // Scanline racing decouples the app framerate from the display framerate,
      // allowing immersive interaction even at the throttled clockrates set by
      // sustained performance mode.
      AndroidCompat.setSustainedPerformanceMode(this, true);
    }

    // Enable VR Mode.
    AndroidCompat.setVrModeEnabled(this, true);

	nativeApp =
        nativeCreateRenderer(
            getClass().getClassLoader(),
            this.getApplicationContext(),
            gvrLayout.getGvrApi().getNativeGvrContext());
  }

  @Override
  protected void onPause() {
    surfaceView.queueEvent(pauseNativeRunnable);
    surfaceView.onPause();
    gvrLayout.onPause();
    super.onPause();
  }

  @Override
  protected void onResume() {
    super.onResume();
    gvrLayout.onResume();
    surfaceView.onResume();
    surfaceView.queueEvent(resumeNativeRunnable);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    // Destruction order is important; shutting down the GvrLayout will detach
    // the GLSurfaceView and stop the GL thread, allowing safe shutdown of
    // native resources from the UI thread.
    gvrLayout.shutdown();
	nativeDestroyRenderer(nativeApp);
    nativeApp = 0;
  }

  @Override
  public void onWindowFocusChanged(boolean hasFocus) {
    super.onWindowFocusChanged(hasFocus);
    if (hasFocus) {
      setImmersiveSticky();
    }
  }

  @Override
  public boolean dispatchKeyEvent(KeyEvent event) {
    // Avoid accidental volume key presses while the phone is in the VR headset.
    if (event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_UP
        || event.getKeyCode() == KeyEvent.KEYCODE_VOLUME_DOWN) {
      return true;
    }
	if (((event.getSource() & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD) && (event.getRepeatCount() == 0)) {
		
	}
    return super.dispatchKeyEvent(event);
  }

  private void setImmersiveSticky() {
    getWindow()
        .getDecorView()
        .setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
  }

  private native long nativeCreateRenderer(
      ClassLoader appClassLoader, Context context, long nativeGvrContext);
  private native void nativeDestroyRenderer(long nativeApp);
  private native void nativeInitializeGl(long nativeApp);
  private native long nativeDrawFrame(long nativeApp);
  private native void nativeOnTouchEvent(long nativeApp, int action, float x, float y);
  private native void nativeOnPause(long nativeApp);
  private native void nativeOnResume(long nativeApp);
  private native void nativeOnGamepadButtonEvent(long nativeApp, int action, int button);
}