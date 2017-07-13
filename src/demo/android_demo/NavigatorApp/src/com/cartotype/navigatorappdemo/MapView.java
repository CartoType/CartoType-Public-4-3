package com.cartotype.navigatorappdemo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextWatcher;
import android.text.format.Time;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.ScaleGestureDetector.OnScaleGestureListener;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.RadioButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.cartotype.*;

/**
 * This implements a display of a CartoType map.
 */
class MapView extends View implements OnTouchListener, LocationListener,
		OnScaleGestureListener
	{

	// For keeping track of push pins
	private static final int ID_SELECTED_POINT = 1;
	private static final int ID_ROUTE_START = 2;
	private static final int ID_ROUTE_END = 3;
	private static final int ID_CURRENT_LOCATION = 4;

	private final Context context;

    // File Locations.
    
    // Note that as CartoType is a native library, it cannot directly access files
    // from .apk assets. Test map, stylesheet and font are therefore copied
    // to your devices external memory, often an sdcard.
    
    // Set default starting map for first session and in case of crashes due to
    // corrupt map downloads,
    // (User can then substitute and persist another map).
    // On most devices the ExternalStorageDirectory is /sdcard
    private static final String iCartoTypeFolder = Environment
            .getExternalStorageDirectory().getPath() + "/CartoType";
    private static final String iDefaultMapFile = iCartoTypeFolder
            + "/map/santa-cruz.ctm1";
    // This is set using restoreSavedMapFile(), it defaults to iDefaultMapFile
    private String iMapFile;
    // Set location and name of the stylesheet to be loaded
    private static final String FILE_STYLE_SHEET = iCartoTypeFolder
            + "/style/standard.ctstyle";
    // Set location and name of the initial font to be loaded (this demo only loads one)
    private static final String FILE_FONT = iCartoTypeFolder
            + "/font/DejaVuSans.ttf";

	/**
	 * A string constant to use in calls to the "Log" methods.
	 */
	private static final String TAG = "MapView";

	/**
	 * Main constructor
	 * 
	 * @param aContext
	 * @param aScreenDpi
	 */
	@SuppressLint("ClickableViewAccessibility")
	public MapView(Context aContext, int aScreenDpi)
		{
		super(aContext);
		// Need to provide a performClick() method in a production version to guarenteee accessibility services
		setOnTouchListener(this);
		iScreenDpi = aScreenDpi;
		context = aContext;

		}

	/**
	 * Standard constructor to keep Eclipse IDE lint check and layout editor
	 * happy.
	 * 
	 * @param aContext
	 */
	@SuppressLint("ClickableViewAccessibility")
	public MapView(Context aContext)
		{
		super(aContext);
		setOnTouchListener(this);
		iScreenDpi = 160;
		context = aContext;

		}

	/**
	 * Initiate a new map display. Needs to be called when the app starts or
	 * when the user wants to use a different map file.
	 */
	public void init()
		{
		setKeepScreenOn(true);
		iScaleGestureDetector = new ScaleGestureDetector(getContext(), this);

		// Before we start the CartoType framework, we need to make sure that
		// folder and file dependencies are satisfied.
		folderCheck();

		// Map box background is uncovered when panning or zooming,
		// change to an unobtrusive grey.
		setBackgroundColor(0xFFCCCCCC);

		int w = getWidth();
		int h = getHeight();

		restoreSavedMapFile();

		// Check that the map file exists before starting map
		File f = new File(iMapFile);
		if (!f.exists())
			{
			// Hmm, substitute in the default file
			Notify("WARNING: Could not find your map file " + iMapFile
					+ ", using " + iDefaultMapFile + " instead.");
			Log.e(TAG, "Map file not found: " + iMapFile);
			iMapFile = iDefaultMapFile;
			// User's previous state is no longer relevant
			deleteState();
			}
            
		Log.v(TAG, "Creating new framework using map: " + iMapFile);
		// Note: System.loadLibrary("cartotype") is automatically executed when
		// creating the Framework instance, you do not need explicitly code it.
		iFramework = new Framework(iMapFile, FILE_STYLE_SHEET, FILE_FONT, w, h);
		iFramework.setResolutionDpi(iScreenDpi);

		// For routing/navigation functions
		iFramework.setNavigatorMinimumFixDistance(5);
		iFramework.setNavigatorTimeTolerance(15);

		// Create a Legend object with a scale bar
		iLegend = new Legend(iFramework);
		iLegend.setBackgroundColor(Framework.Color(255,255,255,128)); // half-transparent white
		iLegend.addScaleLine(true);

		// Set position, scale, rotation, perspective etc from a previous
		// session
		restoreState();

		// Now we have parameters set up, display the map
		getMap();
		// Force screen update (only relevant if we call init() for a second
		// time when changing map)
		invalidate();

		}

	/**
	 * Check that the prerequisite sdcard folders and files exist and put in
	 * place if not. Folders are created by software. Files come from .apk
	 * "assets" section
	 */
	private void folderCheck()
		{

		// Does parent folder exist ?
		File dir = new File(iCartoTypeFolder);
		if (!dir.exists())
			{
			dir.mkdirs();
			}

		// Does log folder exist ?
		dir = new File(iCartoTypeFolder + "/log");
		if (!dir.exists())
			{
			dir.mkdirs();
			}

		// Does default map file exist ?
		File f = new File(iDefaultMapFile);
		if (!f.exists())
			{
			assetCopy("map");
			Log.i(TAG, "Default Map file not found (first time to run app?): "
					+ iDefaultMapFile);
			}

		// Does font file exist ?
		f = new File(FILE_FONT);
		if (!f.exists())
			{
			assetCopy("font");
			Log.i(TAG, "Font file not found (first time to run app?): "
					+ FILE_FONT);
			}

		// Does style sheet file exist ?
		f = new File(FILE_STYLE_SHEET);
		if (!f.exists())
			{
			assetCopy("style");
			Log.i(TAG, "Style sheet file not found (first time to run app?): "
					+ FILE_STYLE_SHEET);
			}

		}

	private void assetCopy(String assetfolder)
		{

		File sdcarddir = new File(iCartoTypeFolder + "/" + assetfolder);
		if (!sdcarddir.exists())
			{
			sdcarddir.mkdirs();
			}

		AssetManager assetManager = getResources().getAssets();
		String[] files = null;
		try
			{
			files = assetManager.list(assetfolder);
			} catch (Exception e)
			{
			Log.e("read asset ERROR ", e.toString());
			e.printStackTrace();
			}
		for (int i = 0; i < files.length; i++)
			{
			InputStream in = null;
			OutputStream out = null;
			try
				{
				in = assetManager.open(assetfolder + "/" + files[i]);
				out = new FileOutputStream(iCartoTypeFolder + "/" + assetfolder
						+ "/" + files[i]);
				copyFile(in, out);
				in.close();
				in = null;
				out.flush();
				out.close();
				out = null;
				} catch (Exception e)
				{
				Log.e("copy asset ERROR ", e.toString());
				e.printStackTrace();
				}
			}
		}

	/**
	 * Used by assetCopy()
	 * 
	 * @param in
	 * @param out
	 * @throws IOException
	 */
	private void copyFile(InputStream in, OutputStream out) throws IOException
		{
		byte[] buffer = new byte[1024];
		int read;
		while ((read = in.read(buffer)) != -1)
			{
			out.write(buffer, 0, read);
			}
		}

	/**
	 * Displays a address-oriented FindAddress pop-up window, initialising if
	 * necessary. This is the main entry point to the FindAddress pop-up
	 * functionality.
	 */
	public void showFindAddress()
		{

		if (iIsFirstShowFindAddr)
			{

			showFindAddr_init();
			iIsFirstShowFindAddr = false;
			}

		iFindAddrPopup.showAtLocation(this, Gravity.TOP, 0, 50);

		}

	/**
	 * Provides functionality to do a Find/Search using a PopupWindow
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	public void showFindAddr_init()
		{

		// Get layout from XML layout file
		final LayoutInflater factory = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		final View layout = factory.inflate(R.layout.findaddr_form, null);

		// Start a pop-up screen
		int x = (int) Math.floor((double) this.getWidth() * 0.8);
		int y = (int) Math.floor((double) this.getHeight() / 1.2);
		if (y > 350) {
			// Quick hack for displaying on larger screens
			y = 350;
		}
		iFindAddrPopup = new PopupWindow(this, x, y);

		layout.setBackgroundColor(Color.DKGRAY);
		layout.getBackground().setAlpha(250);
		iFindAddrPopup.setContentView(layout);

		if (iFindAddrSearchField1 == null)
			{
			// Set up functionality to so that the user can clear any text typed
			// in the search field
			iFindAddrButtonClearText = (Button) layout
					.findViewById(R.id.findaddr_form_button_text_clear);
			iFindAddrButtonClearText.setVisibility(RelativeLayout.INVISIBLE);
			iFindAddrButtonClearText.setOnClickListener(new OnClickListener()
				{

					public void onClick(View v)
						{

						iFindAddrSearchField1.setText("");
						iFindAddrSearchField2.setText("");
						iFindAddrSearchField3.setText("");
						iFindAddrSearchField4.setText("");
						iFindAddrSearchField5.setText("");
						iFindAddrSearchField6.setText("");
						iFindAddrSearchField7.setText("");
						iFindAddrSearchField8.setText("");
						}

				});

			}

		if (iFindAddrSearchField1 == null)
			{
			// First time we have been in this method
			iFindAddrSearchField1 = (EditText) layout
					.findViewById(R.id.findaddr_form_editText1);
			iFindAddrSearchField1.setFocusable(true);

			iFindAddrSearchField1.addTextChangedListener(new TextWatcher()
				{

					@Override
					public void onTextChanged(CharSequence s, int start,
							int before, int count)
						{
						if (s.length() > 0)
							iFindAddrButtonClearText
									.setVisibility(RelativeLayout.VISIBLE);
						else
							iFindAddrButtonClearText
									.setVisibility(RelativeLayout.INVISIBLE);
						}

					@Override
					public void beforeTextChanged(CharSequence s, int start,
							int count, int after)
						{
						// TODO Auto-generated method stub

						}

					@Override
					public void afterTextChanged(Editable s)
						{
						// TODO Auto-generated method stub

						}
				});
			}

		iFindAddrSearchField1.requestFocus();

		iFindAddrSearchField2 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText2);
		iFindAddrSearchField3 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText3);
		iFindAddrSearchField4 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText4);
		iFindAddrSearchField5 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText5);
		iFindAddrSearchField6 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText6);
		iFindAddrSearchField7 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText7);
		iFindAddrSearchField8 = (EditText) layout
				.findViewById(R.id.findaddr_form_editText8);

		// Set up functionality to so that the user can dismiss the popup window
		Button buttonDismiss = (Button) layout
				.findViewById(R.id.findaddr_form_button_dismiss);
		buttonDismiss.setOnClickListener(new OnClickListener()
			{

				public void onClick(View v)
					{
					iFindAddrPopup.dismiss();
					}

			});

		// Set up functionality to so that the user can perform the actual find
		Button buttonFind = (Button) layout
				.findViewById(R.id.findaddr_form_button_search);
		buttonFind.setOnClickListener(new OnClickListener()
			{

				public void onClick(View v)
					{

					// Grab the search text and search type the User entered.

					// Force the soft keyboard to close (until the User touches
					// search field again)
					InputMethodManager imm = (InputMethodManager) context
							.getSystemService(Activity.INPUT_METHOD_SERVICE);
					imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY,
							0);

					Log.v(TAG, "User is searching for " + "1 "
							+ iFindAddrSearchField1.getText().toString()
							+ ", 2 "
							+ iFindAddrSearchField2.getText().toString()
							+ ", 3 "
							+ iFindAddrSearchField3.getText().toString()
							+ ", 4 "
							+ iFindAddrSearchField4.getText().toString()
							+ ", 5 "
							+ iFindAddrSearchField5.getText().toString()
							+ ", 6 "
							+ iFindAddrSearchField6.getText().toString()
							+ ", 7 "
							+ iFindAddrSearchField7.getText().toString()
							+ ", 8 "
							+ iFindAddrSearchField8.getText().toString());

					// cartotype.navigator.Address
					Address address = new Address();
					address.iBuilding = iFindAddrSearchField1.getText()
							.toString();
					address.iFeature = iFindAddrSearchField2.getText()
							.toString();
					address.iStreet = iFindAddrSearchField3.getText()
							.toString();
					address.iLocality = iFindAddrSearchField4.getText()
							.toString();
					address.iSubAdminArea = iFindAddrSearchField5.getText()
							.toString();
					address.iAdminArea = iFindAddrSearchField6.getText()
							.toString();
					address.iCountry = iFindAddrSearchField7.getText()
							.toString();
					address.iPostCode = iFindAddrSearchField8.getText()
							.toString();
					doFindAddr(address, 40, layout);

					}

			});

		// Needs to be false to continue panning map but true to get a soft
		// keyboard for the Find edit field
		iFindAddrPopup.setFocusable(true);

		// Force the soft keyboard to show immediately
		if (android.os.Build.VERSION.SDK_INT >= 11)
			{
			iFindAddrPopup
					.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE
							| WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);
			} else
			{
			iFindAddrPopup
					.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
			}
		
		iFindAddrPopup.setOutsideTouchable(false);
		iFindAddrPopup.setTouchable(true);

		}

	/**
	Finds an address and puts the found objects in a View.
	*/
	public void doFindAddr(Address address, int aMaxObjectCount, View view)
		{

		// In this example, fuzzy word searching is allowed, third parameter.
		MapObject[] mapObjects = iFramework.findAddress(aMaxObjectCount,address,true);

		TextView textView = (TextView) view
				.findViewById(R.id.findaddr_form_textView1);

		if (mapObjects != null)
			{
			if (mapObjects.length == 0)
				{

				textView.setText("Nothing found.");

				} else
				{

				// OK, we found something.
				// Remove the input form from the display
				iFindAddrForm = (RelativeLayout) view
						.findViewById(R.id.findAddrLayout_Form);
				iFindAddrForm.setVisibility(RelativeLayout.GONE);
				// Make the results layout visible
				// iFindAddrResults =
				// (RelativeLayout)view.findViewById(R.id.findAddrLayout_Results);
				// iFindAddrResults.setVisibility(RelativeLayout.VISIBLE);

				// When the User clicks this button, the input form toggles for
				// the results list
				// there appears to be a bug in Android that prevents the
				// results view being made fully invisible)
				iFindAddrButtonSearchAgain = (Button) view
						.findViewById(R.id.find_results_button_search_again);
				iFindAddrButtonSearchAgain
						.setVisibility(RelativeLayout.VISIBLE);
				iFindAddrButtonSearchAgain
						.setOnClickListener(new OnClickListener()
							{

								public void onClick(View v)
									{
									// Android bug?
									// Causes findAddrLayout_Top (top row and
									// buttons) to be invisible too!
									// iFindAddrResults.setVisibility(RelativeLayout.GONE);

									iFindAddrForm
											.setVisibility(RelativeLayout.VISIBLE);
									iFindAddrButtonSearchAgain
											.setVisibility(RelativeLayout.GONE);
									}

							});

				List<FindListItem> listMap = new ArrayList<FindListItem>();

				textView.setText("");

				// Silently ignore any repetitions of getGeoCodeSummary,
				// in OpenStreetMap data, they are probably segments of the same
				// road.
				HashMap<String, Boolean> duplicates = new HashMap<String, Boolean>();

				for (MapObject mapObject : mapObjects)
					{

					// If using fuzzy searching, some of the items returned have
					// no label,
					// ignore them.

					String summary = mapObject.getGeoCodeSummary();

					if (!duplicates.containsKey(summary))
						{
						duplicates.put(summary, true);

						if (mapObject.getLabel().length() != 0)
							{

							// Extract a representative lat/lon
							double[] point = getCenterLatLon(mapObject);
							double longitude = point[0];
							double latitude = point[1];

							// We need an approximation of linear extent of
							// the object in order to calculate an appropriate
							// display map scale
							double size = 0;
							int type = mapObject.getType();
							switch (type)
								{
								case MapObject.POINT_TYPE:
									// Log.v(TAG,"Point type");
									break;
								case MapObject.POLYGON_TYPE:
									// Assume feature to be roughly equilinear
									size = Math.sqrt(mapObject.getArea());
									break;
								case MapObject.LINE_TYPE:
								case MapObject.ARRAY_TYPE:
									/*
									 * getArea is zero for these types so we
									 * need another method Assume the centre
									 * point is half-way along (Slightly more
									 * sophisticated than taking start and end
									 * points, as they may be close together.)
									 */

									// Get the start point
									com.cartotype.PathPoint startPoint = new com.cartotype.PathPoint();
									mapObject.getPoint(0, 0, startPoint);

									// Get the centre point (in map coordinates
									// this time)
									com.cartotype.PathPoint centrePoint = new com.cartotype.PathPoint();
									mapObject.getCenter(centrePoint);

									double x = centrePoint.iX - startPoint.iX;
									double y = centrePoint.iY - startPoint.iY;

									size = Math.sqrt((x * x) + (y * y));

									break;
								default:
									break;
								}

							listMap.add(new FindListItem(mapObject,mapObject.getLabel(),
									summary, longitude, latitude, size));
							}
						}

					}

				// Create data adapter

				final FindListAdapter iFindListAdapter = new FindListAdapter(
						context, R.layout.find_results_row_item, listMap);

				ListView listViewMapFile = (ListView) view
						.findViewById(R.id.findaddr_list);

				listViewMapFile.setAdapter(iFindListAdapter);

				listViewMapFile
						.setOnItemClickListener(new OnItemClickListener()
							{

								/**
								 * User has clicked on one of the results of a
								 * Find. We "expand" the entry by making a Set
								 * Route Start and a Set Route End button
								 * visible.
								 * 
								 * TODO Need also to close (un-expand) other
								 * entries and cancel their click listeners so
								 * that the click listener comes back to here.
								 */
								@Override
								public void onItemClick(AdapterView<?> parent,
										View arg1, int position, long id)
									{

									// Retrieve the data we saved during the
									// Find.
									FindListItem findListItem = (FindListItem) parent
											.getItemAtPosition(position);
									iFindListAdapter
											.setSelectedPosition(position);

									// Move the map to the item location
									findListItem.setMap();

									}

							});
				}
			} else
			{

			// No Map Objects were returned.

			// Make sure any previous results are no longer displayed
			List<FindListItem> listMap = new ArrayList<FindListItem>();
			ListView listViewMapFile = (ListView) view
					.findViewById(R.id.findaddr_list);
			listViewMapFile.setAdapter(new FindListAdapter(context,
					R.layout.find_results_row_item, listMap));

			textView.setText("Nothing found");
			invalidate();
			Log.e(TAG, "Nothing found");

			}
		}

	/**
	 * Displays a general Find pop-up window, initialising if necessary. This is
	 * the main entry point to the Find pop-up functionality.
	 */
	public void showFind()
		{

		if (iIsFirstShowFind)
			{

			showFind_init();
			iIsFirstShowFind = false;
			}

		iFindPopup.showAtLocation(this, Gravity.TOP, 0, 0);

		}

	/**
	 * Provides functionality to do a Find/Search using a PopupWindow
	 */
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	public void showFind_init()
		{

		// Get layout from XML layout file
		final LayoutInflater factory = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		final View layout = factory.inflate(R.layout.find_form, null);

		int x = (int) Math.floor((double) this.getWidth() * 0.8);
		// Start a pop-up screen
		int y = (int) Math.floor((double) this.getHeight() / 2);
		iFindPopup = new PopupWindow(this, x, y);

		layout.setBackgroundColor(Color.DKGRAY);
		layout.getBackground().setAlpha(250);
		iFindPopup.setContentView(layout);

		if (iFindSearchField == null)
			{
			// Set up functionality to so that the user can clear any text typed
			// in the search field
			iFindButtonClearText = (Button) layout
					.findViewById(R.id.find_form_button_text_clear);
			// The button should not be shown initially (because no text has
			// been entered)
			iFindButtonClearText.setVisibility(RelativeLayout.INVISIBLE);
			iFindButtonClearText.setOnClickListener(new OnClickListener()
				{

					public void onClick(View v)
						{

						iFindSearchField.setText("");
						}

				});

			}

		if (iFindSearchField == null)
			{
			// First time we have been in this method
			iFindSearchField = (EditText) layout
					.findViewById(R.id.find_form_editText1);
			iFindSearchField.setFocusable(true);
			// iSearchField.setText(iSearchText);
			iFindSearchField.addTextChangedListener(new TextWatcher()
				{

					@Override
					public void onTextChanged(CharSequence s, int start,
							int before, int count)
						{
						if (s.length() > 0)
							iFindButtonClearText
									.setVisibility(RelativeLayout.VISIBLE);
						else
							iFindButtonClearText
									.setVisibility(RelativeLayout.INVISIBLE);
						}

					@Override
					public void beforeTextChanged(CharSequence s, int start,
							int count, int after)
						{
						// TODO Auto-generated method stub

						}

					@Override
					public void afterTextChanged(Editable s)
						{
						// TODO Auto-generated method stub

						}
				});
			}

		iFindSearchField.requestFocus();

		// Set up functionality to so that the user can dismiss the popup window
		Button buttonDismiss = (Button) layout
				.findViewById(R.id.find_form_button_dismiss);
		buttonDismiss.setOnClickListener(new OnClickListener()
			{

				public void onClick(View v)
					{
					iFindPopup.dismiss();
					}

			});

		// Set up functionality to so that the user can perform the actual find
		Button buttonFind = (Button) layout
				.findViewById(R.id.find_form_button_search);
		buttonFind.setOnClickListener(new OnClickListener()
			{

				public void onClick(View v)
					{

					// Grab the search text and search type the User entered.

					final RadioButton fuzzyMatch = (RadioButton) layout
							.findViewById(R.id.find_form_radio2);
					final RadioButton startsMatch = (RadioButton) layout
							.findViewById(R.id.find_form_radio1);
					int searchType = Framework.EXACT_STRING_MATCH_METHOD;
					if (fuzzyMatch.isChecked())
						{
						searchType = Framework.FUZZY_STRING_MATCH_FLAG;
						} else if (startsMatch.isChecked())
						{
						searchType = Framework.PREFIX_STRING_MATCH_FLAG;
						}

					// Force the soft keyboard to close (until the User touches
					// search field again)
					InputMethodManager imm = (InputMethodManager) context
							.getSystemService(Activity.INPUT_METHOD_SERVICE);
					imm.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY,
							0);

					Log.v(TAG, "User is searching for "
							+ iFindSearchField.getText().toString());
					doFind(iFindSearchField.getText().toString(), searchType,
							10, layout);

					TextView tv = (TextView) layout
							.findViewById(R.id.find_form_textView3);
					tv.setText("");
					tv.setTextColor(Color.BLACK);
					}

			});

		// Needs to be false to continue panning map but true to get a soft
		// keyboard for the Find edit field
		iFindPopup.setFocusable(true);

		// Force the soft keyboard to show immediately
		if (android.os.Build.VERSION.SDK_INT >= 11)
			{
			iFindPopup
					.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE
							| WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);
			} else
			{
			iFindPopup
					.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
			}
		
		iFindPopup.setOutsideTouchable(false);
		iFindPopup.setTouchable(true);

		}

	/**
	 * A find function that returns finds occurrences of aSearchString and
	 * returns the values to a ListView.
	 * 
	 * @param aSearchString
	 * @param aMatchMethod
	 * @param aMaxObjectCount
	 * @return
	 */
	public void doFind(String aSearchString, int aMatchMethod,
			int aMaxObjectCount, View view)
		{

		MapObject[] mapObjects = iFramework.find(aMaxObjectCount,aSearchString,aMatchMethod);

		TextView textView = (TextView) view
				.findViewById(R.id.find_form_textView3);

		if (mapObjects != null)
			{
			if (mapObjects.length == 0)
				{

				textView.setText("Sorry, could not find anything");

				} else
				{
				List<FindListItem> listMap = new ArrayList<FindListItem>();

				textView.setText("");

				for (MapObject mapObject : mapObjects)
					{

					// If using fuzzy searching, some of the items returned have
					// no label, ignore them.
					if (mapObject.getLabel().length() != 0)
						{

						// Extract a representative lat/lon
						double[] point = getCenterLatLon(mapObject);
						double longitude = point[0];
						double latitude = point[1];

						// We need an approximation of linear extent of
						// the object in order to calculate an appropriate
						// display map scale
						double size = 0;
						int type = mapObject.getType();
						switch (type)
							{
							case MapObject.POINT_TYPE:
								// Log.v(TAG,"Point type");
								break;
							case MapObject.POLYGON_TYPE:
								// Assume feature to be roughly equilinear
								size = Math.sqrt(mapObject.getArea());
								break;
							case MapObject.LINE_TYPE:
							case MapObject.ARRAY_TYPE:
								/*
								 * getArea is zero for these types so we need
								 * another method Assume the centre point is
								 * half-way along (Slightly more sophisticated
								 * than taking start and end points, as they may
								 * be close together.)
								 */

								// Get the start point
								com.cartotype.PathPoint startPoint = new com.cartotype.PathPoint();
								mapObject.getPoint(0, 0, startPoint);

								// Get the centre point (in map coords this
								// time)
								com.cartotype.PathPoint centrePoint = new com.cartotype.PathPoint();
								mapObject.getCenter(centrePoint);

								double x = centrePoint.iX - startPoint.iX;
								double y = centrePoint.iY - startPoint.iY;

								size = Math.sqrt((x * x) + (y * y));

								break;
							default:
								break;
							}

						listMap.add(new FindListItem(mapObject,mapObject.getLabel(),
								mapObject.getGeoCodeSummary(),
								longitude, latitude, size));
						}

					}

				// Create data adapter

				final FindListAdapter iFindListAdapter = new FindListAdapter(
						context, R.layout.find_results_row_item, listMap);

				ListView listViewMapFile = (ListView) view
						.findViewById(R.id.find_list);

				listViewMapFile.setAdapter(iFindListAdapter);

				listViewMapFile
						.setOnItemClickListener(new OnItemClickListener()
							{

								/**
								 * User has clicked on one of the results of a
								 * Find. We "expand" the entry by making a Set
								 * Route Start and a Set Route End button
								 * visible.
								 * 
								 * TODO Need also to close (un-expand) other
								 * entries and cancel their click listeners so
								 * that the click listener comes back to here.
								 */
								@Override
								public void onItemClick(AdapterView<?> parent,
										View arg1, int position, long id)
									{

									// Retrieve the data we saved during the
									// Find.
									FindListItem findListItem = (FindListItem) parent
											.getItemAtPosition(position);
									iFindListAdapter
											.setSelectedPosition(position);

									// Move the map to the item location
									findListItem.setMap();

									}

							});
				}
			} else
			{

			// Make sure any previous results are no longer displayed
			List<FindListItem> listMap = new ArrayList<FindListItem>();
			ListView listViewMapFile = (ListView) view
					.findViewById(R.id.find_list);
			listViewMapFile.setAdapter(new FindListAdapter(context,
					R.layout.find_results_row_item, listMap));

			textView.setText("Nothing found");
			invalidate();
			Log.e(TAG, "Nothing found");

			}
		}

	/**
	 * Find object. The main functionality of this object is the
	 * OnClickListener() method. This allows us to then display a list of found
	 * MapObjects and attach a click listener to display buttons.
	 */
	public class FindListItem
		{
		private MapObject mapObject;
		/**
		 * CartoType label (we are only interested in objects that actually have
		 * a name.
		 */
		private String name;
		private String geoCodeSummary;
		private double longitude;
		private double latitude;
		/**
		 * An approximation on how "wide" a feature is in metres, used to
		 * calculate appropriate display scale. Zero for point objects.
		 */
		private double size;

		/**
		 * This can be attached to one or more (ListView) display item buttons
		 */
		public OnClickListener listener = new OnClickListener()
			{
				@Override
				public void onClick(View v)
					{
					// This allows us to create multiple buttons to allow
					// User to do different things with a found MapObject
					switch (v.getId())
						{
						case R.id.find_results_button_route_start:

							// User has clicked "Route Start" button
							iRouteStartLong = longitude;
							iRouteStartLat = latitude;

							deleteSelectedPoint();
							iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
									ID_ROUTE_START, ID_ROUTE_START, null);
							iFramework.insertPointMapObject(
									iFramework.getMemoryMapHandle(),
									"route_start",
									iRouteStartLong, iRouteStartLat,
									Framework.DEGREE_COORDS, "route start",
									0, ID_ROUTE_START, true);
							
							if (iDisplayRoute)
								{
								// Reset existing route display
								iFramework.endNavigation();
								setDisplayRouteOn();
								} else if ((iRouteEndLong != 0 && iRouteEndLat != 0))
								{
								setDisplayRouteOn();
								}

							break;
						case R.id.find_results_button_route_end:

							// User has clicked "Route End" button
							iRouteEndLong = longitude;
							iRouteEndLat = latitude;

							/**
							 * Add a point object. The id aId can be used to
							 * reference the object from other functions. If
							 * aCircleRadius is greater than zero, convert it to
							 * a circle of the specified radius in map meters
							 * (projected meters). Return a CartoType error
							 * code: 0 = success.
							 */

							deleteSelectedPoint();

							iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
									ID_ROUTE_END, ID_ROUTE_END, null);
							iFramework.insertPointMapObject(
									iFramework.getMemoryMapHandle(),
									"route_end",
									iRouteEndLong, iRouteEndLat,
									Framework.DEGREE_COORDS, "route end", 0,
									ID_ROUTE_END, true);
							
							if (iDisplayRoute)
								{
								// Reset existing route display
								iFramework.endNavigation();
								setDisplayRouteOn();
								} else if ((iRouteStartLong != 0 && iRouteStartLat != 0))
								{
								setDisplayRouteOn();
								}
							break;
						default:
							break;
						}
					getMap();
					invalidate();
					}
			};

		/**
		 * Constructor
		 * 
		 * @param aName
		 * @param aLongitude
		 * @param aLatitude
		 */
		public FindListItem(MapObject aMapObject, String aName, String aGeoCodeSummary,
				double aLongitude, double aLatitude, double aSize)
			{
			super();
			this.mapObject = aMapObject;
			this.name = aName;
			this.geoCodeSummary = aGeoCodeSummary;
			this.longitude = aLongitude;
			this.latitude = aLatitude;
			this.size = aSize;
			}

		/**
		 * Adjust the map display to show the the location of the current
		 * FindListItem and set the Selected Point visible marker on the map.
		 * The map is centred just above the point so that both the Find menu
		 * and the point are visible on WVGA and similar displays.
		 * 
		 */
		public void setMap()
			{
			iFramework.setView(mapObject,32,5000);

			// Display, Set and remember "Selected Point"
			setSelectedPoint(getLongitude(), getLatitude());

			// Optional, post a transient message displaying the lat and lon of
			// the selected point.
			Notify("lat/lon is " + iInterestLat + " " + iInterestLong);

			// Update map display
			getMap();
			invalidate();
			}

		// Accessor
		public String getName()
			{
			return this.name;
			}

		// Modifier, not really needed as the Constructor does the work
		public void setName(String nameText)
			{
			this.name = nameText;
			}

		// Accessor
		public String getGeoCodeSummary()
			{
			return this.geoCodeSummary;
			}

		// Modifier, not really needed as the Constructor does the work
		public void setGeoCodeSummary(String value)
			{
			this.name = value;
			}

		// Accessor
		public double getLongitude()
			{
			return this.longitude;
			}

		// / Modifier, not really needed as the Constructor does the work
		public void setLongitude(double value)
			{
			this.longitude = value;
			}

		// Accessor
		public double getLatitude()
			{
			return this.latitude;
			}

		// Modifier, not really needed as the Constructor does the work
		public void setLatitude(double value)
			{
			this.latitude = value;
			}

		/*
		 * Accessor. An actual or approximate measure of the size of the map
		 * object as it will appear on the map, I.e. length of a straight road
		 * or approximate width of a polygonal area. Zero for point objects.
		 */
		public double getSize()
			{
			return this.size;
			}

		// Modifier, not really needed as the Constructor does the work
		public void setSize(double value)
			{
			this.size = value;
			}

		}

	/**
	 * Built with hints from
	 * "Tutorial: ListView in Android using custom ListAdapter and Cache View" ,
	 * Fabio Tuzza,
	 * http://www.framentos.com/en/android-tutorial/2012/07/16/listview
	 * -in-android-using-custom-listadapter-and-viewcache/
	 */
	private class FindListAdapter extends ArrayAdapter<FindListItem>
		{
		private int resource;
		private LayoutInflater inflater;
		private TextView txtName;

		// used to keep selected position in ListView
		private int selectedPosition = -1; // init value for not-selected

		public FindListAdapter(Context ctx, int resourceId,
				List<FindListItem> objects)
			{

			super(ctx, resourceId, objects);
			resource = resourceId;
			inflater = LayoutInflater.from(ctx);

			}

		/**
		 * Keeps track of what item in the list a User has clicked (if any) -
		 * Modifier
		 * 
		 * @param position
		 */
		public void setSelectedPosition(int position)
			{
			selectedPosition = position;
			// inform the view of this change
			notifyDataSetChanged();
			}

		@SuppressWarnings("unused")
		public int getSelectedPosition()
			{
			return selectedPosition;
			}

		@Override
		public View getView(int position, View convertView, ViewGroup parent)
			{

			// only inflate the view if it's null, i.e. set up for first time
			if (convertView == null)
				{
				/* create a new view of my layout and inflate it in the row */
				convertView = (RelativeLayout) inflater.inflate(resource, null);
				}

			FindListItem findListItem = (FindListItem) getItem(position);

			txtName = (TextView) convertView
					.findViewById(R.id.find_results_name);
			txtName.setText(findListItem.getGeoCodeSummary());

			Button buttonStart = (Button) convertView
					.findViewById(R.id.find_results_button_route_start);

			Button buttonEnd = (Button) convertView
					.findViewById(R.id.find_results_button_route_end);

			if (selectedPosition == position)
				{
				txtName.setTextColor(Color.CYAN);
				buttonStart.setVisibility(View.VISIBLE);
				buttonStart.setOnClickListener(findListItem.listener);
				buttonEnd.setVisibility(View.VISIBLE);
				buttonEnd.setOnClickListener(findListItem.listener);

				} else
				{
				txtName.setTextColor(Color.WHITE);
				buttonStart.setVisibility(View.GONE);
				buttonEnd.setVisibility(View.GONE);
				}

			return convertView;
			}
		}

	/**
	 * Display a PopupWindow showing nearby POIs
	 */
	public void showNearbyPOIs()
		{

		if (noSelectedPoint())
			{
			return;
			}
		// Get the data
		String result = getNearbyPOIs();

		// Start a pop-up screen (100% programmatically, no XML layout)
		PopupWindow popUp = new PopupWindow(context);

		LinearLayout layout = new LinearLayout(context);
		LayoutParams params = new LayoutParams(LayoutParams.WRAP_CONTENT,
				LayoutParams.WRAP_CONTENT);
		layout.setBackgroundColor(Color.GRAY);
		layout.getBackground().setAlpha(5);

		TextView tv = new TextView(context);
		tv.setText("These are nearby:\n\n" + result);

		layout.addView(tv, params);
		popUp.setContentView(layout);
		popUp.setOutsideTouchable(true);
		popUp.dismiss();
		popUp.showAtLocation(layout, Gravity.CENTER, 0, 0);
		// TODO Hard coded absolute pixels, not density pixels. May degrade on
		// hi-resolution devices-
		popUp.update(0, 0, 450, 300);

		}

	/**
	 * Demonstrates the Framework.findInDisplay() method. Using the current map
	 * centre as a reference points, it searches for and returns a list of
	 * nearby features, (streets, POIs, etc).
	 * 
	 * @return Human readable summary list of the points found.
	 */
	public String getNearbyPOIs()
		{

		String result = "";

		double[] point = new double[2];

		// Use current map centre as the point.
		// getMapPosition(point);

		// OR use the current selected point
		point[0] = iInterestLong;
		point[1] = iInterestLat;

		// Convert from map coord to screen coord
		iFramework.convertCoords(point, Framework.DEGREE_COORDS,
				Framework.SCREEN_COORDS);
		// xcord (pixels), y coord (pixels), radius (pixels), max no of objects
		MapObject[] mapObjects = iFramework.findInDisplay(20,point[0],point[1],15);

		if (mapObjects != null && mapObjects.length > 0)
			{
			int i = 0;
			for (MapObject mapObject : mapObjects)
				{

				// Ignore any feature with no name.
				// Ignore ourselves, i.e. the selected point used as the
				// reference for search
				if (mapObject.getLabel().length() != 0
						&& !mapObject.getLabel().equals("my selected point"))
					{

					double[] resultPoint = getCenterLatLon(mapObject);
					i++;
					result += mapObject.getLabel() + " lat="
							+ String.format("%.5f", resultPoint[1]) + ", lon="
							+ String.format("%.5f", resultPoint[0]) + "\n";

					}
				}
			if (i == 0)
				result += "Nothing found close by with a name.";

			} else
			{
			result += "Nothing found close by.";
			}

		return result;
		}

	/**
	 * Wraps the MapObject getCenter() method to return point in lat/lon.
	 * 
	 * @param aMapObject
	 * @return two point array: longitude, latitude
	 */
	private double[] getCenterLatLon(MapObject aMapObject)
		{

		com.cartotype.PathPoint pathPoint = new com.cartotype.PathPoint();
		aMapObject.getCenter(pathPoint);
		double[] point = new double[2];
		point[0] = pathPoint.iX;
		point[1] = pathPoint.iY;

		// Convert from map UTM-like coord to lat/lon coord
		iFramework.convertCoords(point, Framework.MAP_COORDS,
				Framework.DEGREE_COORDS);

		return point;
		}

	/**
	 * Toggles the map from a normal flat 2D view to a perspective view.
	 */
	public void togglePerspective()
		{
		iPerspective = !iPerspective;
		iFramework.setPerspective(iPerspective);
		getMap();
		invalidate();
		}

	/**
	 * Toggles the display of terrain and terrain shading (if available in the
	 * map file being used).
	 */
	public void toggleTerrain()
		{
		iDisplayTerrain = !iDisplayTerrain;
		iFramework.enableLayer("terrain-height-feet", iDisplayTerrain);
		iFramework.enableLayer("terrain-shadow", iDisplayTerrain);
		getMap();
		invalidate();
		}

	/**
	 * Zoom into the map one degree
	 */
	public void zoomIn()
		{
		iFramework.zoomIn();
		getMap();
		invalidate();

		}

	/**
	 * Zoom out the map one degree
	 */
	public void zoomOut()
		{
		iFramework.zoomOut();
		getMap();
		invalidate();

		}

	/**
	 * Rotate the map 90 degrees clockwise.
	 */
	public void rotate()
		{
		iFramework.rotate(90);
		getMap();
		invalidate();
		}

	/**
	 * Set the map's orientation to an absolute angle given in degrees. Zero
	 * =north-up.
	 * 
	 * @param aAngle
	 */
	public void setRotation(double aAngle)
		{
		iFramework.setRotation(aAngle);
		getMap();
		invalidate();
		}

	/**
	 * When simulating navigation, a route is calculated between the last two
	 * points touched. Further touches provide a simulated vehicle position.
	 */
	public void toggleDisplayRoute()
		{
		if (iDisplayRoute)
			{
			iFramework.endNavigation();
			iFramework.displayRoute(false);
			iDisplayRoute = false;

			getMap();
			invalidate();
			} else
			{
			setDisplayRouteOn();
			}
		}

	/**
	 * Tests to see if a valid navigation route start and end point has been set
	 * and, if so, calls Framework.startNavigation() to calculate and display
	 * the route between them.
	 */
	public void setDisplayRouteOn()
		{
		
		iFramework.displayRoute(true);

		if (iRouteStartLong == 0 || iRouteStartLat == 0 || iRouteEndLong == 0
				|| iRouteEndLat == 0)
			{
			Notify("Cannot display route: positions not known. Please set start and end positions and try again");
			return;
			}

		if (iRouteStartLong == 0 || iRouteStartLat == 0)
			{
			Notify("Cannot display route: Please set start position and try again");
			return;
			}

		if (iRouteEndLong == 0 || iRouteEndLat == 0)
			{
			Notify("Cannot display route: Please set end position and try again");
			return;
			}

		Notify("Now calculating your route, may take a while the first time ...");
		// Force screen update before calculation starts so that Notify() shows
		// NOTE This does not work, Toast (used by Notify *requests* the UI
		// thread
		// to display the message but does not force. The general advice is
		// to put long running processes in an async thread. Not possible here.
		// getMap();
		// invalidate();

		// ((NavigatorAppActivity)context).requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		// TODO This will lock up the UI thread if a complex calculation, really
		// need it to be an ASync task with a spinner. How?
		// ((NavigatorAppActivity)context).setProgressBarIndeterminateVisibility(true);
		int result = iFramework.startNavigation(iRouteStartLong,
				iRouteStartLat, Framework.DEGREE_COORDS, iRouteEndLong,
				iRouteEndLat, Framework.DEGREE_COORDS);

		// ((NavigatorAppActivity)context).setProgressBarIndeterminateVisibility(false);
		// Causes CartoType SIGSEGV
		// RouteCalculation task = new RouteCalculation();
		// task.execute("");

		if (result != 0)
			Notify("Could not display route: error = " + result + ".");
		else
			{
			iDisplayRoute = true;
			}

		getMap();
		invalidate();

		}

	/**
	 * Low-level drawing routine. Overrides View.OnDraw. Provides an image
	 * transformation used during an zoom in/out.
	 */
	@SuppressWarnings("deprecation")
	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@Override
	protected void onDraw(Canvas aCanvas)
		{

		if (iBitmap == null)
			{
			init();
			iPlainMatrix = aCanvas.getMatrix();

			/*
			 * Adjust the matrix to take account of the raw screen position: see
			 * http://code.google.com/p/android/issues/detail?id=24517 Canvas
			 * getMatrix and setMatrix don't correspond when there is graphics
			 * acceleration.
			 */

			// android.graphics.Canvas.isHardwareAccelerated is only available on Android 3.0 and higher
			if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB
						&& aCanvas.isHardwareAccelerated())
				{
				int[] x = new int[2];
				getLocationOnScreen(x);
				iPlainMatrix.preTranslate(x[0], x[1]);
				}
			}

		if (iScale != 1.0 || iXOffset != 0 || iYOffset != 0)
			{
			@SuppressWarnings("deprecation")
			// Google now prefer that View.getMatrix() is used instead of Canvas.getMatrix
			// The reason appears to be explained here: https://code.google.com/p/android/issues/detail?id=24517
			// However, a workaround for this bug is already deployed immediately above.
			Matrix m = aCanvas.getMatrix();
			m.set(iPlainMatrix);
			m.preTranslate(getWidth() / 2 + iXOffset, getHeight() / 2
					+ iYOffset);
			m.preScale(iScale, iScale);
			m.preTranslate(-getWidth() / 2, -getHeight() / 2);
			aCanvas.setMatrix(m);
			}
		else
			aCanvas.setMatrix(iPlainMatrix);

		if (iBitmap != null)
			aCanvas.drawBitmap(iBitmap, 0, 0, null);

		if (iLegendBitmap != null)
			{
			aCanvas.setMatrix(iPlainMatrix);
			aCanvas.drawBitmap(iLegendBitmap, getWidth() - iLegendBitmap.getWidth(), getHeight() - iLegendBitmap.getHeight(), null);
			}
		}

	/**
	 * (Re)Draw the map
	 */
	private void getMap()
		{
		iBitmap = iFramework.getMap();
		iLegendBitmap = iLegend.createLegend(2, "cm", iFramework.getScale(), iFramework.getScaleDenominatorInView());
		}

	/**
	 * Handle what happens when User touches the map.
	 * 
	 * 1) A single-finger drag pans the map
	 * 
	 * 2) A two-fingered pinched gesture zooms the map
	 * 
	 * 3) A single long tap sets a "location of interest", (which can then be
	 * used for viewing nearby POIs, or setting navigation stop/stop points).
	 * 
	 * TODO: Can we also add a gesture for rotating the map??
	 */
	@SuppressLint("ClickableViewAccessibility")
	public boolean onTouch(View aView, MotionEvent aEvent)
		{

		// Handle scale gestures.
		// TODO 2 x NullPointer exception error on next line at start up. Why?
		// Also get NullPointer exception in setScale at start up.
		// Possible cause: appears to happen when User is touching the screen
		// as the app starts => onTouch events start happening before
		// iScaleGestureDetector and iFramework are initialised.
		// 'Orrible HACK:
		if (iScaleGestureDetector == null || iFramework == null)
			return false;
		iScaleGestureDetector.onTouchEvent(aEvent);
		if (iScaleGestureDetector.isInProgress())
			return true;

		switch (aEvent.getAction())
			{
			// User has put finger on the screen,
			// Save the location as the start of a possible drag
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

				// User has lifted finger from the screen,
				// If drag is small, register the location as new current
				// selected point
				// else, complete map pan
			case MotionEvent.ACTION_UP:
				if (iTouchCount != 1)
					break;

				iXOffset = aEvent.getX() - iStartTouchPointX;
				iYOffset = aEvent.getY() - iStartTouchPointY;
				/*
				 * If the movement was very small, drop a pushpin at this point
				 * to mark current selected point. This point can then be used
				 * for displaying surrounding POIs, or marking the start or end
				 * of a navigation route.
				 */
				if (iXOffset > -13 && iXOffset < 13 && iYOffset > -13
						&& iYOffset < 13)
					{

					double x[] = new double[2];
					x[0] = iStartTouchPointX;
					x[1] = iStartTouchPointY;
					iFramework.convertCoords(x, Framework.SCREEN_COORDS,
							Framework.DEGREE_COORDS);
					setSelectedPoint(x[0], x[1]);

					iTouchCount = 0;

					if (iSimulatingNavigation)
						{
						// Additionally, use the point as a simulated location
						// for turn-by-turn navigation
						setSimulatedNavLocation(x[0], x[1]);
						}

					} else
					{
					iTouchCount = 0;
					iFramework.pan((int) Math.floor(-iXOffset + 0.5),
							(int) Math.floor(-iYOffset + 0.5));

					iXOffset = 0;
					iYOffset = 0;
					getMap();
					invalidate();
					}

				return true;
			}

		return false;
		}

	/*
	 * Several functions require the User to have selected a point on the map as
	 * a predicate. If not set, this asks the user to set a point and returns
	 * true.
	 */
	private boolean noSelectedPoint()
		{
		if (iInterestLong == 0.0 && iInterestLat == 0.0)
			{
			Notify("To use this function, please first select a point by pressing on the map.");
			return true;

			}
		return false;
		}

	/**
	 * Remove selected point, if any, from the display.
	 */
	private void deleteSelectedPoint()
		{
		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_SELECTED_POINT, ID_SELECTED_POINT, null);
		iInterestLong = 0.0;
		iInterestLat = 0.0;
		}

	/*
	 * Add a "selected point" to the display and remember its lat/lon for access
	 * by other methods.
	 */
	public void setSelectedPoint(double aLongitude, double aLatitude)
		{
		iInterestLong = aLongitude;
		iInterestLat = aLatitude;

		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_SELECTED_POINT, ID_SELECTED_POINT, null);
		iFramework.insertPointMapObject(
				iFramework.getMemoryMapHandle(),
				"pushpin", iInterestLong, iInterestLat,
				Framework.DEGREE_COORDS, "my selected point", 0,
				ID_SELECTED_POINT, true);
		
		getMap();
		invalidate();
		}

	/*
	 * For route display. Sets the navigation start point to the current
	 * selected point.
	 */
	public void setRouteStart()
		{
		if (noSelectedPoint())
			{
			return;
			}

		iRouteStartLong = iInterestLong;
		iRouteStartLat = iInterestLat;
		deleteSelectedPoint();
		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_ROUTE_START, ID_ROUTE_START, null);
		iFramework.insertPointMapObject(
				iFramework.getMemoryMapHandle(),
				"route_start", iRouteStartLong,
				iRouteStartLat, Framework.DEGREE_COORDS, "route start", 0,
				ID_ROUTE_START, true);
		
		if (iDisplayRoute)
			{
			// Reset existing route display
			iFramework.endNavigation();
			setDisplayRouteOn();
			} else if ((iRouteEndLong != 0 && iRouteEndLat != 0))
			{
			setDisplayRouteOn();
			}

		getMap();
		invalidate();

		}

	/**
	 * Route Start needs to be deleted when changing map.
	 */
	public void deleteRouteStart()
		{
		iRouteStartLong = 0.0;
		iRouteStartLat = 0.0;
		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_ROUTE_START, ID_ROUTE_START, null);

		}

	/**
	 * Route End needs to be deleted when changing map.
	 */
	public void deleteRouteEnd()
		{
		iRouteEndLong = 0.0;
		iRouteEndLat = 0.0;
		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_ROUTE_END, ID_ROUTE_END, null);

		}

	/*
	 * For route display. Sets the end point to the current selected point.
	 */
	public void setRouteEnd()
		{

		if (noSelectedPoint())
			{
			return;
			}

		iRouteEndLong = iInterestLong;
		iRouteEndLat = iInterestLat;
		deleteSelectedPoint();
		iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
				ID_ROUTE_END, ID_ROUTE_END, null);
		iFramework.insertPointMapObject(
				iFramework.getMemoryMapHandle(),
				"route_end", iRouteEndLong, iRouteEndLat,
				Framework.DEGREE_COORDS, "route end", 0, ID_ROUTE_END, true);
		
		if (iDisplayRoute)
			{
			// Reset existing route display
			iFramework.endNavigation();
			setDisplayRouteOn();
			} else if ((iRouteStartLong != 0 && iRouteStartLat != 0))
			{
			setDisplayRouteOn();
			}
		getMap();
		invalidate();

		}

	/*
	 * For route display. If both route start and end points are set, reverse
	 * them, re-calculate the route and display it. The route may be different
	 * due to one-way roads.
	 */
	public void reverseRoute()
		{
		if (iRouteEndLong != 0.0 && iRouteEndLat != 0.0
				&& iRouteStartLong != 0.0 && iRouteStartLat != 0.0)
			{
			double intermediate = iRouteEndLong;
			iRouteEndLong = iRouteStartLong;
			iRouteStartLong = intermediate;
			intermediate = iRouteEndLat;
			iRouteEndLat = iRouteStartLat;
			iRouteStartLat = intermediate;

			// Swap coloured markers labelling end and start points
			iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
					ID_ROUTE_START, ID_ROUTE_START, null);
			iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
					ID_ROUTE_END, ID_ROUTE_END, null);
			iFramework.insertPointMapObject(
					iFramework.getMemoryMapHandle(),
					"route_start", iRouteStartLong,
					iRouteStartLat, Framework.DEGREE_COORDS, "route start",
					0, ID_ROUTE_START, true);
			iFramework.insertPointMapObject(
					iFramework.getMemoryMapHandle(),
					"route_end", iRouteEndLong, iRouteEndLat,
					Framework.DEGREE_COORDS, "route end", 0, ID_ROUTE_END, true);

			if (iDisplayRoute)
				{
				// Reset existing route display
				iFramework.endNavigation();
				setDisplayRouteOn();
				} else if ((iRouteStartLong != 0 && iRouteStartLat != 0))
				{
				setDisplayRouteOn();
				}
			getMap();
			invalidate();

			}
		}

	public void onProviderDisabled(String arg0)
		{
		// Auto-generated method stub. Not needed.

		}

	public void onProviderEnabled(String arg0)
		{
		// Auto-generated method stub. Not needed.

		}

	public void onStatusChanged(String arg0, int arg1, Bundle arg2)
		{
		// Auto-generated method stub. Not needed.

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
		iTouchCount = 0;

		if (iXOffset != 0 || iYOffset != 0)
			iFramework.pan((int) Math.floor(-iXOffset + 0.5),
					(int) Math.floor(-iYOffset + 0.5));
		iXOffset = 0;
		iYOffset = 0;

		getMap();
		invalidate();
		}

	/**
	 * Simple routine for displaying a short-lived message to the User.
	 * 
	 * @param aMessage
	 */
	public void Notify(String aMessage)
		{
		Toast.makeText(getContext(), aMessage, Toast.LENGTH_SHORT).show();
		}

	/**
	 * Returns the file name of the current map being used. E.g.
	 * "/sdcard/CartoType/maps/stockholm.ctm1"
	 * 
	 * @return
	 */
	public String getMapName()
		{
		return iMapFile;
		}

	/**
	 * Change map. The map file is set to that specified and the Framework
	 * object is re-initialised to load it.
	 */
	public void setMapName(String aMapFile)
		{
		iMapFile = aMapFile;
		saveState();
		// We need to remove at least the lat, lon and desirably scale in the
		// saved state
		// ... they are irrelevant now.
		deleteState();
		Log.v(TAG, "Attempting to display new map " + iMapFile);
		iFramework = null;
		this.init();
		}

	/**
	 * Returns map scale denominator. E.g. if scale is 1:50,000 then 50000 is
	 * returned.
	 * 
	 * @return
	 */
	public int getMapScale()
		{

		try
			{
			// Occasionally happens on start with Null pointer. iFramework not
			// initialised?
			// Also happens if no map can be found.
			return iFramework.getScale();
			} catch (NullPointerException e)
			{
			// TODO Possibly a little dangerous, may hide a real bug
			return 100000;
			}
		}

	/**
	 * Returns the map rotation in degrees. North=0
	 * 
	 * @return
	 */
	public double getMapRotation()
		{
		try
			{
			return iFramework.getRotation();
			} 
		catch (NullPointerException e)
			{
			// Can happen on start with menu initialisation. Android 
			// (particularly older versions) may do 
			// an onPause()/onResume when iFramework not initialised.
			return 0.0;
			}
		}

	/**
	 * Are we in Perspective mode?
	 * 
	 * @return boolean
	 */
	public boolean getMapPerspective()
		{
		try
			{

			return iFramework.getPerspective();
			} catch (NullPointerException e)
			{
			// Happens on start with Android 3.0
			return false;
			}
		}

	/**
	 * Return current map lat and lon centre point.
	 * 
	 * @param aPoint; aPoint[0] = lon, aPoint[1] = lat
	 */
	public void getMapPosition(double aPoint[])
		{
		try
			{
			iFramework.getViewDimensions(aPoint, Framework.DEGREE_COORDS);
			} catch (NullPointerException e)
			{
			// Happens on start with Android 3.0
			aPoint[0] = 0.0;
			aPoint[1] = 0.0;
			}
		}

	/**
	 * Persist basic parameters about a User's session, (map file name, scale,
	 * centre location, etc), so that the same view can be restored at the next
	 * view. In the Android life-cycle, this is particularly important as
	 * hitting the Android Home or Menu button and then returning to the App
	 * later will cause settings to be lost.
	 */
	public void saveState()
		{

		SharedPreferences prefs = context.getSharedPreferences(
				((NavigatorAppActivity) context).iPrefsName,
				Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = prefs.edit();

		// Save map name, position etc when screen is rotated, User
		// hits Home or Back button or Android closes app.
		editor.putInt(SAVE_MAP_SCALE, getMapScale());
		double[] aPoint = new double[2];
		getMapPosition(aPoint);
		editor.putLong(SAVE_MAP_LON, Double.doubleToLongBits(aPoint[0]));
		editor.putLong(SAVE_MAP_LAT, Double.doubleToLongBits(aPoint[1]));
		editor.putBoolean(SAVE_MAP_PERSPECTIVE, getMapPerspective());
		editor.putBoolean(SAVE_MAP_TERRAIN, iDisplayTerrain);
		editor.putLong(SAVE_MAP_ROTATION,
				Double.doubleToLongBits(getMapRotation()));
		editor.putString(SAVE_MAP_NAME, getMapName());
		editor.putString(SAVE_DISTANCE_UNITS, iDistanceUnits);

		editor.commit();
		}

	/**
	 * Used when the User wants to change map. Scale, rotation, lat, lon are
	 * irrelevant and are removed.
	 */
	public void deleteState()
		{

		SharedPreferences prefs = context.getSharedPreferences(
				((NavigatorAppActivity) context).iPrefsName,
				Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = prefs.edit();

		editor.remove(SAVE_MAP_SCALE);
		editor.remove(SAVE_MAP_LON);
		editor.remove(SAVE_MAP_LAT);
		editor.remove(SAVE_MAP_ROTATION);

		editor.commit();
		}

	/**
	 * Get the name of the map file previously used by the User. Set default if
	 * none exists.
	 */
	public void restoreSavedMapFile()
		{

		SharedPreferences prefs = context.getSharedPreferences(
				((NavigatorAppActivity) context).iPrefsName,
				Context.MODE_PRIVATE);
		iMapFile = prefs.getString(SAVE_MAP_NAME, iDefaultMapFile);
		}

	/**
	 * Get any map centre, scale and toggle values that the User save previously
	 * and restore them. If there are previous values, apply defaults.
	 */
	public void restoreState()
		{

		SharedPreferences prefs = context.getSharedPreferences(
				((NavigatorAppActivity) context).iPrefsName,
				Context.MODE_PRIVATE);

		int map_scale = 0;
		double map_rotation = 0;
		double map_lat = 0.0;
		double map_lon = 0.0;
		try
			{

			iDistanceUnits = prefs.getString(SAVE_DISTANCE_UNITS, "metric");
			// Get back last map position, scale, toggle values etc
			iDisplayTerrain = prefs.getBoolean(SAVE_MAP_TERRAIN, false);
			map_scale = prefs.getInt(SAVE_MAP_SCALE, 0);
			map_rotation = Double.longBitsToDouble(prefs.getLong(
					SAVE_MAP_ROTATION, 0));
			iPerspective = prefs.getBoolean(SAVE_MAP_PERSPECTIVE, false);
			map_lat = Double.longBitsToDouble(prefs.getLong(SAVE_MAP_LAT, 0));
			map_lon = Double.longBitsToDouble(prefs.getLong(SAVE_MAP_LON, 0));
			} catch (ClassCastException e)
			{
			Log.e(TAG,
					"Previous user values not read in properly, possible corrupt, setting all to default.");
			e.printStackTrace();

			}
		Log.v(TAG, "getSavedMapValues() Lat=" + map_lat + " Lon=" + map_lon
				+ " Scale=1:" + map_scale + " Rotation=" + map_rotation
				+ " Pespective=" + iPerspective);

		// Set position, scale, rotation, perspective
		if (map_scale != 0)
			{
			Log.v(TAG, "init() Using last scale " + map_scale);
			iFramework.setScale(map_scale);
			} else
			{
			Log.v(TAG, "init() Using default scale");
			iFramework.setScale(2500000);
			}

		// Enable terrain shading (depends on the map being loaded whether this
		// is available)
		iFramework.enableLayer("terrain-height-feet", iDisplayTerrain);
		iFramework.enableLayer("terrain-shadow", iDisplayTerrain);

		if (iPerspective)
			{
			iFramework.setPerspective(true);
			}
		if (map_rotation != 0)
			{
			Log.v(TAG, "init() Using last rotation: " + map_rotation);
			iFramework.setRotation(map_rotation);
			}
		if (map_lat != 0 && map_lon != 0)
			{
			Log.v(TAG, "init() Using last map centre: Lat=" + map_lat + " Lon="
					+ map_lon);
			iFramework.setViewCenterLatLong(map_lon, map_lat);
			}
		}

	/**
	 * Toggle real turn-by-turn navigation. i.e use devices actual location.
	 * (This is not useful unless in the demo map area. Use simulated navigation
	 * otherwise.
	 */
/*
	/**
	 * Toggle real turn-by-turn navigation. i.e use devices actual location.
	 * This is not useful unless in the map area!
	 */
	public void toggleNavigation()
		{
		if (iNavigateOn )
			{
			NavigateStop();
			} 
		else
			{
			NavigateStart();
			}
		}

	/**
	 * When simulating navigation, a route is calculated between the last two
	 * points touched. Further touches provide a simulated vehicle position.
	 */
	public void toggleSimulateNavigation()
		{
		if (iSimulatingNavigation)
			{
			iFramework.endNavigation();
			iSimulatingNavigation = false;

			showNav_destroy();
			} else
			{
			if (iRouteStartLong == 0 || iRouteStartLat == 0
					|| iRouteEndLong == 0 || iRouteEndLat == 0)
				{
				iNavMessage1 = "Cannot start simulating navigation: positions not known. Please touch two positions and try again";
				return;
				}

			int result = iFramework.startNavigation(iRouteStartLong,
					iRouteStartLat, Framework.DEGREE_COORDS, iRouteEndLong,
					iRouteEndLat, Framework.DEGREE_COORDS);

			if (result != 0)
				iNavMessage1 = "Could not start simulated navigation: error = "
						+ result + ".";
			else
				{
				iSimulatingNavigation = true;
				iWantNavigation = false;
				iNavigateStarted = false;			
				iNavMessage1 = "Starting simulated navigation, touch map to simulate next vehicle location.";
				iNavMsgSecondTurn = null;
				iNavMsgTimeLeft = null;
				iNavMsgStreetTo = null;
				iNavMsgStreetFrom = null;
				iNavMsgFirstTurnType = Turn.TURN_NONE;
				iFirstTurnDistStr = "";
				}

			showNav();
			}

		getMap();
		invalidate();
		}

	/**
	 * Use specified point as a simulated location for turn-by-turn navigation
	 * 
	 * @param x
	 * @param y
	 */
	public void setSimulatedNavLocation(double x, double y)
		{

		Time t = new Time();
		double seconds = (double) t.toMillis(true) / 1000.0;
		Navigate(Framework.POSITION_VALID | Framework.TIME_VALID, seconds, x,
				y, 0, 0, 0);

		}

	/**
	 * Toggle real turn-by-turn navigation. i.e use devices actual location.
	 * This is not useful unless in the map area!
	 */
	public void NavigateStart()
		{
				
		if (iRouteEndLong == 0 && iRouteEndLat == 0)
			{
			Notify("Cannot start navigation: destination not known. Please set the destination by touching the map and try again.");
			iNavigateOn = false;
			return;
			}
		
		iNavigateOn = true;
		
		if (!iNavigateStarted)
			{
					
			if (iIsGPSOn)
				{
				//Log.d(TAG, "TEMPP toggleNavigation GPS is on");
				if (iCurLong == 0 || iCurLat == 0)
					{
					Notify("Waiting for a current GPS position fix.");
					iWantNavigation = true;
					return;
					}
				} else
				{

				// Switch on navigation when we get our first fix
				//Log.d(TAG, "TEMPP NavigateStart GPS is off");
				iWantNavigation = true;
				toggleGPS(true);
				return;
				}
			
			// Make sure GPS device is really on, we may have a stale value.
			toggleGPS(true);
			
			// If we get here we have a valid (but possibly state) position

			// Note: We get an error if Navigation is already on
			int result = iFramework.startNavigation(iCurLong, iCurLat,
					Framework.DEGREE_COORDS, iRouteEndLong, iRouteEndLat,
					Framework.DEGREE_COORDS);

			if (result != 0)
				{
				Notify("Could not start navigation: error = " + result + ".");

				return;
				} else
				{
				iNavigateStarted = true;
				iNavMessage1 = "Waiting for GPS fix ...";
				iNavMsgSecondTurn = null;
				iNavMsgTimeLeft = null;
				iNavMsgStreetTo = null;
				iNavMsgStreetFrom = null;
				iNavMsgFirstTurnType = Turn.TURN_NONE;
				iFirstTurnDistStr = "";

				showNav();
				}
			}

		getMap();
		invalidate();
		}

	/**
	 * Toggle real turn-by-turn navigation. i.e use devices actual location.
	 * This is not useful unless in the map area!
	 */
	public void NavigateStop()
		{
		
		iNavigateOn = false;
		Notify("Turning navigation off.");
		if (iNavigateStarted)
			{
			iFramework.endNavigation();
			iFramework.displayRoute(false);
			iNavigateStarted = false;
			iNavMessage1 = "NOT NAVIGATING";
			iNavMsgSecondTurn = null;
			iNavMsgTimeLeft = null;
			iCurLong = 0;
			iCurLat = 0;
			

			showNav_destroy();
			
			// Set north at top, else User a left with a random orientation
			// TODO: May be pre-navigate orientation should be saved
			this.setRotation(0.0);
			} 
		if (iIsGPSOn) {
			// Switch off GPS polling to save battery
			Notify("Turning off GPS requests to save your battery.");
			toggleGPS(false);
			}
		}

	/**
	 * Display turn-by-turn navigation instructions to the User.
	 */
	public void showNav()
		{

		// initialise if necessary
		if (iShowNavPopup == null)
			{
			showNav_init();
			}

		// Set turn image to display
		ImageView image = ((ImageView) iShowNavPopup.getContentView()
				.findViewById(R.id.turn_by_turn_imageTurnType));
		switch (iNavMsgFirstTurnType)
			{
			case Turn.TURN_NONE:
				image.setImageResource(R.drawable.turn0);
				break;
			case Turn.TURN_AHEAD:
				image.setImageResource(R.drawable.turn1);
				break;
			case Turn.TURN_BEAR_RIGHT:
				image.setImageResource(R.drawable.turn2);
				break;
			case Turn.TURN_RIGHT:
				image.setImageResource(R.drawable.turn3);
				break;
			case Turn.TURN_SHARP_RIGHT:
				image.setImageResource(R.drawable.turn4);
				break;
			case Turn.TURN_AROUND:
				image.setImageResource(R.drawable.turn5);
				break;
			case Turn.TURN_SHARP_LEFT:
				image.setImageResource(R.drawable.turn6);
				break;
			case Turn.TURN_LEFT:
				image.setImageResource(R.drawable.turn7);
				break;
			case Turn.TURN_BEAR_LEFT:
				image.setImageResource(R.drawable.turn8);
				break;
			}

		// Set the text to display

		if (iNavMsgSecondTurn == null)
			((TextView) iShowNavPopup.getContentView().findViewById(
					R.id.turn_by_turn_Message1)).setText(iNavMessage1);
		else
			{
			if (iNavMessage1 == null)
				((TextView) iShowNavPopup.getContentView().findViewById(
						R.id.turn_by_turn_Message1)).setText(iNavMsgSecondTurn);
			else
				((TextView) iShowNavPopup.getContentView().findViewById(
						R.id.turn_by_turn_Message1)).setText(iNavMessage1
						+ "\n" + iNavMsgSecondTurn);
			}

		((TextView) iShowNavPopup.getContentView().findViewById(
				R.id.turn_by_turn_StreetTo)).setText(iNavMsgStreetTo);
		((TextView) iShowNavPopup.getContentView().findViewById(
				R.id.turn_by_turn_StreetFrom)).setText(iNavMsgStreetFrom);

		((TextView) iShowNavPopup.getContentView().findViewById(
				R.id.turn_by_turn_FirstTurnDist)).setText(iFirstTurnDistStr);

		((TextView) iShowNavPopup.getContentView().findViewById(
				R.id.turn_by_turn_TimeLeft)).setText(iNavMsgTimeLeft);

		((TextView) iShowNavPopup.getContentView().findViewById(
				R.id.turn_by_turn_DistLeft)).setText(iNavMsgDistLeft);

		iShowNavPopup.showAtLocation(this, Gravity.BOTTOM, 0, 20);
		}

	/**
	 * Initialise Turn-by-turn navigation PopupWindow
	 */
	public void showNav_init()
		{

		// Creates a grey semi-transparent pop-up at the bottom of the screen

		// Get layout from XML layout file
		final LayoutInflater factory = (LayoutInflater) context
				.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		final View layout = factory.inflate(R.layout.turn_by_turn, null);

		int x = (int) Math.floor((double) this.getWidth() * 0.9);
		// Start a pop-up screen
		int y = (int) Math.floor((double) this.getHeight() / 3);
		iShowNavPopup = new PopupWindow(this, x, y);

		layout.setBackgroundColor(Color.DKGRAY);
		layout.getBackground().setAlpha(150);
		iShowNavPopup.setContentView(layout);

		// Allow panning/zooming of the map underneath
		iShowNavPopup.setFocusable(false);
		iShowNavPopup.setTouchable(false);

		}

	/**
	 * Remove Turn-by-turn navigation PopupWindow
	 */
	public void showNav_destroy()
		{
		// iShowNavPopup.showAsDropDown(anchor);
		iShowNavPopup.dismiss();

		iNavMessage1 = "NOT NAVIGATING";
		iNavMsgStreetFrom = "";
		iNavMsgStreetTo = "";
		iNavMsgTimeLeft = "";
		iNavMsgDistLeft = "";
		iFirstTurnDistance = 0;
		;
		iFirstTurnDistStr = "";
		iNavMsgFirstTurnType = 0;
		iNavMsgSecondTurn = "";
		}

	/**
	 * Update the navigation state using location data. The native interface
	 * code then adjusts the map position, orientation and scale.
	 * 
	 * @param aValidity
	 * @param aTime
	 * @param aLong
	 * @param aLat
	 * @param aSpeed
	 * @param aBearing
	 * @param aHeight
	 */
	public void Navigate(int aValidity, double aTime, double aLong,
			double aLat, double aSpeed, double aBearing, double aHeight)
		{
		iFramework.navigate(aValidity, aTime, aLong, aLat, aSpeed, aBearing,
				aHeight);

		iNavigationState = iFramework.getNavigationState();

		// Return true if the position is known.
		// iPositionKnown = iFramework.positionKnown();

		// Return the distance from the current point on the route to the
		// destination in meters.
		double distance = iFramework.distanceToDestination();
		iNavMsgDistLeft = formatDistance(distance);

		// Return estimated time from the current point on the route to the
		// destination in seconds.
		double seconds = iFramework.estimatedTimeToDestination();
		// Now, format as HH:MM:SS to destination
		double hours = (int) (seconds / 3600);
		seconds -= hours * 3600;
		double minutes = (int) (seconds / 60);
		seconds -= minutes * 60;
		iNavMsgTimeLeft = String.format("%02d", (int) hours) + ":"
				+ String.format("%02d", (int) minutes) + ":"
				+ String.format("%02d", (int) seconds);

		iFirstTurn = new Turn();
		iSecondTurn = new Turn();
		iFirstTurnDistance = iFramework.getFirstTurn(iFirstTurn);
		iSecondTurnDistance = iFramework.getSecondTurn(iSecondTurn);

		iNavMsgSecondTurn = null;
		iNavMsgStreetTo = null;
		iNavMsgStreetFrom = null;
		iNavMsgFirstTurnType = Turn.TURN_NONE;
		iFirstTurnDistStr = "";

		switch (iNavigationState)
			{
			case NavigationState.NO_ACTION:
				break;

			case NavigationState.TURN:

				iNavMessage1 = iFirstTurn.iInstructions;
				iNavMsgFirstTurnType = iFirstTurn.iTurnType;
				if (iSecondTurn.iTurnType != Turn.TURN_NONE)
					iNavMsgSecondTurn = "THEN: " + iSecondTurn.iInstructions;
				break;

			case NavigationState.TURN_ROUND:
				iNavMessage1 = "Turn round at the next safe and legal opportunity";
				break;

			case NavigationState.NEW_ROUTE:
				iNavMessage1 = "Calculating new route";
				break;

			// This state occurs when the end of the route is within one
			// kilometre
			// and there are no more junctions.
			case NavigationState.ARRIVAL:
				iFirstTurnDistance = iFramework.getFirstTurn(iFirstTurn);
				iNavMessage1 = formatDistance(iFirstTurnDistance)
						+ " to destination";
				break;

			case NavigationState.OFF_ROUTE:
				iNavMessage1 = "Off route";
				break;
			}

		showNav();

		getMap();
		invalidate();
		}

	/**
	 * Returns a string in en_gb describing road type. The list is not complete
	 * and could be expanded. In this demo we are only using it to give more
	 * info for ways that are typically unnamed. HACK: Using int for road type.
	 * 
	 * @param iRoadType
	 * @return
	 */
	private String getRoadType(int iRoadType)
		{

		// OSM motorway_slip
		String type = "";
		if ((iRoadType & RoadType.RAMP_ROAD_TYPE_FLAG) != 0)
			{
			// Any kind of slip road (e.g. OpenStreetMap motorway, primary,
			// trunk, ...
			// slip roads typically but not always have no name or ref)
			type = "slip road";
			} else if ((iRoadType & RoadType.SERVICE_ROAD_TYPE) != 0)
			{
			type = "service road";
			} else if ((iRoadType & RoadType.BYWAY_ROAD_TYPE) != 0)
			{
			type = "track";
			} else if ((iRoadType & RoadType.VEHICULAR_FERRY_ROAD_TYPE) != 0)
			{
			type = "ferry";
			} else
			{
			type = "";
			}

		return type;
		}

	/**
	 * Returns a formatted string of distance with appropriate units and
	 * resolution. Ready for adding conversion to imperial units if set in User
	 * preferences.
	 * 
	 * @param distance
	 *            in metres
	 * @return
	 */
	public String formatDistance(double distance)
		{

		String distanceString;

		if (iDistanceUnits.equals("metric"))
			{
			if (distance > 10000.0)
				distanceString = (int) (distance / 1000.0) + "km";
			else if (distance > 1000.0)
				{
				distanceString = String.format("%.2f", (distance / 1000.0))
						+ "km";
				} else
				distanceString = (int) distance + "m";
			} else
			{

			distance *= 0.000621371;
			if (distance > 10.0)
				distanceString = (int) (distance) + " miles";
			else
				distanceString = String.format("%.2f", distance) + " miles";
			}

		return distanceString;

		}

	/**
     * User wants to know current GPS location right now.
     */
    public void whereAmI()
    	{
    	
	    iWantWhereAmI = true;
		if (iCurLong != 0 && iCurLat != 0) {
		
			// Centre map on "current location".
			// Note that this may be stale, particularly if GPS has been switched off
			iFramework.setViewCenterLatLong(iCurLong, iCurLat);
		}

		// Switch on GPS if not already on
		toggleGPS(true);
    	
		// If we already have a fix, display it (otherwise onLocationChanged() will handle it)
    	displayCurrentLocation();
   	
    	}
    
	/**
	 * Add a  current location icon to the display.
	 */
	public void displayCurrentLocation()
		{
		if (iCurLong != 0 && iCurLat != 0 && iFramework != null)
			{
			iFramework.deleteMapObjects(iFramework.getMemoryMapHandle(),
					ID_CURRENT_LOCATION, ID_CURRENT_LOCATION, null);
			iFramework.insertPointMapObject(
					iFramework.getMemoryMapHandle(),
					"route-position", iCurLong, iCurLat,
					Framework.DEGREE_COORDS, "current location", 0,
					ID_CURRENT_LOCATION, true);
			getMap();
			invalidate();
			}
		}
	
    /** 
     * Switch GPS on and off.
     * Polling for GPS fixes is very draining and not necessary if the User
     * is not actually physically in the map area.
     * 
     * @param state - True for on, false for off
     */
    public void toggleGPS(boolean state)
    	{
    	
    	if (! iIsGPSOn && state)
    		{
    		// Supply GPS fixes to the map view.
    		//Log.d(TAG, "TEMPP toggleGPS to on"); 
    		
    		LocationManager locationManager = (LocationManager)context.getSystemService(Context.LOCATION_SERVICE);
    		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,3000,10,this);
    		boolean isGPSEnabled = locationManager.isProviderEnabled (LocationManager.GPS_PROVIDER);
    		if (isGPSEnabled) {
    			Notify("Switching your GPS device on, it may take a while to get a fix.");
    			iIsGPSOn = true;
	    		} 
    		else 
	    		{
	    			PackageManager pm = context.getPackageManager();
	    			boolean hasGps = pm.hasSystemFeature(PackageManager.FEATURE_LOCATION_GPS);
	    			if (hasGps)
	    				{
	    				Notify("Your GPS is disabled. You cannot do Turn-by-turn navigation or Where Am I until you enable it.");
	    				}
	    			else
	    				{
	    				Notify("You have no GPS on your device. Sorry cannot do Turn-by-turn navigation or Where Am I.");
	    				}
	    			
	    		}
    		}
    	else if (iIsGPSOn && ! state) 
    		{
    		
    		LocationManager locationManager = (LocationManager)context.getSystemService(Context.LOCATION_SERVICE);
    		locationManager.removeUpdates(this);
    		iIsGPSOn = false;
    	}
    	}
    
	/**
	 * For getting and doing something with GPS or NETWORK fixes. Implements
	 * inherited abstract onLocationChanged(Location)
	 */
	public void onLocationChanged(Location aLocation)
		{

		Log.d(TAG, "location update " + iCurLong + " " + iCurLat);
		// if (iCurLong == aLocation.getLongitude() && iCurLat ==
		// aLocation.getLatitude())
		// return;

		boolean location_was_unknown = iCurLong == 0;

		iCurLong = aLocation.getLongitude();
		iCurLat = aLocation.getLatitude();

		displayCurrentLocation();

		if (location_was_unknown)
			Notify("Location now known");

		if (iWantNavigation)
			{
				// User wanted to started navigation, but there was no  fix
				this.NavigateStart();
				iWantNavigation = false;
			}
		
		if (iNavigateStarted)
			{
			// Get the validity flags; position and time are always valid if we
			// get a fix.
			int validity = Framework.POSITION_VALID | Framework.TIME_VALID;
			if (aLocation.hasSpeed())
				validity |= Framework.SPEED_VALID;
			if (aLocation.hasBearing())
				validity |= Framework.COURSE_VALID;
			if (aLocation.hasAltitude())
				validity |= Framework.HEIGHT_VALID;

			double time = aLocation.getTime();
			Navigate(validity, time / 1000.0, // convert time to seconds
					iCurLong, iCurLat, aLocation.getSpeed() * 3.6, // convert
																	// speed
																	// from
																	// metres
																	// per
																	// second to
																	// kilometres
																	// per hour
					aLocation.getBearing(), aLocation.getAltitude());
			} else if (iWantWhereAmI) {
				// We have a fix, so can satisfy this one-off request now
		    	// Centre map on new current location
		    	iFramework.setViewCenterLatLong(iCurLong, iCurLat);
		    	iWantWhereAmI = false;
		    	displayCurrentLocation();
			}
		}

	private Framework iFramework;
	private Bitmap iBitmap;
	private Legend iLegend;
	private Bitmap iLegendBitmap;
	private int iScreenDpi = 160;
	private ScaleGestureDetector iScaleGestureDetector;
	private boolean iPerspective;
	private boolean iDisplayTerrain = true;
	private int iTouchCount;
	private float iStartTouchPointX;
	private float iStartTouchPointY;
	private float iPrevScaleFocusX;
	private float iPrevScaleFocusY;
	private float iXOffset;
	private float iYOffset;
	private float iScale = 1;
	private Matrix iPlainMatrix;
	private boolean iDisplayRoute = false;
	private double iInterestLong;
	private double iInterestLat;
	private double iRouteStartLong;
	private double iRouteStartLat;
	private double iRouteEndLong;
	private double iRouteEndLat;

	// For GPS fixes
	/** Is GPS "on", i.e. we have programmatically requested GPS updates?*/
	public boolean iIsGPSOn = false;
	/** User wants current location map at next update */
	private boolean iWantWhereAmI;
	/** User wanted to started navigation, but there was no fix */
	private boolean iWantNavigation = false;
	private double iCurLong;
	private double iCurLat;

	// For turn-by-turn navigation
	// User wants turn-by-turn navigation (whether it has started or not)
	public boolean iNavigateOn = false;
	//private boolean iNavigating = false;
	//  turn-by-turn navigation has physically started/not started
	public boolean iNavigateStarted = false;
	private boolean iSimulatingNavigation = false;
	private int iNavigationState;
	private String iNavMessage1 = "NOT NAVIGATING";
	private String iNavMsgStreetFrom = "";
	private String iNavMsgStreetTo = "";
	private String iNavMsgTimeLeft;
	private String iNavMsgDistLeft;
	private Turn iFirstTurn = new Turn();
	public String iDistanceUnits = "";
	private double iFirstTurnDistance;
	private String iFirstTurnDistStr = "";
	private int iNavMsgFirstTurnType = 99;
	private Turn iSecondTurn = new Turn();
	private double iSecondTurnDistance;
	private String iNavMsgSecondTurn;

	private PopupWindow iShowNavPopup = null;

	// Keep User's general search text and results between searches.
	private boolean iIsFirstShowFind = true;
	private PopupWindow iFindPopup = null;
	private EditText iFindSearchField = null;
	private Button iFindButtonClearText = null;

	// Keep User's address search text and results between searches.
	private boolean iIsFirstShowFindAddr = true;
	private PopupWindow iFindAddrPopup = null;
	private EditText iFindAddrSearchField1 = null;
	private EditText iFindAddrSearchField2 = null;
	private EditText iFindAddrSearchField3 = null;
	private EditText iFindAddrSearchField4 = null;
	private EditText iFindAddrSearchField5 = null;
	private EditText iFindAddrSearchField6 = null;
	private EditText iFindAddrSearchField7 = null;
	private EditText iFindAddrSearchField8 = null;
	private RelativeLayout iFindAddrForm = null;
	private Button iFindAddrButtonClearText = null;
	private Button iFindAddrButtonSearchAgain = null;

	// For saving state
	private static final String SAVE_MAP_SCALE = "last_scale";
	private static final String SAVE_MAP_LON = "last_lon";
	private static final String SAVE_MAP_LAT = "last_lat";
	private static final String SAVE_MAP_PERSPECTIVE = "last_perspective";
	private static final String SAVE_MAP_ROTATION = "last_rotation";
	private static final String SAVE_MAP_NAME = "last_map";
	private static final String SAVE_MAP_TERRAIN = "last_terrain";
	private static final String SAVE_DISTANCE_UNITS = "display_units";

	}
