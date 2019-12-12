package androidmk.ndk.ne.sty.com.nendkandroidmk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    {
        System.loadLibrary("hello-jni");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView textView = findViewById(R.id.tv_text);
        textView.setText("nativeTest: " + nativeTest() + "-->" + stringFromJNI());
    }

    public native int nativeTest();

    public native String stringFromJNI();
}
