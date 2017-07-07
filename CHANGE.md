
====== Changelist =========

# 2017-7 Version 2.4.6 #

1. Add demos for android version.
2. Write some JNI wrapper for native code.
3. Add some new filters like color mapping.
4. Add real time camera filter. (Preview, take picture, record video)
4. Add real time video filter. (Preview and save)
5. Add face landmark module. (By [FaceTracker](https://github.com/kylemcdonald/FaceTracker), not neccessary. )

/////  Other small fix.     ////////


# 2015-5-13 Version 1.13.1 #

Fix definition of ``CGE_CURVE_PRECISION``

# 2015-5-11 Version 1.13.0 #

Feature: new filter ``monochrome`` with argument ``red, green, blue, cyan, magenta, yellow``. (The same with Photoshop)

# 2015-4-10 Version 1.12.0 #

Feature: Add font render module. (With FreeType)

# 2015-3-26 Version 1.11.1 #

Replace ``glGetAttribLocation`` with ``glBindAttribLocation``.
No ``glGetAttribLocation`` now.

# 2015-3-26 Version 1.11.0 #

Bug fix for invalid usage of glPixelStorei.

# 2015-2-4 Version 1.10.1 #

Bug fix for scene roaming: Error value when the watch point plus the watch dir is equal to 0.

# 2015-1-26 Version 1.10.0 #

1. Support all blend modes of photoshop. (There may be difference such as dissolve)
2. Optimize the random function in shader. (The blur looks better)
3. Add fast filter judgement. (in cgeInitialize function)
4. Bug fix.
5. Finish slideshow3d, support scene roaming. Support both using 2d&3d sprites in one timeline.

# 2015-1-13 Version 1.9.1 #

Optimize the code structure of slideshow, reduce useless public functions.

# 2015-1-8 Version 1.9.0 #

1. New 3d sprite.
2. Support adding actions to the timeline. (Use actions to control the scene. )
3. Optimize for the code structure.

# 2014-12-18 Version 1.8.1 #

1. Be better with iOS.
2. Remove some inline functions, reduce the size of library.

# 2014-12-16 Version 1.8.0 #

1. SDK fix, support image with alpha channels.(Keep the alpha channel for the result).
2. Fix the invalid effect of ``cvlomo`` on iOS device.
3. Some compatible fix for Android.

# 2014-12-16 Version 1.7.1 #

Fix issue: The curve texture may be failed to create in some device(OpenGLES2.0).

# 2014-12-10 Version 1.7.0 #

Be compatible with older version of CGE app.

# 2014-11-24 Version 1.6.1 #

Bug fix: Fix the parsing method of rule ``vigblend``.

# 2014-11-18 Version 1.6.0 #

Merge with CGE Filter module, add some filters & effects.

# 2014-11-14 Version 1.5.1 #

Add ``Sprite2dWithSpecialAlpha`` for special alpha gradient.

# 2014-11-12 Version 1.5.0 #

Fix for curve module, remove some useless files.

# 2014-11-11 Version 1.4.4 #

Bug fix & Optimize Curve Filter.

# 2014-11-5 Version 1.4.3 #

Finish filter generator for libCGE.

# 2014-10-31 Version 1.4.2 #

Make GeometryLineStrip2d's program single instance. Avoid recreating it when using.

# 2014-10-30 Version 1.4.1 #

Add some test case for sprite module.

# 2014-10-29 Version 1.4.0 #

New feature: timeline realtime jumping by progress.

# 2014-10-25 Version 1.3.0 #

Add shader cache for the sprite module.
Reduce the time for slideshow's initialization.

# 2014-10-16 Version 1.2.1 #

Add global blending mode macro. Support global setting of texture's alpha.(premultiply or not)

# 2014-10-16 Version 1.2.0 #

Optimize code include method, avoid long-time compile time for slideshow module. 

# 2014-10-13 Version 1.1.5 #

Add antialias for line drawing.

# 2014-10-11 Version 1.1.4 #

Optimize the stucture of the Sprite module.

# 2014-9-25 Version1.1.1 #

Use global array buffer cache, avoid recreating it for every instance.
You may need to call ``cgeInitialize``.

//////////////////////////////////////////////////////////

# 2014-9-24 Version 1.1.0 #

Support mixed type of sprites in the same timeline.

//////////////////////////////////////////////////////////

## 2014-9-4 Version 1.0.0 ##
First stable version.
Maybe some bugs. If there're anything wrong, you can go back to 0.1.*

/////////////////////////////////////////////////////////