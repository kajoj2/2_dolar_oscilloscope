/** \file
	\brief Miniscope roadmap
*/

/** \page roadmap Roadmap

Bunch of things to do in future releases. No particular order:
	- auto-reconnect

	- FFT: logarithmic vertical scale
	- FFT: add phase plot?

	- printing
	- tool to select part of the plot for FFT, statistics, export
	- plot manipulation using keyboard: Ctrl+arrows = MOVE, Shift+arrows = ZOOM,
	shift+space = clear zoom & offset, LAlt/RAlt+arrows = cursors
	- shape selection for cursors
	- limiting precision of displayed float values using LSB
	- docking support for frmStatistics and toolboxes

	- plot: style select (line/point/bar)
	- plot: antialiasing
	- plot: auto-scrolling mode (for recorder mode only?)
	- plot: associate cursor position with physical value, not position on canvas
	- plot: show relative position of display window in full buffer frame
	- plot: sinx/x interpolation mode (now: only linear)
	- plot: channel drawing order (Z-order)
	- color selection for display
	- color schemes for display (default/printer-friendly)	
	- plot: OSD (trigger mode, sensitivity, time base...) as a part of printer-friendly mode

	- measurement: flexible selection of interesting data
	- measurement: pulse H/L width

	- acquisition modes:
	peak detect mode (like current recorder mode, but stores min and max value from frame),
	high-res mode (currently used in v2a)
	Sample Mode: This is the simplest acquisition mode. The oscilloscope
	creates a waveform point by saving one sample point during each
	waveform interval.
	Peak Detect Mode: The oscilloscope saves the minimum and 
	maximum value sample points taken during two waveform intervals
	and uses these samples as the two corresponding waveform points.
	Digital oscilloscopes with peak detect mode run the ADC at a fast 
	sample rate, even at very slow time base settings (slow time base 
	settings translate into long waveform intervals) and are able to 
	capture fast signal changes that would occur between the waveform
	points if in sample mode
	Hi Res Mode: Like peak detect, hi res mode is a way of getting 
	more information in cases when the ADC can sample faster than 
	the time base setting requires. In this case, multiple samples taken
	within one waveform interval are averaged together to produce one
	waveform point. The result is a decrease in noise and an improvement
	in resolution for low-speed signals.
	Envelope Mode: Envelope mode is similar to peak detect mode.
	However, in envelope mode, the minimum and maximum waveform
	points from multiple acquisitions are combined to form a waveform
	that shows min/max accumulation over time. Peak detect mode is 
	usually used to acquire the records that are combined to form the
	envelope waveform.
	Average Mode: In average mode, the oscilloscope saves one 
	sample point during each waveform interval as in sample mode.
	However, waveform points from consecutive acquisitions are then 
	averaged together to produce the final displayed waveform. Average
	mode reduces noise without loss of bandwidth, but requires a 
	repeating signal.

	- triggering
	Slew Rate Triggering. High frequency signals with slew rates
	faster than expected or needed can radiate troublesome 
	energy. Slew rate triggering surpasses conventional edge 
	triggering by adding the element of time and allowing you to
	selectively trigger on fast or slow edges.
	Glitch Triggering. Glitch triggering allows you to trigger 
	on digital pulses when they are shorter or longer than a 
	user-defined time limit. This trigger control enables you to
	examine the causes of even rare glitches and their effects 
	on other signals
	Pulse Width Triggering. Using pulse width triggering, you 
	can monitor a signal indefinitely and trigger on the first 
	occurrence of a pulse whose duration (pulse width) is 
	outside the allowable limits.
	Time-out Triggering. Time-out triggering lets you trigger 
	on an event without waiting for the trigger pulse to end, by
	triggering based on a specified time lapse.
	Runt Pulse Triggering. Runt triggering allows you to 
	capture and examine pulses that cross one logic threshold,
	but not both.

	- trigger: low frequency rejection, high frequency rejection, noise rejection
	- trigger holdoff: new triggers are not recognized during holdoff time

	- DPO mode (pixel color intensivity calculated from data value frequency in multiple frames)

	- store/load settings sets (sensitivity, sampling, ...)
	- handle PC sleep/suspend to RAM (reconnect/reconfigure device)
	- wavein.dll - add more features (sampling frequency, buffer size, trigger settings)
	- JSON: add zoom, position, comment and device name to exported data
	- WAVE export and/or direct play with some resampling
	- auto-setup for sampling frequency and sensitivity
	- ms3: some timer-based playback?

	- rename recorder mode?
	- device library selection at startup: no point to show dll list if it's empty


	- device interface: voltage range (from...to) specification
	instead of (or next to?) voltage per bit -> would make bit number obsolete
	- device interface: error feedback (to help with different time base range when
	some channels are disabled)
	- device interface: configure time to forced trigger in auto mode

	- handle mouse VK_XBUTTON1, VK_XBUTTON2
	- mark channel selector button as inactive (crossed?) when channel is disabled
	- warn when trigger source is set to inactive channel


	- FIXME MS3: format, browser:
		- trigger type: falling slope instead of continuous
	- FIXME: "fit plot vertically" working incorrectly when browsing MS3

	- hide channel selector when device has only one channel
	- control: show sensitivity and AC/DC settings for more than single channel
	
	- bookmarks: zoom/position

	FIXME: incorrect frequency (FFT) reading and plot moving at very high zoom



miniscope v4: log-log scale for spectrum analyzer
miniscope v4: data part selection for spectrum analyzer

- play short wave file on trigger event

- plot: Y offset should be related to geometry, not physical value

T.M.: 2 ideas: 
- a button for selecting 1:1 or 1:10 probes ( with scaling of the voltages)
- an option for placing horizontal or vertical "cursor-lines" on the display ( voltage or time between the lines could be shown)

miniscope v4: remember last tool used on restart

X-Y mode to plot correlation between two signals (point type plot based on density)

snap GND level to grid when moving plot

*/


