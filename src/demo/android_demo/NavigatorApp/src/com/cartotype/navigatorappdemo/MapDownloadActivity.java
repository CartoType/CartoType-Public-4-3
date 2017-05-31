package com.cartotype.navigatorappdemo;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import org.jsoup.Connection;
import org.jsoup.HttpStatusException;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import android.annotation.TargetApi;
import android.app.Dialog;
import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.Context;
import android.net.Uri;
import android.os.AsyncTask;
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
Display a screen showing list of .ctm1 map files found on a remote website
URL. The User can then select one for download.
 
Note: The web server must allow directory browsing for this to work. It may
also be possible to put anchor links to the files in an index.html 
 */
public class MapDownloadActivity extends ListActivity
	{

	private ListView listViewMapFile;
	private Context ctx;

	// A web server folder containing 1 or more .ctm1 CartoType map files
	//private static final String mapUrl = "http://192.168.9.35/cartotype_maps";
	private static final String mapUrl =
	 "http://www.cartotype.com/assets/downloads/maps";

	// Display status info here
	private TextView textView;

	// For display User feedback on download progress
	public static final int DIALOG_DOWNLOAD_PROGRESS = 0;
	private ProgressDialog mProgressDialog;

	// For logging
	private static final String TAG = "MapDownload";

	@TargetApi(Build.VERSION_CODES.HONEYCOMB)
	@Override
	public void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);

		ctx = this;

		setContentView(R.layout.activity_download_list);
		if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB)
			{
			// Not available in Android 2.2, added in API 11
			getActionBar().setDisplayHomeAsUpEnabled(true);
			}

		// Download links to .ctm1 maps and display
		// (Network tasks must be done outside the UI thread)
		DownloadMapList task = new DownloadMapList();
		task.execute(mapUrl);

		}

	/**
	Gets the specified URL and parses it for any links to Cartotype maps
	(i.e. anything with a .ctm1 suffix)
 
	Requires open source third-party jsoup HTML parser from http://jsoup.org/
	  
	Executed as an Async process to avoid NetworkOnMainThreadException
	 
	Reference:
	http://developer.android.com/reference/android/os/AsyncTask.html
	  
	@param aUrls
	*/
	private class DownloadMapList extends AsyncTask<String, Void, Elements>
		{

		String connectionError = "";
		
		@Override
		protected Elements doInBackground(String... aUrls)
			{
			Elements links = null;
			try
				{
				// Find out if a connection fails and inform User
				Connection.Response response = Jsoup.connect(aUrls[0])
						.timeout(10000).execute();

				int statusCode = response.statusCode();
				if (statusCode == 200)
					{
					Document doc = Jsoup.connect(aUrls[0]).timeout(10000).get();
					links = doc.select("a[href]:contains(.ctm1)"); // a with
																	// href and
																	// .ctm1 in
																	// the URI
					} else
					{
					Log.e(TAG, "received error code : " + statusCode);
					
					//We cannot communicate this directly as we are not in the UI thread
					connectionError = "ERROR connecting to " + mapUrl
							+ ", Status =" + statusCode + ", "
							+ response.statusMessage();
					}

				} catch (SocketException e)
				{
				Log.e(TAG, "Connection error : " + e.getLocalizedMessage());
				connectionError = "ERROR connecting to " + mapUrl
						+ ",\n" + e.getLocalizedMessage();
				} catch (HttpStatusException e)
					{
					Log.e(TAG, "Connection error : " + e.getLocalizedMessage());
					connectionError = "ERROR connecting to " + mapUrl
							+ ",\n" + e.getLocalizedMessage();

				} catch (IOException e)
				{
				Log.e(TAG, "IOException Connection error : " + e.getLocalizedMessage());
				connectionError = "ERROR connecting to " + mapUrl
						+ ",\n" + e.getLocalizedMessage();

				
			} catch (Exception e)
				{
				Log.e(TAG, "Other Exception Connection error : " + e.getLocalizedMessage());
				connectionError = "ERROR connecting to " + mapUrl
						+ ",\n" + e.getLocalizedMessage();

				}

			return links;
			}

		@Override
		protected void onPostExecute(Elements links)
			{

			setContentView(R.layout.activity_download_list);
			textView = (TextView) findViewById(R.id.maplist_textView2);

			if (links != null)
				{

				if (links.size() == 0)
					{
									
					textView.setText("Sorry, could not find any maps in "+mapUrl);

					} else
					{
					List<MapFile> listMap = new ArrayList<MapFile>();

					// Done (we hope). Remove "Connecting to server ..." text
					textView.setText("");

					for (Element link : links)
						{
						listMap.add(new MapFile(link.attr("href")));
						}

					listViewMapFile = (ListView) findViewById(android.R.id.list);
					listViewMapFile.setAdapter(new FileListAdapter(ctx,
							R.layout.activity_download_row_item, listMap));
					}
				} else {
				
					textView.setText(connectionError);
				
				}

			}

		/**
		MapFile object.
		The main functionality of this object is the OnClickListener() method.
		This allows us to then display a list of downloadable map files and attach
		the click listener to a Download button for each map.
		*/
		public class MapFile
			{
			/**
			The map's file name
			*/
			private String fileName; 

			/**
			This can be attached to a (ListView) display download button
			*/
			public OnClickListener listener = new OnClickListener()
				{ 
					@Override
					public void onClick(View v)
						{
						// Like it says, download and save the specified map file.
						downloadAndSave(mapUrl + "/" + fileName);
						}
				};

			/**
			Constructor
			@param aFileName
			*/
			public MapFile(String aFileName)
				{
				super();
				this.fileName = aFileName;
				}

			/// Accessor
			public String getFileName()
				{
				return this.fileName;
				}

			/// Modifier, not really needed as the Constructor does the work
			@SuppressWarnings("unused")
			public void setFileName(String nameText)
				{
				this.fileName = nameText;
				}
			}

		/**
		Built with hints from
		"Tutorial: ListView in Android using custom ListAdapter and Cache View",
		Fabio Tuzza,
		http://www.framentos.com/en/android-tutorial/2012/07/16/listview-in-android-using-custom-listadapter-and-viewcache/
		*/
		private class FileListAdapter extends ArrayAdapter<MapFile>
			{
			private int resource;
			private LayoutInflater inflater;

			public FileListAdapter(Context ctx, int resourceId,
					List<MapFile> objects)
				{

				super(ctx, resourceId, objects);
				resource = resourceId;
				inflater = LayoutInflater.from(ctx);
				}

			@Override
			public View getView(int position, View convertView, ViewGroup parent)
				{

				/* create a new view of my layout and inflate it in the row */
				convertView = (RelativeLayout) inflater.inflate(resource, null);

				/* Extract the city's object to show */
				MapFile mapFile = (MapFile) getItem(position);

				/* Take the TextView from layout and set the map file name */
				TextView txtName = (TextView) convertView
						.findViewById(R.id.maplist_file_name);
				txtName.setText(mapFile.getFileName());

				Button button = (Button) convertView
						.findViewById(R.id.maplist_download_button);
				button.setOnClickListener(mapFile.listener);

				return convertView;
				}
			}
		}

	/**
	For monitoring file download progress Reference:
	http://www.java-samples.com/showtutorial.php?tutorialid=1521
	*/
	@Override
	protected Dialog onCreateDialog(int id)
		{
		switch (id)
			{
			case DIALOG_DOWNLOAD_PROGRESS:
				mProgressDialog = new ProgressDialog(this);
				mProgressDialog.setMessage("Downloading map file ...");
				mProgressDialog
						.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
				if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB)
				{
					// Only available for API 11 and higher:
					mProgressDialog.setProgressNumberFormat(null);
				}				
				// In a production app, you probably want to setCancelable(true)
				// and then add code to elegantly stop download and remove incomplete download.
				mProgressDialog.setCancelable(false);
				mProgressDialog.show();
				return mProgressDialog;
			default:
				return null;
			}
		}

	private void downloadAndSave(String aUrl)
		{

		// This is a network task, cannot execute on UI thread
		DownloadWebPageTask task = new DownloadWebPageTask();
		task.execute(aUrl);

		}

	/**
	Download a web page, in this case a map file, as an Asymmetric background
	task.
	*/
	private class DownloadWebPageTask extends
			AsyncTask<String, Integer, String>
		{

		/**
		For monitoring download progress
		*/
		@Override
		protected void onPreExecute()
			{
			super.onPreExecute();
			showDialog(DIALOG_DOWNLOAD_PROGRESS);
			}

		/**
		This method runs on another thread than the UI thread
		 
		NOTE: as written this is inefficient for a large file, it will be
		downloaded completely to memory before being written be the
		onPostExecute() method.
		*/
		@Override
		protected String doInBackground(String... aUrl)
			{

			Uri u = Uri.parse(aUrl[0]);
			File f = new File("" + u);
			// TODO Hard-coded directory name
			String fileName = Environment.getExternalStorageDirectory()
					+ "/CartoType/map/" + f.getName();

			try
				{

				// First, get the size of the file for progress monitoring
				java.net.URL url = new java.net.URL(aUrl[0]);
				java.net.URLConnection conn = url.openConnection();
				int fileSize = conn.getContentLength();
				if (fileSize < 0)
					Log.e(TAG, "Could not determine file size of " + aUrl[0]);
				else
					Log.i(TAG, "The size of the file is = " + fileSize
							+ " bytes");
				// NOTE: You could save last-modified value to avoid downloading
				// files again
				Log.i(TAG,
						"The file last modified: "
								+ new Date(conn.getLastModified()));
				conn.getInputStream().close();

				java.io.BufferedInputStream in = new java.io.BufferedInputStream(
						new java.net.URL(aUrl[0]).openStream());
				java.io.FileOutputStream fos = new java.io.FileOutputStream(
						fileName);
				java.io.BufferedOutputStream bout = new BufferedOutputStream(
						fos, 1024);
				byte[] data = new byte[1024];
				int x = 0;
				int totalDownloaded = 0;
				while ((x = in.read(data, 0, 1024)) >= 0)
					{
					bout.write(data, 0, x);

					// For UI progress monitoring
					totalDownloaded += x;
					publishProgress((int) ((totalDownloaded * 100) / fileSize));
					}
				fos.flush();
				bout.flush();
				fos.close();
				bout.close();
				in.close();

				Log.i(TAG, "Download completed, written to " + fileName);

				} catch (Exception e)
				{
				/* Error to console. */
				Log.e(TAG, "Error: " + e.getMessage());
				return "Error: " + e.getMessage();
				}

			return "Done!";
			}

		// This method runs on the UI thread
		@Override
		protected void onProgressUpdate(Integer... progress)
			{
			super.onProgressUpdate();
			mProgressDialog.setProgress(progress[0]);
			}

		@Override
		protected void onPostExecute(String unused)
			{
			dismissDialog(DIALOG_DOWNLOAD_PROGRESS);
			}

		}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
		{
		getMenuInflater().inflate(R.menu.activity_map_list, menu);

		// TODO Work out how to change map in this activity, only main
		// NavigatorApp knows about the map.
		// menu.add(0, MENU_CHANGE_MAP, 0, "Change Current Map ...");

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
