/** \file DisplayOpenGL.h
	\brief Display implementation based on OpenGL renderer
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

#ifndef DisplayOpenGLH
#define DisplayOpenGLH

#include <windef.h>
#include "Display.h"
#include <vector>

//---------------------------------------------------------------------------
/** \brief OpenGL based Display implementation
*/
class DisplayOpenGL: public Display
{
private:	// User declarations
	HDC hdc;
	HGLRC hrc;
	unsigned int base;				// Base Display List For The Font Set
	static const float fHeight; ///< floating point (OpenGL internal) display height
	int iWidth;                 ///< horizontal dimension in screen pixels
	int iHeight;                ///< vertical dimension in screen pixels
	/** \brief Create bitmap font used for text labels */
	void CreateBmpFont(void);
	/** \brief Destroy bitmap font */
	void DestroyFont(void);
	/** \brief Put text on display using bitmap font */
	void Print(const char *fmt, ...);
	/** \brief Put text with outline
		\param x X position
		\param y Y position
		\param color font color
		\param outline outline color
	*/
	void PrintWithOutline(float x, float y, TColor color, TColor outline, const char *fmt, ...);
	void SetVsync(bool enabled);
	void SetupRC(void);
	void SetPixelFormatDescriptor();
	/** \brief Transform X coordinate to physical value
	*/
	float PosToPhysicalX(int x);
	/** \brief Transform Y coordinate to physical value
	*/
	float PosToPhysicalY(int y);

public:		// User declarations
	DisplayOpenGL(Data* data, void* hParent);
	virtual ~DisplayOpenGL();
	virtual void SetMode(const enum E_MODE m);
	virtual void Update(enum E_UPDATE_LEVEL level);
	virtual void ChangeSize(int iWidth, int iHeight);
	virtual void SetData(Data *data);
	virtual void SetCursor1(int x, int y);
	virtual void SetCursor2(int x, int y);
	virtual void PosToPhysical(int x, int y, float &fValX, float &fValY);
	virtual int GetCursorPhysical(enum E_CURSOR cursor, float &fValX, float &fValY);
	virtual void SetZoom(int iCenterX, int iCenterY, float fZoomX, float fZoomY);
	virtual void SetOffsetY(float fVal);
	virtual void Move(int iStartX, int iStartY, int iStopX, int iStopY);
	//virtual void SetRange(int channel, float range);
	virtual std::auto_ptr<Graphics::TBitmap> CreateBitmap(void);
};
#endif
