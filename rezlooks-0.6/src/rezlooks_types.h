#ifndef REZLOOKS_TYPES_H
#define REZLOOKS_TYPES_H

typedef unsigned char boolean;
typedef unsigned char uint8;

typedef enum
{
	CL_STATE_NORMAL,
	CL_STATE_ACTIVE,
	CL_STATE_SELECTED,
	CL_STATE_INSENSITIVE
} RezlooksStateType;

typedef enum
{
	CL_CORNER_NONE        = 0,
	CL_CORNER_TOPLEFT     = 1,
	CL_CORNER_TOPRIGHT    = 2,
	CL_CORNER_BOTTOMLEFT  = 4,
	CL_CORNER_BOTTOMRIGHT = 8,
	CL_CORNER_ALL         = 15
} RezlooksCorners;

typedef enum
{
	CL_JUNCTION_NONE      = 0,
	CL_JUNCTION_BEGIN     = 1,
	CL_JUNCTION_END       = 2
} RezlooksJunction;

typedef enum
{
	CL_STEPPER_UNKNOWN    = 0,
	CL_STEPPER_A          = 1,
	CL_STEPPER_B          = 2,
	CL_STEPPER_C          = 4,
	CL_STEPPER_D          = 8
} RezlooksStepper;

typedef enum
{
	CL_ORDER_FIRST,
	CL_ORDER_MIDDLE,
	CL_ORDER_LAST
} RezlooksOrder;

typedef enum
{
	CL_ORIENTATION_LEFT_TO_RIGHT,
	CL_ORIENTATION_RIGHT_TO_LEFT,
	CL_ORIENTATION_BOTTOM_TO_TOP,
	CL_ORIENTATION_TOP_TO_BOTTOM
} RezlooksOrientation;

typedef enum
{
	CL_GAP_LEFT,
	CL_GAP_RIGHT,
	CL_GAP_TOP,
	CL_GAP_BOTTOM
} RezlooksGapSide;

typedef enum
{
	CL_SHADOW_NONE,
	CL_SHADOW_IN,
	CL_SHADOW_OUT,
	CL_SHADOW_ETCHED_IN,
	CL_SHADOW_ETCHED_OUT
} RezlooksShadowType;

typedef enum
{
	CL_HANDLE_TOOLBAR,
	CL_HANDLE_SPLITTER
} RezlooksHandleType;

typedef enum
{
	CL_ARROW_NORMAL,
	CL_ARROW_COMBO
} RezlooksArrowType;

typedef enum
{
	CL_DIRECTION_UP,
	CL_DIRECTION_DOWN,
	CL_DIRECTION_LEFT,
	CL_DIRECTION_RIGHT
} RezlooksDirection;

typedef enum
{
	CL_PROGRESSBAR_CONTINUOUS,
	CL_PROGRESSBAR_DISCRETE
} RezlooksProgressBarStyle;

typedef enum
{
	CL_WINDOW_EDGE_NORTH_WEST,
	CL_WINDOW_EDGE_NORTH,
	CL_WINDOW_EDGE_NORTH_EAST,
	CL_WINDOW_EDGE_WEST,
	CL_WINDOW_EDGE_EAST,
	CL_WINDOW_EDGE_SOUTH_WEST,
	CL_WINDOW_EDGE_SOUTH,
	CL_WINDOW_EDGE_SOUTH_EAST  
} RezlooksWindowEdge;

typedef struct
{
	double r;
	double g;
	double b;
} CairoColor;

typedef struct
{
	double x;
	double y;
	double width;
	double height;
} RezlooksRectangle;

typedef struct
{
	CairoColor bg[5];
	CairoColor base[5];
	
	CairoColor shade[9];
	CairoColor spot[3];
} RezlooksColors;

typedef struct
{
	boolean active;
	boolean prelight;
	boolean disabled;
	boolean focus;
	boolean is_default;
	
	RezlooksStateType state_type;
	
	uint8 corners;
	uint8 xthickness;
	uint8 ythickness;

	CairoColor parentbg;
} WidgetParameters;

typedef struct
{
	boolean inverted;
	int     fill_size;
	boolean horizontal;
} SliderParameters;

typedef struct
{
	RezlooksOrientation orientation;
	RezlooksProgressBarStyle style;
} ProgressBarParameters;

typedef struct
{
	int linepos;
} OptionMenuParameters;

typedef struct
{
	RezlooksShadowType shadow;
	RezlooksGapSide gap_side;
	int gap_x;
	int gap_width;
	CairoColor *border;
} FrameParameters;

typedef struct
{
	RezlooksGapSide gap_side;
} TabParameters;

typedef struct
{
	RezlooksCorners    corners;
	RezlooksShadowType shadow;	
} ShadowParameters;

typedef struct
{
	boolean horizontal;
} SeparatorParameters;

typedef struct
{
	RezlooksOrder order;
	boolean         resizable;
} ListViewHeaderParameters;

typedef struct
{
	CairoColor         color;
	RezlooksJunction junction;       /* On which sides the slider junctions */
	RezlooksStepper  steppers;       /* The visible steppers */
	boolean            horizontal;
	boolean            has_color;
} ScrollBarParameters;

typedef struct
{
	RezlooksHandleType type;
	boolean              horizontal; 
} HandleParameters;

typedef struct
{
	RezlooksStepper stepper;         /* Which stepper to draw */
} ScrollBarStepperParameters;

typedef struct
{
	RezlooksArrowType type;
	RezlooksDirection direction;
} ArrowParameters;

typedef struct
{
	RezlooksWindowEdge edge;
} ResizeGripParameters;

#define REZLOOKS_RECTANGLE_SET(rect, _x, _y, _w, _h) rect.x      = _x; \
                                                       rect.y      = _y; \
                                                       rect.width  = _w; \
                                                       rect.height = _h;

#endif /* REZLOOKS_TYPES_H */
