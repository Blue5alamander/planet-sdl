# Planet SDL2 Library

Provides some basic C++ wrappers around SDL2


## Asset manager

The asset manager expects to find files in a `share` folder which is a sibling to the executable path. This only works on platforms which distribute the executable along with files that end up on the file system. Other platforms hold the assets in a platform specific way.


### Android

Include the `android/java/com` folder in your Java sources directory (a soft link is encouraged for this purpose). The native code asset manager requires the Kotlin code in order to access the platform asset manager.
