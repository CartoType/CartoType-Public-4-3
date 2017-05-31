/*
NavigatorActivity.java
Copyright (C) 2012 Cartography Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype.navigator;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuItem;

public class NavigatorActivity extends Activity
{
private static final int MENU_PERSPECTIVE = 0; 
private static final int MENU_ZOOM_IN = 1; 
private static final int MENU_ZOOM_OUT = 2;
private static final int MENU_ROTATE = 3;
private static final int MENU_NAVIGATE = 4;
private static final int MENU_SIMULATE_NAVIGATION = 5;
private static final int MENU_TOGGLE_TERRAIN = 6;

/** Called when the activity is first created. */
@Override
public void onCreate(Bundle savedInstanceState)
	{
    super.onCreate(savedInstanceState);
    
    // Lock the orientation to portrait.
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

    // Get the screen resolution to pass to CartoType.
    DisplayMetrics metrics = new DisplayMetrics();
    getWindowManager().getDefaultDisplay().getMetrics(metrics);
    
    iMapView = new MapView(this,metrics.densityDpi);
    setContentView(iMapView);
    
    // Supply GPS fixes to the map view.
    LocationManager locationManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
    locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,0,0,iMapView);
	}

@Override
public boolean onCreateOptionsMenu(Menu aMenu)
    {
    aMenu.add(0,MENU_PERSPECTIVE,0,"Perspective");
    aMenu.add(0,MENU_ZOOM_IN,0,"Zoom In");
    aMenu.add(0,MENU_ZOOM_OUT,0,"Zoom Out");
    aMenu.add(0,MENU_ROTATE,0,"Rotate");
    aMenu.add(0,MENU_NAVIGATE,0,"Navigate");
    aMenu.add(0,MENU_SIMULATE_NAVIGATION,0,"Simulate Navigation");
    aMenu.add(0,MENU_TOGGLE_TERRAIN,0,"Toggle Terrain");
    return true;
    }

@Override
public boolean onOptionsItemSelected(MenuItem aItem)
	{
	switch (aItem.getItemId())
    	{
    	case MENU_PERSPECTIVE:
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
    	case MENU_NAVIGATE:
    		iMapView.toggleNavigation();
    		return true;
        case MENU_SIMULATE_NAVIGATION:
            iMapView.toggleSimulateNavigation();
            return true;
        case MENU_TOGGLE_TERRAIN:
            iMapView.toggleTerrain();
        return true;
    	}
	return false;
	}

private MapView iMapView;
}
