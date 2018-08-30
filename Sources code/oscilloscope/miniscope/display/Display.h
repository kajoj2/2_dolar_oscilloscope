/** \file Display.h
    \brief Abstract display interface.
*/

/* Copyright (C) 2008-2009 Tomasz Ostrowski <tomasz.o.ostrowski at gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.

 * Miniscope is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DisplayH
#define DisplayH

#include "Mode.h"
#include "Observable.h"
#include <vector>
#include <memory>
#include <Graphics.hpp>

class Data;

/** \brief Class of object passed to registered observers
*/
class DisplayNotifyArgument: public Argument
{
public:
	/** \brief Change type
	*/
	enum TYPE
	{
		SCALE	///< display scale (X or Y axis) changed; 
	} type;
};

/** \brief Abstact display interface class
 */
class Display: public Observable
{
public:
	/** \brief Required level of graph update
	 *
	 *  Drawing is time-consuming. Display object can hold partial graph drawings
	 *  (i.e. graph background + graph grid) and reuse them when they do not need
	 *  to be updated.
	 */
	enum E_UPDATE_LEVEL
	{
		UPDATE_ALL = 0,
		UPDATE_DATA,
		UPDATE_CURSOR,
		UPDATE_REPAINT
	};

	/** \brief Cursor identifiers
	*/
	enum E_CURSOR
	{
		CURSOR1,
		CURSOR2,
		CURSOR_LIMIT
	};

	/** \brief Plot elements
	*/
	enum E_ELEMENT
	{
		GRID_X = 0,
		GRID_Y,
		GRID_LABELS_X,
		GRID_LABELS_Y,
		CURSOR,
		GND_MARKER,
		TRIGGER_POS_MARKER,	///< trigger position inside frame marker
		TRIGGER_VAL_MARKER,	///< trigger voltage marker
		RANGE_VALUES,		///< time/div, voltage/div
		CURSOR_VALUES,
		ELEMENT_LIMIT
	};

	/** \brief Special values for SetZoom function passed as X or Y offset value
	*/
	enum E_OFFSET_ACTION
	{
		OFFSET_KEEP_GND_POSITION,   ///< keep 0V level absolute position (Y axis only)
		OFFSET_LEAVE = -2,			///< leave current offset
		OFFSET_CLEAR = -1			///< clear offset
	};

	virtual ~Display() {};
	virtual void SetData(Data *data);
	virtual Data* GetData(void);
	/** \brief Switch between plot modes (time graph/FFT/...)
	*/
	virtual void SetMode(const enum E_MODE m) = 0;
	/** \brief Update displayed content.
	 *  \param level Used to specify parts of graph that must be updated.
	 */
	virtual void Update(enum E_UPDATE_LEVEL level) = 0;
	/** \brief Resize viewport (visible area of plot)
	*/
	virtual void ChangeSize(int w, int h) = 0;
	/** \brief Set expected range of input data to -range...+range
	*/
	virtual void SetRange(int channel, float range);
	/** \brief Set number of channels to display
	*/
	virtual void SetNumberOfChannels(int number);
	/** \brief Read back number of channels
	*/
	virtual int GetNumberOfChannels(void);
	/** \brief Set colors used for traces
	*/
	virtual void SetChannelColors(const std::vector<unsigned int> &colors);
	/** \brief Get color assigned to channel (either user defined or default
	*/
	TColor GetChannelColor(unsigned int channel_id);
	virtual void SetCurrentChannel(int number);
	/** \brief Set position of cursor #1 */
	virtual void SetCursor1(int x, int y) = 0;
	/** \brief Set position of cursor #2 */
	virtual void SetCursor2(int x, int y) = 0;
	/** \brief Set trigger channel
	*/
	virtual void SetTriggerChannel(int channel);
	/** \brief Set trigger position inside frame (pre-/posttriggering).
		Trigger position markup makes sense only in oscilloscope mode.
	*/
	virtual void SetTriggerPosition(int pos);
	/** \brief Set trigger value (-100%...+100%)
	*/
	virtual void SetTriggerValue(int val);
	virtual void GetZoom(float& fZoomX, float& fZoomY); 
	/** \brief Set viewport offset and zoom
		\param iCenterX	requested center of viewport (i.e. zoom click coordinate) or enum E_OFFSET_ACTION
		\param iCenterY requested center of viewport (i.e. zoom click coordinate) or enum E_OFFSET_ACTION
		\param fZoomX	X-axis scale
		\param fZoomY	Y-axis scale
	*/
	virtual void SetZoom(int iCenterX, int iCenterY, float fZoomX, float fZoomY) = 0;
	/** \brief Set Y-axis offset
		\param fVal requested value at center position
	*/
	virtual void SetOffsetY(float fVal) = 0;
	/** \brief Move viewport. */
	virtual void Move(int iStartX, int iStartY, int iStopX, int iStopY) = 0;
	/** \brief Transform screen coordinates to physical values */
	virtual void PosToPhysical(int x, int y, float &fValX, float &fValY) = 0;
	/** \brief Get physical (voltage/time or freq/time) coordinates of cursor
		\return 0 on success
	*/
	virtual int GetCursorPhysical(enum E_CURSOR cursor, float &fValX, float &fValY) = 0;
	/** \brief Set visibility (ON/OFF) of specified plot element
	*/
	virtual void SetVisibility(enum E_ELEMENT element, bool state);
	/** \brief Set Y-axis zoom for particular channel
	*/
	virtual void SetScopeChannelZoom(int channel, float fZoomY);
	/** \brief Get Y-axis offset for particular channel
	*/
	virtual float GetScopeChannelOffsetY(unsigned int channel);
	/** \brief Set Y-axis affset for particular channel (restoring position after restart)
	*/
	virtual void SetScopeChannelOffsetY(unsigned int channel, float offset);
	/** \todo MarkTrigger
	*/

	virtual void SetPlotLineWidth(float width);

	/** \brief Create plot bitmap
	*/
	virtual std::auto_ptr<Graphics::TBitmap> CreateBitmap(void) = 0;
	/** \brief Get current X div value
	*/
	virtual float GetDivX(void);
	/** \brief Get current (and related to current channel) Y div value
	*/
	virtual float GetDivY(void);

protected:
	Display(Data *data);

	enum E_MODE mode;   ///< current display mode type
	int iNumberOfChannels;
	int iCurrentChannel;
	int iTriggerChannel;
    std::vector<unsigned int> channel_colors;

	/** \brief Data to plot
	*/
	Data *data;

	/** \brief Visibility state of plot elements, indexed by E_ELEMENT
	*/
	bool visibility[ELEMENT_LIMIT];

	float fPlotLineWidth;

	/** \brief State of plot elements (visibility, zoom, position) in specific mode
	*/
	struct S_STATE
	{
		struct Channel {
			float fRange;
			float fZoomY;           ///< vertical zoom value
			float fOffsetY;	        ///< Y position of plot on canvas
			float fCursor1Y;        ///< cursor 1 position Y
			float fCursor2Y;        ///< cursor 2 position Y
			Channel() {
				fRange = 1.0;
				fZoomY = 1.0;
				fCursor1Y = 0.0;
				fCursor2Y = 0.0;
				fOffsetY = 0.0;
			}
		};
		std::vector<Channel> channels;

		float fZoomX;           ///< horizontal zoom value
		float fOffsetX;         ///< X position of plot on canvas
		float fWidth;
		float fCursor1X;        ///< cursor 1 position X
		float fCursor2X;        ///< cursor 2 position X
		int iTriggerPosition;	///< trigger position inside frame (0..100%), valid for scope mode
        int iTriggerValue;		///< trigger value (-100%...+100%), valid for scope only

		S_STATE() {
			fZoomX = 1.0;
			fCursor1X = 0.0;	///< cursor/ruler coordinate (physical value)
			fCursor2X = 0.0;    ///< cursor/ruler coordinate (physical value)
			fOffsetX = 0.0;
			fWidth = 1.0;
			iTriggerPosition = 0;
		}
	};
	struct S_STATE scope;       ///< display state for scope mode
	struct S_STATE spectrum;    ///< display state for spectrum analyzer state
    struct S_STATE recorder;	///< display state for slow changing signal recorder

    /** \brief Get current plot state/zoom/offset/visibility
    */
	inline struct S_STATE& State(void) {
		if (mode == MODE_OSCILLOSCOPE)
			return scope;
		else if (mode == MODE_RECORDER)
        	return recorder;
		else
			return spectrum;
	}
	/** \brief Get plot state for specified mode
	*/
	inline struct S_STATE& State(enum E_MODE mode) {
		if (mode == MODE_OSCILLOSCOPE)
			return scope;
		else if (mode == MODE_RECORDER)
        	return recorder;
		else
			return spectrum;
	}
	/** \brief Get padded data count for FFT mode
	*/
	unsigned int NearestPowerOfTwo(unsigned int count);
};

#endif // DisplayH
