package com.cartotype.navigatorappdemo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
 
/**
 * A simple screen to change the default display distance units.
 * Can be extended to support more user settings.
 * 
 * @author Michael Collinson 2013-03-25
 *
 */
public class SettingsActivity extends Activity {
 
  private RadioGroup radioUnitsGroup;
  //private RadioButton radioUnitsButton;
  private Button btnDisplay;
  
  //For logging
  private static final String TAG = "Settings";
 
  @Override
  public void onCreate(Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	setContentView(R.layout.settings);
	
	// Set units radio button to current value
	if (getIntent().getExtras().getString("units").equals("imperial")) {
		RadioButton rb1 = (RadioButton) findViewById(R.id.radioUnitsImperial);
		rb1.setChecked(true);
	}
 
	addListenerOnButton();
 
  }
 
  public void addListenerOnButton() {
 
	radioUnitsGroup = (RadioGroup) findViewById(R.id.radioUnits);
	btnDisplay = (Button) findViewById(R.id.settings_btnDisplay);
 
	btnDisplay.setOnClickListener(new OnClickListener() {
 
		@Override
		public void onClick(View v) {
		
			String units = "metric";
			
			// Check which radio button was clicked
		    switch(radioUnitsGroup.getCheckedRadioButtonId()) {
		        case R.id.radioUnitsMetric:
		            break;
		        case R.id.radioUnitsImperial:
		        	units = "imperial";
		            break;
		        default:
		        	break;
		    }
		    
		    Log.d(TAG,"User wants to change display units");
			
			// Use Android Intent to send data (the new map) back to parent
			Intent resultIntent = new Intent();
			resultIntent.putExtra("units", units);
			setResult(Activity.RESULT_OK, resultIntent);
			
			// And quit this screen
		    finish();
 
		}
 
	});
 
  }
}