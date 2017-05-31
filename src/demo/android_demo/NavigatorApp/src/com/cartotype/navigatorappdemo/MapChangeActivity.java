package com.cartotype.navigatorappdemo;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.NavUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

/**
An Activity to show a list of maps in the sdcard CartoType/map directory.
If one is selected it is swapped with the current map and displayed.
 */
public class MapChangeActivity extends Activity
	{

	// For Android Menu button items
	private static final int MENU_MORE_MAPS = 10;

	// For logging
	private static final String TAG = "MapChanger";

	private ListView listViewMapFile;
	private Context ctx;

	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@Override
	public void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);
		ctx = this;

		// Show a list of maps in the sdcard CartoType/map directory
		// As a ListView with a Select button next to each one.
		setContentView(R.layout.activity_map_change);
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB)
			{
			// Not available in Android 2.2, added in API 11
			getActionBar().setDisplayHomeAsUpEnabled(true);
			}

		File fileList = new File(Environment.getExternalStorageDirectory()
				.getPath() + "/CartoType/map");
		List<MapFile> listMap = new ArrayList<MapFile>();
		if (fileList != null)
			{
			File[] filenames = fileList.listFiles();
			for (File fileName : filenames)
				{
				listMap.add(new MapFile(fileName.getName()));
				}
			}

		listViewMapFile = (ListView) findViewById(android.R.id.list);
		listViewMapFile.setAdapter(new FileListAdapter(ctx,
				R.layout.activity_map_change_row_item, listMap));
		}

	// MapFile object
	public class MapFile
		{
		private String fileName; // the map's file name

		public OnClickListener listener = new OnClickListener()
			{ 
				@Override
				public void onClick(View v)
					{
					// Send the file name back to the calling Activity and exit
					
					Log.d(TAG,"User wants to change map to "+fileName);
					
					// Use Android Intent to send data (the new map) back to parent
					Intent resultIntent = new Intent();
					// TODO Hard-coded folder name
					resultIntent.putExtra("new_map", Environment.getExternalStorageDirectory()
							.getPath() + "/CartoType/map/"+fileName); 
					setResult(Activity.RESULT_OK, resultIntent);
					finish();
					}
			};

		public MapFile(String aFileName)
			{
			super();
			this.fileName = aFileName;
			}

		public String getFileName()
			{
			return this.fileName;
			}

		public void setFileName(String nameText)
			{
			this.fileName = nameText;
			}
		}

	/**
	 Built with hints from
	 "Tutorial: ListView in Android using custom ListAdapter and Cache View" ,
	 Fabio Tuzza, http://www.framentos.com/en/android-tutorial/2012/07/16
	 /listview-in-android-using-custom-listadapter-and-viewcache/
	 */
	private class FileListAdapter extends ArrayAdapter<MapFile>
		{
		private int resource;
		private LayoutInflater inflater;
		//private Context context;

		public FileListAdapter(Context ctx, int resourceId,
				List<MapFile> objects)
			{

			super(ctx, resourceId, objects);
			resource = resourceId;
			inflater = LayoutInflater.from(ctx);
			//context = ctx;
			}

		@SuppressLint("ViewHolder")
		@Override
		public View getView(int position, View convertView, ViewGroup parent)
			{

			/* create a new view of my layout and inflate it in the row */
			// This should use a view Holder in a production version
			// See, for example:
			//http://stackoverflow.com/questions/19289812/findviewbyid-vs-view-holder-pattern-in-listview-adapter
			convertView = (RelativeLayout) inflater.inflate(resource, null);

			/* Extract the city's object to show */
			MapFile mapFile = (MapFile) getItem(position);

			/* Take the TextView from layout and set the map file name */
			TextView txtName = (TextView) convertView
					.findViewById(R.id.maplist_file_name);
			txtName.setText(mapFile.getFileName());

			Button button = (Button) convertView
					.findViewById(R.id.map_change_button);
			button.setOnClickListener(mapFile.listener);

			return convertView;
			}
		}

	@Override
	public boolean onCreateOptionsMenu(Menu aMenu)
		{
		getMenuInflater().inflate(R.menu.activity_map_change, aMenu);

		aMenu.add(0, MENU_MORE_MAPS, 0, "Get More Maps ...");

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
			case MENU_MORE_MAPS:
				// Start a new Activity screen
				Intent myIntent1 = new Intent(MapChangeActivity.this,
						MapDownloadActivity.class);
				MapChangeActivity.this.startActivity(myIntent1);
				return true;
			}
		return super.onOptionsItemSelected(item);
		}

	}
