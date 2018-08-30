/** \file
	\brief HSV (Hue Saturation Value) <-> RGB color space conversions
*/
//---------------------------------------------------------------------------

#ifndef HsvH
#define HsvH
//---------------------------------------------------------------------------

#include <Graphics.hpp>

class Hsv
{
public:
	int h;	///< hue, angle 0 to 360 (not including 360)
	int s;	///< saturation, 0 to 255
	int v;	///< value, 0 to 255
	/** \brief Get RGB values for HSV space color */
	TColor ToRGB(void);
	/** \brief Convert RGB color to HSV space */
	void FromRGB(TColor color);
	/** \brief Get color from HSV space divided in uniform way
		\param count number of colors to divide HSV space into
		\param start_hue hue for first color from set (initial hue)
		\param id number of color from uniformly divided set
	*/
	TColor GetUniformColor(int count, int start_hue, int id);
};

#endif
