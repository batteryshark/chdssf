# CHDSSF
SSF with CHDv5 (Archive.org) Support

SSF is quite the amazing Saturn emulator, however, it lacks an (enabled) option to load from disc images without the use of some adware-infested solution such as DaemonTools.

Included is a patch started less than 24 hours ago that adds CHD support - no emulated drive needed.

Update: Works on most versions automatically now with no modification required.

Instructions:


1. Compile SSFLoader and ssf_patch (or use the provided versions). I use Clion+CMake+Mingw, but use whatever works.

1. Place SSFLoader.exe, libchd.dll, and ssf_patch.dll in the SSF emulator directory.

3. Run SSFLoader.exe with a command argument that contains the relative (or absolute, whatever) path to the chd file.
	e.g.
	```
	"SSFLoader.exe bomb.chd"
	```
4. SSF should start (and so should the game), if it doesn't, check the disc drive selected under Options, it should be set to "CHDDriveVirtual", if not, set it and select "CD Close" from the Hardware drop-down.



Happy Saturn'ing ^^

![Image of CHDSSF](http://i.imgur.com/ad9G42E.png)
