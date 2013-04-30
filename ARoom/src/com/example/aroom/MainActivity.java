package com.example.aroom;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class MainActivity extends Activity {
	
	public EditText ip_field;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		ip_field = (EditText)findViewById(R.id.ip_input);
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void startEgo(View view){
		Intent intent = new Intent(this, EgoCentricActivity.class);
		
		String ip_address = ip_field.getText().toString();
		
		Bundle bundle = new Bundle();
		
		bundle.putString("ip", ip_address);
		intent.putExtras(bundle);
		startActivity(intent);
	}
	
	public void startExo(View view){
		Intent intent = new Intent(this, ExoCentricActivity.class);
		
		String ip_address = ip_field.getText().toString();
		
		Bundle bundle = new Bundle();
		
		bundle.putString("ip", ip_address);
		intent.putExtras(bundle);
		startActivity(intent);
	}

}
