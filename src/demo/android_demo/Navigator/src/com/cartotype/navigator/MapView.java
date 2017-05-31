/*
MapView.java
Copyright (C) 2012 Cartography Ltd.
See www.cartotype.com for more information.
*/

package com.cartotype.navigator;

import com.cartotype.*;
import com.cartotype.Address;
import com.cartotype.Error;

import android.content.Context;
import android.graphics.*;
import android.location.*;
import android.os.Bundle;
import android.text.format.Time;
import android.view.*;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View.OnTouchListener;
import android.widget.Toast;

class MapView extends View implements OnTouchListener, LocationListener, OnScaleGestureListener
	{
    private static final int MESSAGE_TEXT_SIZE = 30;
    private static final int MESSAGE_RECT_HEIGHT = MESSAGE_TEXT_SIZE * 3;
    private static final int MAP_TOP = MESSAGE_RECT_HEIGHT;
	
	public MapView(Context aContext,int aScreenDpi)
		{
		super(aContext);
		setOnTouchListener(this);
		iScreenDpi = aScreenDpi;
		}

    public void init()
        {
        setKeepScreenOn(true);
        iScaleGestureDetector = new ScaleGestureDetector(getContext(),this);
        
        int w = getWidth();
        int h = getHeight();
        
        iTextPaint.setTextSize(MESSAGE_TEXT_SIZE);
        iTextPaint.setAntiAlias(true);
        iMessageBackgroundPaint = new Paint();
        iMessageBackgroundPaint.setColor(0xFFFFFFFF); // white
        iMessageRect = new RectF(0,0,w,MESSAGE_RECT_HEIGHT);
        
        int bitmap_height = h - MESSAGE_RECT_HEIGHT;
        iFramework = new Framework("/sdcard/CartoType/map/santa-cruz.ctm1",
                                   "/sdcard/CartoType/style/osm-style.xml",
                                   "/sdcard/CartoType/font/DejaVuSans.ttf",
                                   100,100);
        iFramework.setResolutionDpi(iScreenDpi);
        iFramework.zoomIn();
        iFramework.resize(w,bitmap_height);
        iFramework.setNavigatorMinimumFixDistance(4);
        iFramework.setNavigatorTimeTolerance(15);
                        
        iFramework.enableLayer("terrain-height-feet",iDisplayTerrain);
        iFramework.enableLayer("terrain-shadow",iDisplayTerrain);
                
        getMap();
        }
	
	public void togglePerspective()
		{
		iPerspective = !iPerspective;
		iFramework.setPerspective(iPerspective);
		getMap();
		invalidate();
		}

    public void toggleTerrain()
        {
        iDisplayTerrain = !iDisplayTerrain;
        iFramework.enableLayer("terrain-height-feet",iDisplayTerrain);
        iFramework.enableLayer("terrain-shadow",iDisplayTerrain);
        getMap();
        invalidate();
        }
	
	public void zoomIn()
		{
		iFramework.zoomIn();
		getMap();
		invalidate();
		}

	public void zoomOut()
		{
		iFramework.zoomOut();
		getMap();
		invalidate();
		}

	public void rotate()
		{
		iFramework.rotate(90);
		getMap();
		invalidate();
		}

	public void toggleNavigation()
		{
		if (iNavigating)
		    {
		    iFramework.endNavigation();
		    iNavigating = false;
		    iNavigationMessage1 = "NOT NAVIGATING";
		    iNavigationMessage2 = null;
		    iNavigationMessage3 = null;
		    }
		else
		    {
		    if (iCurLong == 0 || iCurLat == 0)
		        {
		        Notify("Cannot start navigation: current position not known.");
		        return;
		        }
		    if (iRouteEndLong == 0 || iRouteEndLat == 0)
		        {
                Notify("Cannot start navigation: destination not known. Please set the destination by touching the map and try again.");
                return;
		        }
		               
            Notify("starting navigation from " + iCurLong + "," + iCurLat + " to " + iRouteEndLong + "," + iRouteEndLat);
            
		    int result = iFramework.startNavigation(iCurLong,iCurLat,Framework.DEGREE_COORDS,
		                                            iRouteEndLong,iRouteEndLat,Framework.DEGREE_COORDS);
		    		    
		    if (result != Error.NONE)
		        Notify("Could not start navigation: error = " + result + ".");
		    else
		        {
		        iSimulatingNavigation = false;
                iNavigating = true;
                iNavigationMessage1 = "STARTING NAVIGATION...";
                iNavigationMessage2 = null;
                iNavigationMessage3 = null;
		        }
		    }

		getMap();
        invalidate();
		}

	/**
	When simulating navigation, a route is calculated between the last two points touched.
	Further touches provide a simulated vehicle position.
	*/
	public void toggleSimulateNavigation()
    	{
    	if (iSimulatingNavigation)
    	    {
            iFramework.endNavigation();
            iSimulatingNavigation = false;
            iNavigationMessage1 = "NOT NAVIGATING";
            iNavigationMessage2 = null;
            iNavigationMessage3 = null;
    	    }
    	else
    	    {
            if (iRouteStartLong == 0 || iRouteStartLat == 0 ||
                iRouteEndLong == 0 || iRouteEndLat == 0)
                {
                Notify("Cannot start simulating navigation: positions not known. Please touch two positions and try again");
                return;
                }
            Notify("starting simulated navigation from " + iRouteStartLong + "," + iRouteStartLat + " to " + iRouteEndLong + "," + iRouteEndLat);
            
            int result = iFramework.startNavigation(iRouteStartLong,iRouteStartLat,Framework.DEGREE_COORDS,
                                                    iRouteEndLong,iRouteEndLat,Framework.DEGREE_COORDS);

            if (result != 0)
                Notify("Could not start simulated navigation: error = " + result + ".");
            else
                {
                iSimulatingNavigation = true;
                iNavigating = false;
                iNavigationMessage1 = "STARTING NAVIGATION...";
                iNavigationMessage2 = null;
                iNavigationMessage3 = null;
                }
    	    }

        getMap();
        invalidate();
    	}
		
	protected void onDraw(Canvas aCanvas)
		{
		if (iBitmap == null)
		    {
			init();
			iPlainMatrix = aCanvas.getMatrix();
			
			/*
			Adjust the matrix to take account of the raw screen position:
			see http://code.google.com/p/android/issues/detail?id=24517
			Canvas getMatrix and setMatrix don't correspond when there is graphics acceleration.
			*/
			if (aCanvas.isHardwareAccelerated())
			    {
			    int[] x = new int[2];
			    getLocationOnScreen(x);
			    iPlainMatrix.preTranslate(x[0],x[1]);
			    }
		    }
		
		if (iScale != 1.0 || iXOffset != 0 || iYOffset != 0)
		    {
	        Matrix m = aCanvas.getMatrix();
	        m.set(iPlainMatrix);
            m.preTranslate(getWidth() / 2 + iXOffset,getHeight() / 2 + iYOffset);
		    m.preScale(iScale,iScale);
		    m.preTranslate(-getWidth() / 2,-getHeight() / 2);
		    aCanvas.setMatrix(m);
		    }
		else
		    aCanvas.setMatrix(iPlainMatrix);
		
		if (iBitmap != null)
			aCanvas.drawBitmap(iBitmap,0,MAP_TOP,null);

        aCanvas.drawRect(iMessageRect,iMessageBackgroundPaint);
        
        int x = 4;
        int y = MESSAGE_TEXT_SIZE - 4;
        aCanvas.drawText(iNavigationMessage1,x,y,iTextPaint);
        if (iNavigationMessage2 != null)
            {
            y += MESSAGE_TEXT_SIZE;
            aCanvas.drawText(iNavigationMessage2,x,y,iTextPaint);
            }
        if (iNavigationMessage3 != null)
            {
            y += MESSAGE_TEXT_SIZE;
            aCanvas.drawText(iNavigationMessage3,x,y,iTextPaint);
            }
		}
	
	private void getMap()
		{
		iBitmap = iFramework.getMap();
		}

    public boolean onTouch(View aView,MotionEvent aEvent)
    	{
    	// Handle scale gestures.
    	iScaleGestureDetector.onTouchEvent(aEvent);
    	if (iScaleGestureDetector.isInProgress())
    	    return true;
    	
    	switch (aEvent.getAction())
	    	{
	    	case MotionEvent.ACTION_DOWN:
	    	    if (iTouchCount == 0)
	    	        {
    		    	iTouchCount = 1;
    		    	iStartTouchPointX = aEvent.getX();
    		    	iStartTouchPointY = aEvent.getY();
    		    	return true;
	    	        }
	    	case MotionEvent.ACTION_MOVE:
		    	if (iTouchCount == 1)
		    		{
		    		iXOffset = aEvent.getX() - iStartTouchPointX;
		    		iYOffset = aEvent.getY() - iStartTouchPointY;
		    		invalidate();
		    		}
	    		return true;
	    	case MotionEvent.ACTION_UP:
	    	    if (iTouchCount != 1)
	    	        break;

	    	    iXOffset = aEvent.getX() - iStartTouchPointX;
                iYOffset = aEvent.getY() - iStartTouchPointY;
		    	
                double x[] = new double[2];
                x[0] = aEvent.getX();
                x[1] = aEvent.getY() - MAP_TOP;
                iFramework.convertCoords(x,Framework.SCREEN_COORDS,Framework.DEGREE_COORDS);
		    	
		    	/*
		    	If the movement was very small, drop a pushpin at this point to mark the end of a route,
		    	or if simulating navigation use the point as the new vehicle location.
		    	*/
		    	if (iXOffset > -3 && iXOffset < 3 &&
		    	    iYOffset > -3 && iYOffset < 3)
		    	    {
                    if (!iNavigating && !iSimulatingNavigation)
                        {
                        iRouteStartLong = iRouteEndLong;
                        iRouteStartLat = iRouteEndLat;
                        iRouteEndLong = x[0];
                        iRouteEndLat = x[1];
                        
    		    	    iFramework.insertPointMapObject(0,"route-position",iRouteEndLong,iRouteEndLat,Framework.DEGREE_COORDS,"",0,iRoutePosId,iRoutePosId != 0);
    		    	    iRoutePosId = iFramework.getLastObjectId();

                        // Find the object at the point.
                        MapObject map_object[] = iFramework.findInDisplay(aEvent.getX(),aEvent.getY() - MAP_TOP,8,10);
                        String map_object_name = null;
                        for (int i = 0; i < map_object.length; i++)
                            {
                            if (map_object[i].getLabel() != null && map_object[i].getLabel().length() != 0)
                                {
                                map_object_name = map_object[i].getLabel();
                                break;
                                }
                            }
                        if (map_object_name != null)
                            Notify("Destination = " + map_object_name + " " + iRouteEndLong + "E, " + iRouteEndLat + "N.");
                        else
                            Notify("Destination = " + iRouteEndLong + "E, " + iRouteEndLat + "N.");
                        
		    	        }
                    
                    if (iSimulatingNavigation)
                        {
                        Time t = new Time();
                        double seconds = (double)t.toMillis(true) / 1000.0;
                        Navigate(Framework.POSITION_VALID | Framework.TIME_VALID,seconds,x[0],x[1],0,0,0);
                        }
		    	    }
               
                iTouchCount = 0;
                iFramework.pan((int)Math.floor(-iXOffset + 0.5),(int)Math.floor(-iYOffset + 0.5));
                iXOffset = 0;
                iYOffset = 0;
                getMap();
                invalidate();
		    	
	    		return true;
	    	}
    	
    	return false;
    	}

    /*
    Update the navigation state using location data.
    The native interface code then adjusts the map position, orientation and scale.
    */
    public void Navigate(int aValidity,double aTime,double aLong,double aLat,double aSpeed,double aBearing,double aHeight)
        {
        iFramework.navigate(aValidity,aTime,aLong,aLat,aSpeed,aBearing,aHeight);

        iNavigationState = iFramework.getNavigationState();
        
        iNavigationMessage2 = null;

        double distance = iFramework.distanceToDestination();
        distance = (int)distance / 100;
        distance /= 10;
        iNavigationMessage3 = "bearing = " + (int)(iCurBearing + 0.5) + "; distance left = " + distance + "km;";
        double seconds = iFramework.estimatedTimeToDestination();
        double hours = (int)(seconds / 3600);
        seconds -= hours * 3600;
        double minutes = (int)(seconds / 60);
        seconds -= minutes * 60;
        iNavigationMessage3 += " time left = " + (int)hours + "h " + (int)minutes + "m " + (int)seconds + "s.";
        
        iFirstTurnDistance = iFramework.getFirstTurn(iFirstTurn);
        iSecondTurnDistance = iFramework.getSecondTurn(iSecondTurn);
        
        switch (iNavigationState)
            {
            case NavigationState.NO_ACTION:
                break;
            
            case NavigationState.TURN:
                iNavigationMessage1 = iFirstTurn.iInstructions;
                
                if (!iSecondTurn.iContinue)
                    iNavigationMessage2 = iSecondTurn.iInstructions;
                break;
            
            case NavigationState.TURN_ROUND:
                iNavigationMessage1 = "turn round at the next safe and legal opportunity";
                break;
            
            case NavigationState.NEW_ROUTE:
                iNavigationMessage1 = "calculating a new route";
                break;
            
            case NavigationState.ARRIVAL:
                iFirstTurnDistance = iFramework.getFirstTurn(iFirstTurn);
                iNavigationMessage1 = "arriving after " + (int)iFirstTurnDistance + "m";;
                break;
            
            case NavigationState.OFF_ROUTE:
                iNavigationMessage1 = "off route";
                break;
            }

        getMap();
        invalidate();
        }
    
    public void onLocationChanged(Location aLocation)
		{
		boolean location_was_unknown = iCurLong == 0;
		
		iCurLong = aLocation.getLongitude();
		iCurLat = aLocation.getLatitude();
		iCurBearing = aLocation.getBearing();

		if (location_was_unknown)
		    Notify("location now known");
		
		if (iNavigating)
		    {
		    // Get the validity flags; position and time are always valid if we get a fix.
		    int validity = Framework.POSITION_VALID | Framework.TIME_VALID;
		    if (aLocation.hasSpeed())
		        validity |= Framework.SPEED_VALID;
		    if (aLocation.hasBearing())
		        validity |= Framework.COURSE_VALID;
		    if (aLocation.hasAltitude())
		        validity |= Framework.HEIGHT_VALID;
		    
		    double time = aLocation.getTime();
		    Navigate(validity,
		             time / 1000.0,    // convert time to seconds
		             iCurLong,iCurLat,
		             aLocation.getSpeed() * 3.6,      // convert speed from metres per second to kilometres per hour
		             aLocation.getBearing(),
		             aLocation.getAltitude());
		    }
		}

	public void onProviderDisabled(String arg0)
		{
		// TODO Auto-generated method stub
		
		}

	public void onProviderEnabled(String arg0)
		{
		// TODO Auto-generated method stub
		
		}

	public void onStatusChanged(String arg0, int arg1, Bundle arg2)
		{
		// TODO Auto-generated method stub
		
		}

    @Override
    public boolean onScale(ScaleGestureDetector detector)
        {
        iScale *= iScaleGestureDetector.getScaleFactor();
        float x = iScaleGestureDetector.getFocusX();
        float y = iScaleGestureDetector.getFocusY();
        iXOffset += x - iPrevScaleFocusX;
        iYOffset += y - iPrevScaleFocusY;
        iPrevScaleFocusX = x;
        iPrevScaleFocusY = y;
        invalidate();
        return true;
        }

    @Override
    public boolean onScaleBegin(ScaleGestureDetector detector)
        {
        iPrevScaleFocusX = iScaleGestureDetector.getFocusX();
        iPrevScaleFocusY = iScaleGestureDetector.getFocusY();
        iTouchCount = 2;
        return true;
        }

    @Override
    public void onScaleEnd(ScaleGestureDetector detector)
        {
        if (iScale != 1)
            iFramework.zoom(iScale);
        iScale = 1;

        if (iXOffset != 0 || iYOffset != 0)
            iFramework.pan((int)Math.floor(-iXOffset + 0.5),(int)Math.floor(-iYOffset + 0.5));
        iXOffset = 0;
        iYOffset = 0;

        iTouchCount = 0;
        
        getMap();
        invalidate();
        }
    
	private void Notify(String aMessage)
	    {
	    Toast.makeText(getContext(),aMessage,Toast.LENGTH_SHORT).show();
	    }
	
	private Framework iFramework;
	private Bitmap iBitmap;
    private boolean iPerspective;
    private boolean iDisplayTerrain;
	private int iTouchCount;
    private float iStartTouchPointX;
    private float iStartTouchPointY;
    private float iPrevScaleFocusX;
    private float iPrevScaleFocusY;
	private float iXOffset;
	private float iYOffset;
    private float iScale = 1;
    private Matrix iPlainMatrix;
	private Paint iTextPaint = new Paint();
	private RectF iMessageRect;
	private Paint iMessageBackgroundPaint = new Paint();
	private boolean iNavigating = false;
	private boolean iSimulatingNavigation = false;
	private double iCurLong;
	private double iCurLat;
	private double iCurBearing;
	private double iRouteStartLong;
	private double iRouteStartLat;
    private double iRouteEndLong;
    private double iRouteEndLat;
    private long iRoutePosId = 0;
	private int iNavigationState;
	private Turn iFirstTurn = new Turn();
    private double iFirstTurnDistance;
	private Turn iSecondTurn = new Turn();
    private double iSecondTurnDistance;
    private String iNavigationMessage1 = "NOT NAVIGATING";
    private String iNavigationMessage2;
    private String iNavigationMessage3;
    private int iScreenDpi = 160;
    private ScaleGestureDetector iScaleGestureDetector;
	}
