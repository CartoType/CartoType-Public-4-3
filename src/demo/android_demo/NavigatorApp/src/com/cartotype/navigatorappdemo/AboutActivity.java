package com.cartotype.navigatorappdemo;

import android.os.Build;
import android.os.Bundle;
import android.annotation.TargetApi;
import android.app.Activity;
import android.view.Menu;
import android.view.MenuItem;
import android.support.v4.app.NavUtils;

/**
For displaying a screen describing the parent application and showing credits
and license detail.
*/
public class AboutActivity extends Activity
	{

	/**
    Display activity_about screen
	*/
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@Override
	public void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_about);
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB)
			{
			// Not available in Android 2.2, added in API 11
			getActionBar().setDisplayHomeAsUpEnabled(true);
			}

		}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
		{
		getMenuInflater().inflate(R.menu.activity_about, menu);
		return true;
		}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
		{
		switch (item.getItemId())
			{
			case android.R.id.home:
				NavUtils.navigateUpFromSameTask(this);
				return true;
			}
		return super.onOptionsItemSelected(item);
		}

	}
