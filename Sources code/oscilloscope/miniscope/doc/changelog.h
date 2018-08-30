/** \file
	\brief Miniscope changelog
*/

/** \page changelog Changelog

Version 4.00.00 (comparing to v3):
	- multichannel
	- FIXED: unsafe log routine
	- display settings: grid labels presence splitted to grid labels for X and for Y axis;
	  grid labels for Y axis are disabled by default
	- display: color selection for traces

Version 4.00.01
	- added internalization capability with GNU gettext (dxgettext) tool

Version 4.00.02
	- FIXED: incorrect values (labels) at horizontal grid when zooming in/out
	- changed zoom behaviour: program calculates zoom to get 1:2:5/div value
	sequence when zooming in/out; hold Shift while zooming for more precise changes
	- added current (taking zoom into account) volts/div and time/div (or frequency/div) display
	- Settings: added "Write log to file" switch
	- added markers for 0V (GND) level for each channel
	- added View/Plot menu to toggle visibility of plot elements

Version 4.00.03
	- device interface (version number incremented):
		- added (optional) function to enable/disable each channel
		- added (optional) function to change trigger position (pre-/post-trigger)
	- added pre-/post-trigger control to user interface
	- display: added trigger position and voltage markers
	- refactoring: merged struct S_DATAFRAME (meta-data) with class Data (actual samples) to
	make easier to keep data presentation consistent

Version 4.00.04
	- plot: smooth trace moving (OnMouseMove, configurable option, enabled by default)
	- plot: trace vertical zoom using mouse scroll

Version 4.00.05
	- FIXED: not working recorder mode (added custom assignment operator for Data::Channel)

Version 4.00.06
	- changed date/time formatting to YYYY-mm-dd HH:MM and added capture timestamp to statistics window
	- FIXED: JSON and MS3 viewer panel was disabled when there was no connection with device
	- part of main window separated into separate window, TfrmControl to reduce complexity
	- FIXED: zoom & scale display not updated in some cases;
	added display/frmMain observable/observer relation
	- changed device interface (version number incremented)
		- added "Run" function: acquisition start/stop, this would make it easier
		to work with large buffer size and slow sampling speed combination (previously
		there was no method to stop frame acquisition in progress)
		- added "SetTriggerMode" function: setting trigger mode to single, normal,
		auto or (for devices capturing data in real time such as miniscope v2 or sound card)
		continuous
		- manual trigger ("force trigger") button is now active in every trigger mode
		except continuous (if dll has exported "ManualTrigger" function)

Version 4.00.07
	- FIXED: occasional incorrect "Auto fit X" behavior after switching sampling
	frequency
	- FIXED: not working trigger source selector
	- FIXED: problem with switching to FFT mode

Version 4.00.08
	- FIXED: plot not updated when using frame prev/next in recorder mode until redrawing;
	immediate plot update on frame selection change

Version 4.00.09
	- FIXED: not working controls for low-speed recorder

Version 4.00.10
	- FIXED: trigger mode selector not restored after leaving long-interval recorder

Version 4.00.11
	- old "Statistics" renamed to "Measurements"
	- added new "Statistics" window for calculating time period sum and average values

Version 4.00.12
	- added channel enable/disable function, present previously in device interface,
	but implented only by sig_test.dll now
	- cleaned dll enumeration log
	- added dll re-enumeration function to dll selection window
	- statistics window: added ID to caption; ID is incremented each time new window is opened

Version 4.00.13
	- removed dxgettext (translations) as a possible source of crash at exit
	(stack frame at THook.Disable, PatchPosition[0]:=Original[0]);
	it might be rare and OS-related (Win7 64-bit) problem, but translation usefullness
	seemed questionable and even my initial translation was not maintained,
	so it shouldn't be big loss; thanks to kab for report
	- small changes to device interface headers for compatibility
	with MSVC

Version 4.00.14
	- improved drawing performance; initially my idea was to reduce number of
	OpenGL calls (currently each data point is drawn even if zoom is very low
	and hundreds of points would be drawn in single physical place), but after
	testing OpenGL overhead seems not significant; while profiling other
	bottleneck was found: call to [] std::vector operator, replacing it with
	raw pointer reduced display update time from 600 ms to 200 ms (drawing
	3 channels with 1M samples each on Pentium M @ 600MHz + Radeon 9000M)
	- added Device/Connect, Disconnect to main menu (useful mainly for testing
	purposes: eliminated the need to restart application when connection with
	device is lost, i.e. device firmware was updated)
	- FIXED: assertion on switching to FFT mode with disabled channel
	- added range and cursor values labels to plot
	- FIXED: incorrect time/div displayed value at startup

Version 4.00.15
	- limited recorder file length to 2GB (ftell/fseek limitation)
	- minor fixes and cleanup

Version 4.00.16
	- fixed major bug introduced in 4.00.15, incorrect Data::bEnabled initialization

Version 4.00.17
	- MRU (Most Recently Used) lists for JSON and MS3 data files
	- GND mark is not displayed for disabled channel

Version 4.00.18
	- FIXED: data export (conversion) from loaded MS3 file (custom format) to JSON and CSV
	- allowing storing gain coefficient when its value calculated during calibration is outside
	of reasonable range (thus device DLL can be used with hardware it was not intended for, i.e. with different voltage range
	than specified by device description)
	- FIXED: wrong frequency calculated by automatic frequency measurements for square waves with sharp slopes
	- trigger position and level markers are now hidden in continuous trigger mode

Version 4.00.19
	- FIXED: X, Y value/div labels were not updated on new frame after switching sampling speed
	- X, Y (for each channel) oscilloscope mode zoom is restored after restart
	- settings: new option to keep display in 1-2-5 time base / sensitivity sequence ("rounding") when switching sampling speed / gain

Version 4.00.20
	- plot Y positions are stored in config file
	- plot: added plot line width setting
	- new measurements window allowing copying text
	- added some const qualifiers in device DLL interface header

Version 4.00.21
	- support for png format when exporting plot images (included libpng and zlib as static libraries) 
	- added quick export for plot image toolbar button; files are saved to default (previously used) directory with names containing timestamp including miliseconds
	- status bar: no flickering when moving cursors (changed to DoubleBuffered)
	- updated "About" window, added license text for libpng and zlib, added link to application directory
	- moved json-cpp code to separate static library

Version 4.01.00
	- application configuration uses now JSON istead of ini format; this breaks backward compatibility and
	application must be configured again as there is no ini import, minor version number is changed to indicate this
	- fixed incorrect colors in exported png files
	- added miliseconds to default file name for manual bmp/png exporting for consistency
	- hotkey (local and global), hotkey settings (reused from tSIP), available actions to assign:
		- plot image export
		- manual trigger
		- switching mouse tools
*/
