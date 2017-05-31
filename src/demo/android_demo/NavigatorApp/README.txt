NavigatorApp
============

This is a Eclipse-based project that produces a simple map/navigiation demo.


Notes on code quality
---------------------

This code for demo purposes only and is not a complete production version. 
It is meant to be simple to get you up and running.
Specifically please note:

1) Hard coded strings, no I18N

2) Some minor lint warnings and/or lint suppression, see internal code comments

3) Most importantly, all calls to the CartoType native Framework library are done on the
UI thread.  Testing demonstrates that this significantly slows calls to Framework.getMap().  
One method for dealing with this is as follows:

- Convert the MapView class to derive from SurfaceView instead of View.
- As per the standard SurfaceView pattern, create a separate (map) draw class that runs on a separate thread
- Implement an event loop in the map draw thread that has sole responsibility for calling getMap() or
other canvas changes.
- Communicate with the event loop whenever you want the map re-drawn. At the simplest via simple boolean flags.
- The down side is that you will now need to deploy Java synchronisation {} blocks when interacting with the
Framework object ... unless of course you do it all with the map draw thread.



How to run your first CartoType NavigatorApp:
---------------------------------------------

1.  Install Eclipse and newest Android SDK tools Versions

    It is strongly recommended to use at least version 17 of the Android SDK Platform tools
    and version 22.0.1 of the Android SDK tools. Earlier versions have hard to track problems 
    including libraries into the Android .apk.

    The Android add-on in Eclipse is installed by using 

         Help > Install New Software ... > Add

    and adding this repository:

         https://dl-ssl.google.com/android/eclipse/

2.  Use Window > Android SDK Manager to download the SDK versions that you want to support.

    This is a suggested set, but you only need one to start, (usually the latest):

     API 2.2 (V8)
     API 3.0 (V11)
     API 4.0 (V14)
     API 4.4 (V19)
     API 5.1 (V22)
    
3.  Download "NavigatorApp"-Demo from CartoType
    - Link http://cartotype.com/assets/downloads/NavigatorApp.zip
      (or latest version)
    - From the zip file unzip the folder "CartoType\src\demo\android_demo\NavigatorApp" to the eclipse working space
  
4.  Import the project to Eclipse
    - Menu: File > Import > General > ExistingProjectsIntoWorkspace
    
    Depending on your version of Eclipse, you may get either of these errors:
    
    	Project has no project.properties file! 
    	AndroidManifest.xml file not found!
    	
    If so, clean and re-build the project and the error should go away:
    
    	Menu: Project > Clean
    	Menu: Project > Build Project
 
5.  Download the CartoType Android SDK  

    THIS IS PROVIDED SEPARATELY SO THAT YOU CAN ALWAYS GET THE LATEST STABLE BUILD.
    
    - Link http://cartotype.com/assets/downloads/cartotype_android_sdk.zip
    
    - Add all what you find in the zip file to the folder libs in the NavigatorApp
      -> NavigatorApp\libs\cartotype.jar: This is the Java wrapper to the native library.
      -> NavigatorApp\libs\armeabi\libcartotype.so: This is the native library.
      
6.  Android Support Lib
    
    This step will depend on what version you decide to use and your current environment,
    it may not be necessary.
    
    Right-click over NavigatorApp project and choose "Android Tools" > AddSupportLibrary:
    -> Install whatever the SDK thinks has to be installed.
    
7. Building the project
    Menu: Project > Clean
    -> The project should build now without errors
 
8. Run:
    Menu: Run > RunAs > Android > Application 

Having problems ?
-----------------

If you are having problems getting all the libraries into the .apk, here are some things to check:

1.  Did you put CartoType API in place ? :
    
    NavigatorApp\libs\cartotype.jar
    NavigatorApp\libs\armeabi\libcartotype.so 
      
2.  Build Path ?

    Right-click over the NavigatorApp project and choose Properties > BuildPath
    
    - Under libraries make sure the libs\cartotype.jar wrapper is added
    
    - Under OrderAndExport make sure AndroidPrivateLibraries is checked 
    
    - Under libraries make sure android-support-v4.jar is added and in right location
      (this should have been handled by step 6 above)
      For more info, see
      http://stackoverflow.com/questions/18299898/the-import-android-support-cannot-be-resolved
    
3.  Allow Native support ?

    You will also need to make sure that the following general preferences option is un-checked:
    Menu: Window > Preferences > Android > Build: Force error when external jars contain native libraries 
  
4.  Properties > Builders
 
 	- Make sure the the CopyLatestCartoTypeLibs is *unchecked*.
 	- This is only needed by customers compiling the CartoType API themselves.
 	
 	- Make sure all the other items are checked.
 	
5.  Analyze the .apk

	When you build, this is placed in the bin/ folder.
	If you change the suffix to .zip, you can see what is in it using a Zip unarchiver.
	It should contain at least these files and folders:
	
	assets folder containing the example map, stylesheet and font files in sub-folders.
	lib/armeabi/libcartotype.so
	classes.dex  (about 1.3 MB in unpacked size)
	
	
	
	  