#include "Action.h"

const char* Action::getTypeDescription(enum Action::Type type)
{
	switch (type)
	{
	case TYPE_NONE:
		return "none";
	case TYPE_MANUAL_TRIGGER:
		return "manual trigger";
	case TYPE_SAVE_GRAPH_IMAGE:
		return "(quick) save plot as png";
	case TYPE_TOOL_CURSOR:
		return "switch tool: cursor";
	case TYPE_TOOL_MOVE:
		return "switch tool: move";
	case TYPE_TOOL_ZOOM:
		return "switch tool: zoom";
	case TYPE_TOOL_ZOOM_X:
		return "switch tool: zoom X";
	case TYPE_TOOL_ZOOM_Y:
		return "switch tool: zoom Y";
	case TYPE_TOOL_ZOOM_CLEAR:
		return "clear zoom";
	case TYPE_TOOL_ZOOM_FIT_X:
		return "fit in X axis";
	case TYPE_TOOL_ZOOM_FIT_Y:
		return "fit in Y axis";

	default:
		return "";
	}
}

