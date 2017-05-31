package com.cartotype.navigatorappdemo;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SubMenu;

/**
 * This is the main Activity screen for NavigatorApp, an Android demo of
 * CartoType off-line maps.
 * 
 * The map display and manipulation itself is implemented as a separated MapView
 * class.
 */
public class NavigatorAppActivity extends Activity
	{

	// For Android Menu button items (Note: do not start from 0, sub-menu
	// selection returns 0)
	private static final int MENU_TOGGLE_PERSPECTIVE = 1;
	private static final int MENU_ZOOM_IN = 2;
	private static final int MENU_ZOOM_OUT = 3;
	private static final int MENU_ROTATE = 4;
	private static final int MENU_TOGGLE_ROUTE_DISPLAY = 5;
	private static final int MENU_FIND = 6;
	private static final int MENU_FIND_ADDRESS = 16;
	private static final int MENU_TOGGLE_TERRAIN = 7;
	private static final int MENU_ROTATE_NORTH = 8;
	private static final int MENU_NEARBY_POIS = 9;
	private static final int MENU_MORE_MAPS = 10;
	private static final int MENU_ABOUT_SCREEN = 11;
	private static final int MENU_SET_ROUTE_START = 12;
	private static final int MENU_SET_ROUTE_END = 13;
	private static final int MENU_REVERSE_ROUTE = 14;
	private static final int MENU_CHANGE_MAP = 15;
	private static final int MENU_TOGGLE_TURNBYTURN_REAL = 17;
	private static final int MENU_TOGGLE_TURNBYTURN_SIMULATED = 18;
	private static final int MENU_SETTINGS_SCREEN = 19;
	private static final int MENU_WHEREAMI = 20;
	private static final int MENU_STOPWHEREAMI = 21;
	
	/**
	 * A string constant to use in calls to the "log" methods. Its value is
	 * often given by the name of the class, as this will allow you to easily
	 * determine where log methods are coming from when you analyze your logcat
	 * output.
	 */
	private static final String TAG = "NavigatorApp";

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);

		// Lock the orientation to portrait.
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		// Uncomment to Use the entire screen (for the map), no title bar
		// requestWindowFeature(Window.FEATURE_NO_TITLE);
		// Uncomment to go completely full-screen, no notification bar either:
		// getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
		// WindowManager.LayoutParams.FLAG_FULLSCREEN);

		// Get the screen resolution to pass to CartoType.
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);

		// The actual screen real-estate will be controlled by this map instance
		// Note: Make sure hardware acceleration is switched off.
		// Ref:
		// http://developer.android.com/guide/topics/graphics/hardware-accel.html
		iMapView = new MapView(this, metrics.densityDpi);
		setContentView(iMapView);

	    // Supply GPS fixes to the map view.
	    LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
	    locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,3000,10,iMapView);

		}

	@Override
	public boolean onCreateOptionsMenu(Menu aMenu)
		{

		return true;
		}

	@Override
	public boolean onPrepareOptionsMenu(Menu aMenu)
		{

		aMenu.clear();

		if (iMapView != null && iMapView.iIsGPSOn && ! iMapView.iNavigateStarted)
			{
			aMenu.add(0, MENU_STOPWHEREAMI, 0, "Stop GPS (save battery)");
			} 
		else
			{
			aMenu.add(0, MENU_WHEREAMI, 0, "Where am I?");
			}

		SubMenu sm = aMenu.addSubMenu("Map options ...");
		sm.add(0, MENU_TOGGLE_PERSPECTIVE, 0, "Toggle perspective");
		sm.add(0, MENU_TOGGLE_ROUTE_DISPLAY, 0, "Toggle route display");
		sm.add(0, MENU_TOGGLE_TERRAIN, 0, "Toggle terrain");
		sm.add(0, MENU_ROTATE, 0, "Rotate");
		sm.add(0, MENU_ROTATE_NORTH, 0, "North at top");
		sm.add(0, MENU_ZOOM_IN, 0, "Zoom in");
		sm.add(0, MENU_ZOOM_OUT, 0, "Zoom out");
		SubMenu sm2 = aMenu.addSubMenu("Route display ...");
		sm2.add(0, MENU_SET_ROUTE_START, 0, "Set route start");
		sm2.add(0, MENU_SET_ROUTE_END, 0, "Set route end");
		sm2.add(0, MENU_REVERSE_ROUTE, 0, "Reverse route");
		sm2.add(0, MENU_TOGGLE_ROUTE_DISPLAY, 0, "Toggle route display");
		sm2.add(0, MENU_TOGGLE_TURNBYTURN_REAL, 0, "Toggle real navigation");
		sm2.add(0, MENU_TOGGLE_TURNBYTURN_SIMULATED, 0, "Toggle simulated navigation");
		aMenu.add(0, MENU_NEARBY_POIS, 0, "Show nearby POIs");
		aMenu.add(0, MENU_FIND, 0, "Find ...");
		aMenu.add(0, MENU_FIND_ADDRESS, 0, "Find address ...");
		sm.add(0, MENU_MORE_MAPS, 0, "Get more maps ...");
		sm.add(0, MENU_CHANGE_MAP, 0, "Change map ...");
		aMenu.add(0, MENU_SETTINGS_SCREEN, 0, "Change distance units ...");
		aMenu.add(0, MENU_ABOUT_SCREEN, 0, "About ...");
		return true;
		}

	@Override
	public boolean onOptionsItemSelected(MenuItem aItem)
		{
		switch (aItem.getItemId())
			{
			case MENU_TOGGLE_PERSPECTIVE:
				iMapView.togglePerspective();
				return true;
			case MENU_ZOOM_IN:
				iMapView.zoomIn();
				return true;
			case MENU_ZOOM_OUT:
				iMapView.zoomOut();
				return true;
			case MENU_ROTATE:
				iMapView.rotate();
				return true;
			case MENU_ROTATE_NORTH:
				iMapView.setRotation(0.0);
				return true;
			case MENU_TOGGLE_TERRAIN:
				iMapView.toggleTerrain();
				return true;
			case MENU_TOGGLE_ROUTE_DISPLAY:
				iMapView.toggleDisplayRoute();
				return true;
			case MENU_REVERSE_ROUTE:
				iMapView.reverseRoute();
				return true;
			case MENU_NEARBY_POIS:
				// Shows streets and POIs nearest the current selected point
				iMapView.showNearbyPOIs();
				return true;
			case MENU_FIND:
				// Show a dialog to find and display POIs nearest the current
				// selected point
				iMapView.showFind();
				return true;
			case MENU_FIND_ADDRESS:
				// Show a dialog to find and display feature(s) matching a
				// specific address
				iMapView.showFindAddress();
				return true;
			case MENU_SET_ROUTE_START:
				// (Re)Sets the current selected point as navigation route
				// start
				iMapView.setRouteStart();
				return true;
			case MENU_SET_ROUTE_END:
				// (Re)Sets the current selected point as navigation route
				// end
				iMapView.setRouteEnd();
				return true;
			case MENU_MORE_MAPS:
				// Start a new Activity screen
				Intent myIntent1 = new Intent(NavigatorAppActivity.this,
						MapDownloadActivity.class);
				// No result expected, just start
				NavigatorAppActivity.this.startActivity(myIntent1);
				return true;
			case MENU_CHANGE_MAP:
				// Start a new Activity screen
				Intent myIntent3 = new Intent(NavigatorAppActivity.this,
						MapChangeActivity.class);
				// We expect a result to be returned (to onActivityResult() )
				NavigatorAppActivity.this.startActivityForResult(myIntent3,
						MENU_CHANGE_MAP);
				return true;
			case MENU_SETTINGS_SCREEN:
				// Start a new Activity screen
				Intent myIntent4 = new Intent(NavigatorAppActivity.this,
						SettingsActivity.class);
				myIntent4.putExtra("units", iMapView.iDistanceUnits);
				// We expect a result to be returned (to onActivityResult() )
				NavigatorAppActivity.this.startActivityForResult(myIntent4,
						MENU_SETTINGS_SCREEN);
				return true;
			case MENU_ABOUT_SCREEN:
				Intent myIntent2 = new Intent(NavigatorAppActivity.this,
						AboutActivity.class);
				NavigatorAppActivity.this.startActivity(myIntent2);
				return true;
			case MENU_TOGGLE_TURNBYTURN_SIMULATED:
				iMapView.toggleSimulateNavigation();
				return true;
			case MENU_TOGGLE_TURNBYTURN_REAL:
				iMapView.toggleNavigation();
				return true;
			case MENU_WHEREAMI:
				iMapView.whereAmI();
				return true;
			case MENU_STOPWHEREAMI:
				iMapView.toggleGPS(false);
				return true;
			}
		
		return false;
		}

	/**
	 * For changing map. Menu item MENU_CHANGE_MAP returns here
	 */
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data)
		{
		super.onActivityResult(requestCode, resultCode, data);

		// See which child activity is calling us back.
		switch (requestCode)
			{
			case (MENU_CHANGE_MAP):
				{
				Log.d(TAG, "Map change requested");
				if (resultCode == Activity.RESULT_OK)
					{
					// Extract the data returned from the child Activity.
					String new_map = data.getStringExtra("new_map");
					// Destroy current MapView map and start again
					Log.d(TAG, "Doing Map change");
					iMapView.setMapName(new_map);
					// Just so we don't try to route from the old map to the new
					// map
					iMapView.deleteRouteStart();
					iMapView.deleteRouteEnd();
					}
				break;
				}
			case (MENU_SETTINGS_SCREEN):
				{
				Log.d(TAG, "Units change requested");
				if (resultCode == Activity.RESULT_OK)
					{ 
					// Extract the data returned from the child Activity.
					iMapView.iDistanceUnits = data.getStringExtra("units");
					}
				break;	
				}
			}
		}

	/**
	 * This method is called automatically by the Android OS when the app is
	 * suspended. It is ALWAYS triggered by (at least): User clicks Android Home
	 * or Menu button, screen orientation changes, Android removes the
	 * application for memory reasons. Other onXXXX methods are NOT reliably
	 * called in the above instances, so it is good to save state information
	 * here.
	 */
	@Override
	protected void onPause()
		{
		super.onPause();
		Log.v(TAG, "onPause()");

		// Save current map state
		iMapView.saveState();

		}

	@Override
	protected void onResume()
		{
		super.onResume();
		Log.v(TAG, "onResume()");

		}

	private MapView iMapView;

	// For saving state
	public String iPrefsName = "NavigatorAppPreferences";
	}
