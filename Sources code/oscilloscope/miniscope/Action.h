#ifndef ActionH
#define ActionH

struct Action
{
	enum Type
	{
		TYPE_NONE = 0,
		TYPE_MANUAL_TRIGGER,
		TYPE_SAVE_GRAPH_IMAGE,
		TYPE_TOOL_CURSOR,
		TYPE_TOOL_MOVE,
		TYPE_TOOL_ZOOM,
		TYPE_TOOL_ZOOM_X,
		TYPE_TOOL_ZOOM_Y,
		TYPE_TOOL_ZOOM_CLEAR,
		TYPE_TOOL_ZOOM_FIT_X,
		TYPE_TOOL_ZOOM_FIT_Y,
		
		TYPE_LIMITER
	} type;
	int id;

	Action(void):
		type(TYPE_NONE),
		id(0)
	{
	}
	static const char* getTypeDescription(enum Type type);

};

#endif
