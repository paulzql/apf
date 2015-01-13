package org.paul.apf;

import org.paul.test.ITest;

import android.os.Bundle;
import android.app.Activity;
import android.content.res.Resources;
import android.util.Log;
import android.view.Menu;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		//PluginManager.getInstance(this).uninstall("com.example.org.paul.test.plugin");
		PluginManager.getInstance(this).install("/sdcard/org.paul.test.plugin.apk");
		ITest test = Interface.newInstance(ITest.class);
		test.SayHello();

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_main, menu);
		
		return true;
	}

}
