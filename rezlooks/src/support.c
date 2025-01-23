#include "support.h"

GtkTextDirection
get_direction (GtkWidget *widget)
{
	GtkTextDirection dir;
	
	if (widget)
		dir = gtk_widget_get_direction (widget);
	else
		dir = GTK_TEXT_DIR_LTR;
	
	return dir;
}

void
rgb_to_hls (gdouble *r,
            gdouble *g,
            gdouble *b)
{
	gdouble min;
	gdouble max;
	gdouble red;
	gdouble green;
	gdouble blue;
	gdouble h, l, s;
	gdouble delta;
	
	red = *r;
	green = *g;
	blue = *b;

	if (red > green)
	{
		if (red > blue)
			max = red;
		else
			max = blue;
	
		if (green < blue)
			min = green;
		else
			min = blue;
	}
	else
	{
		if (green > blue)
			max = green;
		else
			max = blue;
	
		if (red < blue)
			min = red;
		else
			min = blue;
	}

	l = (max + min) / 2;
	s = 0;
	h = 0;

	if (max != min)
	{
		if (l <= 0.5)
			s = (max - min) / (max + min);
		else
			s = (max - min) / (2 - max - min);
	
		delta = max -min;
		if (red == max)
			h = (green - blue) / delta;
		else if (green == max)
			h = 2 + (blue - red) / delta;
		else if (blue == max)
			h = 4 + (red - green) / delta;
	
		h *= 60;
		if (h < 0.0)
			h += 360;
	}

	*r = h;
	*g = l;
	*b = s;
}

void
hls_to_rgb (gdouble *h,
            gdouble *l,
            gdouble *s)
{
	gdouble hue;
	gdouble lightness;
	gdouble saturation;
	gdouble m1, m2;
	gdouble r, g, b;
	
	lightness = *l;
	saturation = *s;

	if (lightness <= 0.5)
		m2 = lightness * (1 + saturation);
	else
		m2 = lightness + saturation - lightness * saturation;
		
	m1 = 2 * lightness - m2;

	if (saturation == 0)
	{
		*h = lightness;
		*l = lightness;
		*s = lightness;
	}
	else
	{
		hue = *h + 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;
	
		if (hue < 60)
			r = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			r = m2;
		else if (hue < 240)
			r = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			r = m1;
	
		hue = *h;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;
	
		if (hue < 60)
			g = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			g = m2;
		else if (hue < 240)
			g = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			g = m1;
	
		hue = *h - 120;
		while (hue > 360)
			hue -= 360;
		while (hue < 0)
			hue += 360;
	
		if (hue < 60)
			b = m1 + (m2 - m1) * hue / 60;
		else if (hue < 180)
			b = m2;
		else if (hue < 240)
			b = m1 + (m2 - m1) * (240 - hue) / 60;
		else
			b = m1;
	
		*h = r;
		*l = g;
		*s = b;
	}
}

void
shade (const CairoColor * a, CairoColor * b, float k)
{
	double red;
	double green;
	double blue;
	
	red   = a->r;
	green = a->g;
	blue  = a->b;
	
	rgb_to_hls (&red, &green, &blue);
	
	green *= k;
	if (green > 1.0)
		green = 1.0;
	else if (green < 0.0)
		green = 0.0;
	
	blue *= k;
	if (blue > 1.0)
		blue = 1.0;
	else if (blue < 0.0)
		blue = 0.0;
	
	hls_to_rgb (&red, &green, &blue);
	
	b->r = red;
	b->g = green;
	b->b = blue;
}


void gtk_treeview_get_header_index (GtkTreeView *tv, GtkWidget *header,
                                    gint *column_index, gint *columns,
                                    gboolean *resizable)
{
	GList *list, *list_start;
	*column_index = *columns = 0;
	list_start = list = gtk_tree_view_get_columns (tv);

	do
	{
		GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN(list->data);
		if ( column->button == header )
		{
			*column_index = *columns;
			*resizable = column->resizable;
		}
		if ( column->visible )
			(*columns)++;
	} while ((list = g_list_next(list)));

	g_list_free (list_start);
}

void gtk_clist_get_header_index (GtkCList *clist, GtkWidget *button,
                                 gint *column_index, gint *columns)
{
	int i;
	*columns = clist->columns;
	
	for (i=0; i<*columns; i++)
	{
		if (clist->column[i].button == button)
		{
			*column_index = i;
			break;
		}
	}
}

gboolean
sanitize_size (GdkWindow      *window,
               gint           *width,
               gint           *height)
{
	gboolean set_bg = FALSE;
	
	if ((*width == -1) && (*height == -1))
	{
		set_bg = GDK_IS_WINDOW (window);
		gdk_window_get_size (window, width, height);
	}
	else if (*width == -1)
		gdk_window_get_size (window, width, NULL);
	else if (*height == -1)
		gdk_window_get_size (window, NULL, height);
	
	return set_bg;
}

static GtkRequisition default_option_indicator_size = { 7, 13 };
static GtkBorder default_option_indicator_spacing = { 7, 5, 2, 2 };

void
option_menu_get_props (GtkWidget      *widget,
                       GtkRequisition *indicator_size,
                       GtkBorder      *indicator_spacing)
{
	GtkRequisition *tmp_size = NULL;
	GtkBorder *tmp_spacing = NULL;
	
	if (widget)
		gtk_widget_style_get (widget, "indicator_size", &tmp_size,
		                      "indicator_spacing", &tmp_spacing, NULL);
	
	if (tmp_size)
	{
		*indicator_size = *tmp_size;
		g_free (tmp_size);
	}
	else
		*indicator_size = default_option_indicator_size;
	
	if (tmp_spacing)
	{
		*indicator_spacing = *tmp_spacing;
		g_free (tmp_spacing);
	}
	else
		*indicator_spacing = default_option_indicator_spacing;
}

GtkWidget *special_get_ancestor(GtkWidget * widget, GType widget_type)
{
	g_return_val_if_fail(GTK_IS_WIDGET(widget), NULL);

	while (widget && widget->parent
	       && !g_type_is_a(GTK_WIDGET_TYPE(widget->parent),
			       widget_type))
		widget = widget->parent;

	if (!
	    (widget && widget->parent
	     && g_type_is_a(GTK_WIDGET_TYPE(widget->parent), widget_type)))
		return NULL;

	return widget;
}

GtkWidget *get_parent_window (GtkWidget *widget)
{
        GtkWidget *parent = widget->parent;

        while (parent && GTK_WIDGET_NO_WINDOW (parent))
                parent = parent->parent;

        return parent;
}

GdkColor *get_parent_bgcolor (GtkWidget *widget)
{
        GtkWidget *parent = get_parent_window (widget);

        if (parent && parent->style)
                return &parent->style->bg[GTK_STATE_NORMAL];

        return NULL;
}

GtkWidget *
cl_find_combo_box_widget (GtkWidget * widget)
{
	GtkWidget *result = NULL;
	
	if (widget && !GTK_IS_COMBO_BOX_ENTRY (widget))
	{
		if (GTK_IS_COMBO_BOX (widget))
			result = widget;
		else
			result = cl_find_combo_box_widget(widget->parent);
	}
	
	return result;
}

gboolean
cl_is_combo_box (GtkWidget * widget)
{
	return (cl_find_combo_box_widget(widget) != NULL);
}

void
rezlooks_gdk_color_to_rgb (GdkColor *c, double *r, double *g, double *b)
{
	*r = (double)c->red   /  (double)65535;
	*g = (double)c->green /  (double)65535;
	*b = (double)c->blue  /  (double)65535;
}

void
rezlooks_get_parent_bg (const GtkWidget *widget, CairoColor *color)
{
	GtkStateType state_type;
	const GtkWidget *parent;
	GdkColor *gcolor;
	
	if (widget == NULL)
	{
		color->r = color->g = color->b = 255;
		return;
	}
	
	parent = widget->parent;
	
	while (parent && GTK_WIDGET_NO_WINDOW (parent))
		parent = parent->parent;

	if (parent == NULL)
		parent = widget;
	
	state_type = GTK_WIDGET_STATE (parent);
	
	gcolor = &parent->style->bg[state_type];
	
	rezlooks_gdk_color_to_rgb (gcolor, &color->r, &color->g, &color->b);
}

RezlooksStepper
scrollbar_get_stepper (GtkWidget    *widget,
                       GdkRectangle *stepper)
{
	RezlooksStepper value = CL_STEPPER_UNKNOWN;
	GdkRectangle tmp;
	GdkRectangle check_rectangle;
	GtkOrientation orientation;

	g_return_val_if_fail (GTK_IS_RANGE (widget), CL_STEPPER_UNKNOWN);

	check_rectangle.x      = widget->allocation.x;
	check_rectangle.y      = widget->allocation.y;
	check_rectangle.width  = stepper->width;
	check_rectangle.height = stepper->height;
	
	orientation = GTK_RANGE (widget)->orientation;
	
	if (widget->allocation.x == -1 && widget->allocation.y == -1)
		return CL_STEPPER_UNKNOWN;
		
	if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
		value = CL_STEPPER_A;

	if (value == CL_STEPPER_UNKNOWN) /* Haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + stepper->width;
		else
			check_rectangle.y = widget->allocation.y + stepper->height;
		
		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = CL_STEPPER_B;
	}

	if (value == CL_STEPPER_UNKNOWN) /* Still haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + widget->allocation.width - (stepper->width * 2);
		else
			check_rectangle.y = widget->allocation.y + widget->allocation.height - (stepper->height * 2);
		
		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = CL_STEPPER_C;
	}

	if (value == CL_STEPPER_UNKNOWN) /* STILL haven't found a match */
	{
		if (orientation == GTK_ORIENTATION_HORIZONTAL)
			check_rectangle.x = widget->allocation.x + widget->allocation.width - stepper->width;
		else
			check_rectangle.y = widget->allocation.y + widget->allocation.height - stepper->height;
		
		if (gdk_rectangle_intersect (stepper, &check_rectangle, &tmp))
			value = CL_STEPPER_D;
	}
	
	return value;
}

RezlooksStepper
scrollbar_visible_steppers (GtkWidget *widget)
{
	RezlooksStepper steppers = 0;
	
	g_return_val_if_fail (GTK_IS_RANGE (widget), CL_STEPPER_UNKNOWN);
	
	if (GTK_RANGE (widget)->has_stepper_a)
		steppers |= CL_STEPPER_A;
	
	if (GTK_RANGE (widget)->has_stepper_b)
		steppers |= CL_STEPPER_B;

	if (GTK_RANGE (widget)->has_stepper_c)
		steppers |= CL_STEPPER_C;

	if (GTK_RANGE (widget)->has_stepper_d)
		steppers |= CL_STEPPER_D;
}

RezlooksJunction
scrollbar_get_junction (GtkWidget    *widget)
{	
	GtkAdjustment *adj;
	RezlooksJunction junction = CL_JUNCTION_NONE;
	
	g_return_val_if_fail (GTK_IS_RANGE (widget), CL_JUNCTION_NONE);

	adj = GTK_RANGE (widget)->adjustment;
	
	if (adj->value <= adj->lower &&
		(GTK_RANGE (widget)->has_stepper_a || GTK_RANGE (widget)->has_stepper_b))
	{
		junction |= CL_JUNCTION_BEGIN;
	}
	
	if (adj->value >= adj->upper - adj->page_size &&
		(GTK_RANGE (widget)->has_stepper_c || GTK_RANGE (widget)->has_stepper_d))
	{
		junction |= CL_JUNCTION_END;
	}
	
	return junction;
}

gboolean cl_is_panel_widget (GtkWidget *widget)
{
        return widget && (strcmp(G_OBJECT_TYPE_NAME (widget), "PanelApplet") == 0 ||
	                  strcmp(G_OBJECT_TYPE_NAME (widget), "PanelWidget") == 0);
}

