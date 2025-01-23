#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "rezlooks_style.h"
#include "rezlooks_rc_style.h"
#include "rezlooks_draw.h"
#include "support.h"

/* #define DEBUG 1 */

#define SCALE_SIZE 5

#define DETAIL(xx)   ((detail) && (!strcmp(xx, detail)))
#define COMPARE_COLORS(a,b) (a.red == b.red && a.green == b.green && a.blue == b.blue)

#define DRAW_ARGS    GtkStyle       *style, \
                     GdkWindow      *window, \
                     GtkStateType    state_type, \
                     GtkShadowType   shadow_type, \
                     GdkRectangle   *area, \
                     GtkWidget      *widget, \
                     const gchar    *detail, \
                     gint            x, \
                     gint            y, \
                     gint            width, \
                     gint            height

#ifdef HAVE_ANIMATION
#include "animation.h"
#endif

static GtkStyleClass *parent_class;

static cairo_t *
rezlooks_begin_paint (GdkDrawable  *window, GdkRectangle *area)
{
    cairo_t *cr;

    cr = (cairo_t*)gdk_cairo_create (window);

    if (area) {
        cairo_rectangle (cr, area->x, area->y, area->width, area->height);
        cairo_clip (cr);
        cairo_new_path (cr);
    }

    return cr;
}

static void
rezlooks_set_widget_parameters (const GtkWidget      *widget,
                                  const GtkStyle       *style,
                                  GtkStateType          state_type,
                                  WidgetParameters     *params)
{
	if (widget && GTK_IS_ENTRY (widget))
		state_type = GTK_WIDGET_STATE (widget);

	params->active     = (state_type == GTK_STATE_ACTIVE);
	params->prelight   = (state_type == GTK_STATE_PRELIGHT);
	params->disabled   = (state_type == GTK_STATE_INSENSITIVE);			
	params->state_type = (RezlooksStateType)state_type;
	params->corners    = CL_CORNER_ALL;
		
	params->focus      = widget && GTK_WIDGET_HAS_FOCUS (widget);
	params->is_default = widget && GTK_WIDGET_HAS_DEFAULT (widget);
		
	if (!params->active && widget && GTK_IS_TOGGLE_BUTTON (widget))
		params->active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
		
	params->xthickness = style->xthickness;
	params->ythickness = style->ythickness;
		
	/* I want to avoid to have to do this. I need it for GtkEntry, unless I
	   find out why it doesn't behave the way I expect it to. */
	if (widget)
		rezlooks_get_parent_bg (widget, &params->parentbg);
}

static void
draw_flat_box (DRAW_ARGS)
{
	if (detail && 	
	    state_type == GTK_STATE_SELECTED && (
	    !strncmp ("cell_even", detail, 9) ||
	    !strncmp ("cell_odd", detail, 8)))
	{
		RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
		RezlooksColors *colors = &rezlooks_style->colors;
		cairo_t          *cr = rezlooks_begin_paint (window, area);
		CairoColor       fill;

		cairo_translate (cr, x, y);

		if (GTK_WIDGET_HAS_FOCUS (widget))
		{
		    fill = colors->base[state_type];
		}
		else
		{
			fill = colors->base[GTK_STATE_ACTIVE];
		}

		cairo_set_source_rgb (cr, fill.r, fill.g, fill.b);
		cairo_rectangle  (cr, 0, 0, width, height);
		cairo_fill       (cr);

		cairo_destroy (cr);
	}
	else
	{
		parent_class->draw_flat_box (style, window, state_type,
		                             shadow_type,
		                             area, widget, detail,
		                             x, y, width, height);
	}
}

static void
draw_shadow (DRAW_ARGS)
{
	RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;
	cairo_t          *cr     = rezlooks_begin_paint (window, area);

	sanitize_size (window, &width, &height);

	if (DETAIL ("entry") && !(widget && widget->parent && GTK_IS_TREE_VIEW (widget->parent)))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		if (widget && (GTK_IS_COMBO (widget->parent) ||
		               GTK_IS_COMBO_BOX_ENTRY(widget->parent) ||
		               GTK_IS_SPIN_BUTTON (widget)))
		{
			width += style->xthickness;
			params.corners = CL_CORNER_TOPLEFT | CL_CORNER_BOTTOMLEFT;
		}
		
		rezlooks_draw_entry (cr, &rezlooks_style->colors, &params,
		                       x, y, width, height);
	}
	else if (DETAIL ("frame") && widget && GTK_IS_STATUSBAR (widget->parent))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		gtk_style_apply_default_background (style, window, TRUE, state_type,
		                                    area, x, y, width, height);
		
		rezlooks_draw_statusbar (cr, colors, &params,
		                           x, y, width, height);
	}
	else if (DETAIL ("frame"))
	{
		WidgetParameters params;
		FrameParameters  frame;
		frame.shadow  = shadow_type;
		frame.gap_x   = -1;                 /* No gap will be drawn */
		frame.border  = &colors->shade[4];
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		params.corners = CL_CORNER_NONE;
	
		if (!g_str_equal ("XfcePanelWindow", gtk_widget_get_name (gtk_widget_get_toplevel (widget))))
			rezlooks_draw_frame (cr, colors, &params, &frame,
			                       x, y, width, height);
	}
	else if (DETAIL ("scrolled_window") || DETAIL ("viewport") || detail == NULL)
	{
		CairoColor *border = (CairoColor*)&colors->shade[5];
		cairo_rectangle (cr, x+0.5, y+0.5, width-1, height-1);
		cairo_set_source_rgb (cr, border->r, border->g, border->b);
		cairo_set_line_width (cr, 1);
		cairo_stroke (cr);
	}
	else
	{
		WidgetParameters params;
		FrameParameters frame;

		frame.shadow = CL_SHADOW_NONE;
		frame.gap_x  = -1;
		frame.border = &colors->shade[5];
		//printf("draw_shadow: %s %s\n", detail, widget? G_OBJECT_TYPE_NAME (widget) : "null");
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		params.corners = CL_CORNER_NONE;
		
		rezlooks_draw_frame (cr, colors, &params, &frame, x, y, width, height);
	}
	
	cairo_destroy (cr);
}

static void 
draw_box_gap (DRAW_ARGS,
	          GtkPositionType gap_side,
	          gint            gap_x,
	          gint            gap_width)
{
	RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;
	cairo_t          *cr;
	
	cr = rezlooks_begin_paint (window, area);

	if (DETAIL ("notebook"))
	{
		WidgetParameters params;
		FrameParameters  frame;
		
		frame.shadow    = shadow_type;
		frame.gap_side  = gap_side;
		frame.gap_x     = gap_x;
		frame.gap_width = gap_width;
		frame.border    = &colors->shade[6];
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		params.corners = CL_CORNER_NONE;
		
		rezlooks_draw_frame (cr, colors, &params, &frame,
		                       x, y, width, height);
	}
	else
	{
		if (widget)
			printf("box_gap: %s %s\n", detail, G_OBJECT_TYPE_NAME (widget));
		parent_class->draw_box_gap (style, window, state_type, shadow_type,
									   area, widget, detail,
									   x, y, width, height,
									   gap_side, gap_x, gap_width);
	}
	
	cairo_destroy (cr);	
}

static void
draw_extension (DRAW_ARGS, GtkPositionType gap_side)
{
	RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;
	cairo_t          *cr;

	cr = rezlooks_begin_paint (window, area);
	
	if (DETAIL ("tab"))
	{
		WidgetParameters params;
		TabParameters    tab;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		tab.gap_side = (RezlooksGapSide)gap_side;
		
		if (gap_side == GTK_POS_BOTTOM)
			params.corners = CL_CORNER_TOPLEFT | CL_CORNER_TOPRIGHT;
		else if (gap_side == GTK_POS_TOP)
			params.corners = CL_CORNER_BOTTOMLEFT | CL_CORNER_BOTTOMRIGHT;
		
		rezlooks_draw_tab (cr, colors, &params, &tab,
		                     x, y, width, height);
	}
	else
	{
		printf("draw_extension: %s\n", detail);
		parent_class->draw_extension (style, window, state_type, shadow_type, area,
		                              widget, detail, x, y, width, height,
		                              gap_side);

	}
	
	cairo_destroy (cr);
}

static void 
draw_handle (DRAW_ARGS, GtkOrientation orientation)
{
	RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;
	cairo_t          *cr;
	gboolean         is_horizontal;
	
	sanitize_size (window, &width, &height);
	
	cr = rezlooks_begin_paint (window, area);
	
	// Evil hack to work around broken orientation for toolbars
	is_horizontal = (width > height);
	
	if (DETAIL ("handlebox"))
	{
		WidgetParameters params;
		HandleParameters handle;

		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		handle.type = CL_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;
		
		// Is this ever true? -Daniel
		if (GTK_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE)
		{
			cairo_save (cr);
			rezlooks_draw_toolbar (cr, colors, &params, x, y, width, height);
			cairo_restore (cr);
		}
		
		rezlooks_draw_handle (cr, colors, &params, &handle,
		                        x, y, width, height);
	}
	else if (DETAIL ("paned"))
	{
		WidgetParameters params;
		HandleParameters handle;

		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		handle.type = CL_HANDLE_SPLITTER;
		handle.horizontal = orientation == GTK_ORIENTATION_HORIZONTAL;
			
		rezlooks_draw_handle (cr, colors, &params, &handle,
		                        x, y, width, height);
	}
	else
	{
//		printf ("draw_handle: %s %s\n", detail, widget ? G_OBJECT_TYPE_NAME (widget) : "null");
		WidgetParameters params;
		HandleParameters handle;

		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		handle.type = CL_HANDLE_TOOLBAR;
		handle.horizontal = is_horizontal;
		
		// Is this ever true? -Daniel
		if (GTK_IS_TOOLBAR (widget) && shadow_type != GTK_SHADOW_NONE)
		{
			cairo_save (cr);
			rezlooks_draw_toolbar (cr, colors, &params, x, y, width, height);
			cairo_restore (cr);
		}
		
		rezlooks_draw_handle (cr, colors, &params, &handle,
		                        x, y, width, height);
/*
		parent_class->draw_handle (style, window, state_type, shadow_type, area,
		                           widget, detail, x, y, width, height,
		                           orientation);*/
	}

	cairo_destroy (cr);
}

static int
combo_box_get_seperator_pos (GtkWidget *widget)
{
	GList *children, *children_start;
	GtkWidget *child;
	int pos = 0;
	
	if (widget && GTK_IS_COMBO_BOX (widget->parent))
	{
		children = children_start = gtk_container_get_children (GTK_CONTAINER (widget));
		
		if (children && GTK_IS_HBOX (children->data))
		{
			child = GTK_WIDGET (children->data);
			g_list_free (children_start);
			children = children_start = gtk_container_get_children (GTK_CONTAINER (child));
		}

		if (children)
		{
			do
			{
				if (GTK_IS_ARROW (children->data) || GTK_IS_VSEPARATOR (children->data))
					pos += GTK_WIDGET (children->data)->allocation.width;
				
			} while ((children = g_list_next (children)));
		}
	}
	
	pos += 2; /* don't ask me why.... widget->style->xthickness - 1 ? */
	
	g_list_free (children_start);	

	return pos;
}

static void
draw_box (DRAW_ARGS)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	const RezlooksColors *colors;
	cairo_t *cr;

	cr     = rezlooks_begin_paint (window, area);
	colors = &rezlooks_style->colors;

	if ((width == -1) && (height == -1))
		gdk_window_get_size (window, &width, &height);
	else if (width == -1)   
		gdk_window_get_size (window, &width, NULL);
	else if (height == -1)
		gdk_window_get_size (window, NULL, &height);

	if (DETAIL ("menubar") && 
	    !(widget && (cl_is_panel_widget (widget->parent))))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		rezlooks_draw_menubar (cr, colors, &params,
		                         x, y, width, height,
								 rezlooks_style->menubarstyle);
	}
	else if (DETAIL ("button") && widget && widget->parent &&
                 (GTK_IS_TREE_VIEW(widget->parent) ||
                  GTK_IS_CLIST (widget->parent)))
	{
		WidgetParameters params;
		ListViewHeaderParameters header;
		
		gint columns, column_index;
		gboolean resizable = TRUE;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		params.corners = CL_CORNER_NONE;
		
		if (GTK_IS_TREE_VIEW (widget->parent))
		{
			gtk_treeview_get_header_index (GTK_TREE_VIEW(widget->parent),
										   widget, &column_index, &columns,
										   &resizable);
		}
		else if (GTK_IS_CLIST (widget->parent))
		{
			gtk_clist_get_header_index (GTK_CLIST(widget->parent),
										widget, &column_index, &columns);
		}
		
		header.resizable = resizable;
		
		if (column_index == 0)
			header.order = CL_ORDER_FIRST;
		else if (column_index == columns-1)
			header.order = CL_ORDER_LAST;
		else
			header.order = CL_ORDER_MIDDLE;
		
		gtk_style_apply_default_background (style, window, FALSE, state_type, area, x, y, width, height);
		
		rezlooks_draw_list_view_header (cr, colors, &params, &header,
		                                  x, y, width, height);
	}
	else if (DETAIL ("button") || DETAIL ("buttondefault"))
	{
		WidgetParameters params;
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		if ((widget && (GTK_IS_COMBO_BOX_ENTRY (widget->parent) || GTK_IS_COMBO (widget->parent))))
		{
			params.corners = CL_CORNER_TOPRIGHT | CL_CORNER_BOTTOMRIGHT;

			/* Seriously, why can't non-gtk-apps at least try to be decent citizens?
			   Take this fucking OpenOffice.org 1.9 for example. The morons responsible
			   for this utter piece of crap gave the clip size wrong values! :'(  */
/*			cairo_reset_clip (cr);
			cairo_rectangle (cr, x+ 0.5, y+ 0.5, 10, 10);
			cairo_clip (cr);
			cairo_new_path (cr);
*/
			if (params.xthickness > 2)
			{
				x--;
				width++;
			}			
		}
		else
			params.corners    = CL_CORNER_ALL;		
	
		if (GTK_IS_TOGGLE_BUTTON (widget) &&
		    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
			params.active = TRUE;
		
		// Fix some firefox crap.
		if (GTK_IS_BUTTON (widget) && GTK_IS_FIXED (widget->parent) && widget->allocation.x == -1 &&  widget->allocation.y == -1)
		{
			gtk_style_apply_default_background (widget->parent->style, window, TRUE, GTK_STATE_NORMAL,
			                                    area, x, y, width, height);
		}
		
		rezlooks_draw_button (cr, &rezlooks_style->colors, &params,
		                        x, y, width, height);
	}
	else if (DETAIL ("spinbutton_up"))
	{
		if (state_type == GTK_STATE_ACTIVE)
		{
			WidgetParameters params;
			
			if (style->xthickness ==3)
			{
				x--;
				width++;
			}
			height+=2;
			
			params.state_type = (RezlooksStateType)state_type;
			
			params.corners = CL_CORNER_TOPRIGHT;
			rezlooks_draw_spinbutton_down (cr, &rezlooks_style->colors, &params,
											 x, y, width, height);
		}
	}
	else if (DETAIL ("spinbutton_down"))
	{
		if (state_type == GTK_STATE_ACTIVE)
		{
			WidgetParameters params;
			params.state_type = (RezlooksStateType)state_type;
			
			params.corners = CL_CORNER_BOTTOMRIGHT;
			
			if (style->xthickness ==3)
			{
				x--;
				width++;
			}
			rezlooks_draw_spinbutton_down (cr, &rezlooks_style->colors, &params,
											 x, y, width, height);
		}
	}
	else if (DETAIL ("spinbutton"))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		params.corners = CL_CORNER_TOPRIGHT | CL_CORNER_BOTTOMRIGHT;
		
		if (style->xthickness == 3)
		{
			x--;
			width++;
		}
		
		rezlooks_draw_spinbutton (cr, &rezlooks_style->colors, &params,
		                            x, y, width, height);
	}
	else if (DETAIL ("trough") && widget && GTK_IS_SCALE (widget))
	{
		GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (widget));	
		WidgetParameters params;
		SliderParameters slider;
		gint slider_length;
		
		gtk_widget_style_get (widget, "slider-length", &slider_length, NULL);
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		params.corners    = CL_CORNER_NONE;
		
		slider.inverted   = gtk_range_get_inverted (GTK_RANGE (widget));
		slider.horizontal = (GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL);
		slider.fill_size  = ((slider.horizontal ? width : height) - slider_length) * (1 / ((adjustment->upper - adjustment->lower) / (adjustment->value - adjustment->lower))) + slider_length / 2;
		if (slider.horizontal)
			slider.inverted = slider.inverted != (get_direction (widget) == GTK_TEXT_DIR_RTL);
		rezlooks_draw_scale_trough (cr, &rezlooks_style->colors,
		                              &params, &slider,
		                              x, y, width, height);
	}
	else if (DETAIL ("trough") && widget && GTK_IS_PROGRESS_BAR (widget))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);		
		
		rezlooks_draw_progressbar_trough (cr, colors, &params, 
		                                    x, y, width, height);
	}
	else if (DETAIL ("trough") && widget && (GTK_IS_VSCROLLBAR (widget) || GTK_IS_HSCROLLBAR (widget)))
	{
		WidgetParameters params;
		ScrollBarParameters scrollbar;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		params.corners = CL_CORNER_NONE;
		
		scrollbar.horizontal = TRUE;
		scrollbar.junction   = scrollbar_get_junction (widget);
		scrollbar.steppers   = scrollbar_visible_steppers (widget);
		
		if (GTK_IS_RANGE (widget))
			scrollbar.horizontal = GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL;
		
		if (scrollbar.horizontal)
		{
			x += 2;
			width -= 4;
		}
		else
		{
			y += 2;
			height -= 4;
		}
		
		rezlooks_draw_scrollbar_trough (cr, colors, &params, &scrollbar,
		                                  x, y, width, height);
	}
	else if (DETAIL ("bar"))
	{
		WidgetParameters      params;
		ProgressBarParameters progressbar;
		gdouble               elapsed = 0.0;

#ifdef HAVE_ANIMATION
		if(rezlooks_style->animation && CL_IS_PROGRESS_BAR (widget))
		{	
			gboolean activity_mode = GTK_PROGRESS (widget)->activity_mode;
			
		 	if (!activity_mode)
				rezlooks_animation_progressbar_add ((gpointer)widget);
		}

		elapsed = rezlooks_animation_elapsed (widget);
#endif
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		if (widget && GTK_IS_PROGRESS_BAR (widget))
			progressbar.orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));
		else
			progressbar.orientation = CL_ORIENTATION_LEFT_TO_RIGHT;
		
		cairo_reset_clip (cr);
		
		/* The x-1 and width+2 are to make the fill cover the left and
		 * right-hand sides of the trough box */
		rezlooks_draw_progressbar_fill (cr, colors, &params, &progressbar,
		                                  x-1, y, width+2, height,
		                                  10 - (int)(elapsed * 10.0) % 10);
	}
	else if (DETAIL ("hscale") || DETAIL ("vscale"))
	{
		WidgetParameters params;
		//SliderParameters slider;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		//slider.horizontal = DETAIL ("hscale");
		
		//rezlooks_draw_slider_button (cr, &rezlooks_style->colors,
		//                               &params, &slider,
		//                               x, y, width, height);
		rezlooks_draw_button (cr, &rezlooks_style->colors, &params,
		                        x, y, width, height);
	}
	else if (DETAIL ("optionmenu"))
	{
		WidgetParameters params;
		OptionMenuParameters optionmenu;
		
		GtkRequisition indicator_size;
		GtkBorder indicator_spacing;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		option_menu_get_props (widget, &indicator_size, &indicator_spacing);
		
		if (widget && get_direction (widget) == GTK_TEXT_DIR_RTL)
			optionmenu.linepos = (indicator_size.width + indicator_spacing.left + indicator_spacing.right) + style->xthickness;
		else
			optionmenu.linepos = width - (indicator_size.width + indicator_spacing.left + indicator_spacing.right) - style->xthickness;
		
		rezlooks_draw_optionmenu (cr, colors, &params, &optionmenu,
		                            x, y, width, height);
		                            
		
	}
	else if (DETAIL ("menuitem"))
	{
		WidgetParameters params;
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		if (!(GTK_IS_MENU_BAR (widget->parent) && rezlooks_style->menubaritemstyle))
			rezlooks_draw_menuitem (cr, colors, &params, x, y, width, height);

		if (GTK_IS_MENU_BAR (widget->parent) && rezlooks_style->menubaritemstyle)
		{
			params.active = TRUE;
			params.prelight = TRUE;
			params.focus = TRUE;
			params.state_type = CL_STATE_SELECTED;
			params.xthickness = 2;
			params.ythickness = 2;
			
			rezlooks_draw_menubar_button (cr, colors, &params, x, y, width, height+1);
			
			height += 1;
		}
	}
	else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar") || DETAIL ("slider") || DETAIL ("stepper"))
	{
		WidgetParameters    params;
		ScrollBarParameters scrollbar;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		params.corners = CL_CORNER_NONE;
		
		scrollbar.has_color  = FALSE;
		scrollbar.horizontal = TRUE;
		scrollbar.junction   = scrollbar_get_junction (widget);
		scrollbar.steppers   = scrollbar_visible_steppers (widget);
		
		if (GTK_IS_RANGE (widget))
			scrollbar.horizontal = GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL;
		
		if (rezlooks_style->has_scrollbar_color)
		{
			rezlooks_gdk_color_to_rgb (&rezlooks_style->scrollbar_color, &scrollbar.color.r, &scrollbar.color.g, &scrollbar.color.b);
			scrollbar.has_color = TRUE;
		}
		
		if (DETAIL ("slider"))
		{
			rezlooks_draw_scrollbar_slider (cr, colors, &params, &scrollbar,
			                                  x, y, width, height);
		}
		else
		{
			ScrollBarStepperParameters stepper;
			GdkRectangle this_rectangle = { x, y, width, height };

			stepper.stepper = scrollbar_get_stepper (widget, &this_rectangle);

			rezlooks_draw_scrollbar_stepper (cr, colors, &params, &scrollbar, &stepper,
			                                   x, y, width, height);
		}
	}
	else if (DETAIL ("toolbar") || DETAIL ("handlebox_bin") || DETAIL ("dockitem_bin"))
	{
		// Only draw the shadows on horizontal toolbars
		if (shadow_type != GTK_SHADOW_NONE && height < 2*width )
			rezlooks_draw_toolbar (cr, colors, NULL, x, y, width, height);
	}
	else if (DETAIL ("trough"))
	{
		
	}
	else if (DETAIL ("menu"))
	{
		WidgetParameters params;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		
		rezlooks_draw_menu_frame (cr, colors, &params, x, y, width, height);
	}
	else
	{
		//printf("draw_box: %s\n", detail);
		parent_class->draw_box (style, window, state_type, shadow_type, area,
		                        widget, detail, x, y, width, height);
	}
	
	cairo_destroy (cr);
}

static void
draw_slider (DRAW_ARGS, GtkOrientation orientation)
{
	if (DETAIL ("hscale") || DETAIL ("vscale"))
	{
		draw_box (style, window, state_type, shadow_type, area,
		          widget, detail, x, y, width, height);
	}
	else
		parent_class->draw_slider (style, window, state_type, shadow_type, area,
		                           widget, detail, x, y, width, height, orientation);
}

static void
draw_option (DRAW_ARGS)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	CairoColor *border;
	CairoColor *dot;
	double trans = 1.0;
	gboolean draw_bullet = (shadow_type == GTK_SHADOW_IN);

	cairo_t *cr = rezlooks_begin_paint (window, area);
	cairo_pattern_t *pt;

#ifdef HAVE_ANIMATION
	if (rezlooks_style->animation)
		rezlooks_animation_connect_checkbox (widget);
#endif

	if (state_type == GTK_STATE_INSENSITIVE)
	{
		border = &rezlooks_style->colors.shade[2];
		dot    = &rezlooks_style->colors.shade[2];
	}
	else
	{
		border = &rezlooks_style->colors.shade[7];
		dot    = &rezlooks_style->colors.spot[1];
	}
	pt = cairo_pattern_create_linear (0, 0, 13, 13);
	cairo_pattern_add_color_stop_rgba (pt, 0.0, 0, 0, 0, 0.1);
	cairo_pattern_add_color_stop_rgba (pt, 0.5, 0, 0, 0, 0);
	cairo_pattern_add_color_stop_rgba (pt, 0.5, 1, 1, 1, 0);
	cairo_pattern_add_color_stop_rgba (pt, 1.0, 1, 1, 1, 0.5);
	
	cairo_translate (cr, x, y);
	
	cairo_set_line_width (cr, 2);
	cairo_arc       (cr, 7, 7, 6, 0, M_PI*2);	
	cairo_set_source (cr, pt);
	cairo_stroke (cr);
	cairo_pattern_destroy (pt);

	cairo_set_line_width (cr, 1);

	cairo_arc       (cr, 7, 7, 5.5, 0, M_PI*2);	
	
	if (state_type != GTK_STATE_INSENSITIVE)
	{
		CairoColor *bg = &rezlooks_style->colors.base[0];
		cairo_set_source_rgb (cr, bg->r, bg->g, bg->b);
		cairo_fill_preserve (cr);
	}
	
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke (cr);
	
#ifdef HAVE_ANIMATION
	if (rezlooks_style->animation && GTK_IS_CHECK_BUTTON (widget) &&
        rezlooks_animation_is_animated (widget) &&
        !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = rezlooks_animation_elapsed (widget);
	
		if (draw_bullet)
			trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));	
		else
			trans = 1.0 - sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
		
		draw_bullet = TRUE;
	}
#endif

	/* inconsistent state is missing? */
	if (draw_bullet)
	{
		cairo_arc (cr, 7, 7, 3, 0, M_PI*2);
		//cairo_set_source_rgb (cr, dot->r, dot->g, dot->b);
		cairo_set_source_rgba (cr, dot->r, dot->g, dot->b,trans);
		cairo_fill (cr);
		
		cairo_arc (cr, 6, 6, 1, 0, M_PI*2);
		//cairo_set_source_rgba (cr, 1,1,1, 0.5);
		cairo_set_source_rgba (cr, 1,1,1, 0.5+trans);		
		cairo_fill (cr);
	}

	cairo_destroy (cr);
}

static void
draw_check (DRAW_ARGS)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	CairoColor *border;
	CairoColor *dot;
	double trans = 1.0;
	gboolean draw_bullet = (shadow_type == GTK_SHADOW_IN || shadow_type == GTK_SHADOW_ETCHED_IN);

	cairo_t *cr = rezlooks_begin_paint (window, area);
	cairo_pattern_t *pt;

#ifdef HAVE_ANIMATION
	if (rezlooks_style->animation)
		rezlooks_animation_connect_checkbox (widget);
#endif
	
	if (state_type == GTK_STATE_INSENSITIVE)
	{
		border = &rezlooks_style->colors.shade[2];
		dot    = &rezlooks_style->colors.shade[2];
	}
	else
	{
		border = &rezlooks_style->colors.shade[7];
		dot    = &rezlooks_style->colors.spot[1];
	}

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1);
	
	if (style->xthickness > 2 && style->ythickness > 2)
	{
		/* Draw a gradient around the box so it appears sunken. */
		pt = cairo_pattern_create_linear (0, 0, 0, 13);
		cairo_pattern_add_color_stop_rgba (pt, 0.0, 0, 0, 0, 0.04);
		cairo_pattern_add_color_stop_rgba (pt, 0.5, 0, 0, 0, 0);
		cairo_pattern_add_color_stop_rgba (pt, 0.5, 1, 1, 1, 0);
		cairo_pattern_add_color_stop_rgba (pt, 1.0, 1, 1, 1, 0.4);
		
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
		cairo_set_source (cr, pt);
		cairo_stroke (cr);
		cairo_pattern_destroy (pt);
		
		/* Draw the rectangle for the checkbox itself */
		cairo_rectangle (cr, 1.5, 1.5, width-3, height-3);
	}
	else
	{
		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	}
	
	if (state_type != GTK_STATE_INSENSITIVE)
	{
		CairoColor *bg = &rezlooks_style->colors.base[0];
		cairo_set_source_rgb (cr, bg->r, bg->g, bg->b);
		cairo_fill_preserve (cr);
	}
	
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke (cr);
		
#ifdef HAVE_ANIMATION
	if (rezlooks_style->animation && GTK_IS_CHECK_BUTTON (widget) &&
	    rezlooks_animation_is_animated(widget) &&
	    !gtk_toggle_button_get_inconsistent (GTK_TOGGLE_BUTTON (widget)))
	{
		gfloat elapsed = rezlooks_animation_elapsed (widget);
	
		if (draw_bullet)
			trans = sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
		else
			trans = 1.0 - sqrt (sqrt (MIN(elapsed / CHECK_ANIMATION_TIME, 1.0)));
		
		draw_bullet = TRUE;
	}
#endif

	if (draw_bullet)
	{
		if (shadow_type == GTK_SHADOW_ETCHED_IN) /* Inconsistent */
		{
			cairo_set_line_width (cr, 2.0);
			cairo_move_to (cr, 3, height*0.5);
			cairo_line_to (cr, width-3, height*0.5);
		}
		else
		{
			cairo_set_line_width (cr, 1.7);
			cairo_move_to (cr, 0.5 + (width*0.2), (height*0.5));
			cairo_line_to (cr, 0.5 + (width*0.4), (height*0.7));
		
			cairo_curve_to (cr, 0.5 + (width*0.4), (height*0.7),
			                    0.5 + (width*0.5), (height*0.4),
			                    0.5 + (width*0.70), (height*0.25));
		}
		
		cairo_set_source_rgba (cr, dot->r, dot->g, dot->b, trans);
		cairo_stroke (cr);
	}
	cairo_destroy (cr);
}

static void
draw_tab (DRAW_ARGS)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;	
	WidgetParameters params;
	ArrowParameters  arrow;
	
	cairo_t *cr = rezlooks_begin_paint (window, area);
	
	rezlooks_set_widget_parameters (widget, style, state_type, &params);
	arrow.type      = CL_ARROW_COMBO;
	arrow.direction = CL_DIRECTION_DOWN;
	
	rezlooks_draw_arrow (cr, colors, &params, &arrow, x, y, width, height);

	cairo_destroy (cr);
}

static void
draw_vline                      (GtkStyle               *style,
                                 GdkWindow              *window,
                                 GtkStateType            state_type,
                                 GdkRectangle           *area,
                                 GtkWidget              *widget,
                                 const gchar            *detail,
                                 gint                    y1,
                                 gint                    y2,
                                 gint                    x)
{
	SeparatorParameters separator = { FALSE };
	cairo_t *cr;

	cr = rezlooks_begin_paint (window, area);
	
	rezlooks_draw_separator (cr, NULL, NULL, &separator,
	                           x, y1, 2, y2-y1);
	
	cairo_destroy (cr);
}

static void
draw_hline                      (GtkStyle               *style,
                                 GdkWindow              *window,
                                 GtkStateType            state_type,
                                 GdkRectangle           *area,
                                 GtkWidget              *widget,
                                 const gchar            *detail,
                                 gint                    x1,
                                 gint                    x2,
                                 gint                    y)
{
	cairo_t *cr;

	cr = rezlooks_begin_paint (window, area);
	
	if (DETAIL ("label")) /* wtf? */
	{
		printf("draw_vline: label. ermm....?\n");
	}
	else
	{
		SeparatorParameters separator;
		
		separator.horizontal = TRUE;
		
		rezlooks_draw_separator (cr, NULL, NULL, &separator,
		                           x1, y, x2-x1, 2);
	}
	
	cairo_destroy (cr);
}

static void 
draw_shadow_gap (DRAW_ARGS,
                 GtkPositionType gap_side,
                 gint            gap_x,
                 gint            gap_width)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	const RezlooksColors *colors;
	cairo_t *cr;

	cr     = rezlooks_begin_paint (window, area);
	colors = &rezlooks_style->colors;
	
	if (DETAIL ("frame"))
	{
		WidgetParameters params;
		FrameParameters  frame;
		
		frame.shadow    = shadow_type;
		frame.gap_side  = gap_side;
		frame.gap_x     = gap_x;
		frame.gap_width = gap_width;
		frame.border    = (CairoColor*)&colors->shade[5];
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);

		params.corners = CL_CORNER_NONE;
		
		rezlooks_draw_frame (cr, colors, &params, &frame,
		                       x, y, width, height);
	}
	else
	{
		parent_class->draw_shadow_gap (style, window, state_type, shadow_type, area,
									   widget, detail, x, y, width, height,
									   gap_side, gap_x, gap_width);
	}
	
	cairo_destroy (cr);
}

static void
draw_resize_grip (GtkStyle       *style,
                  GdkWindow      *window,
                  GtkStateType    state_type,
                  GdkRectangle   *area,
                  GtkWidget      *widget,
                  const gchar    *detail,
                  GdkWindowEdge   edge,
                  gint            x,
                  gint            y,
                  gint            width,
                  gint            height)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;

	cairo_t *cr;

	WidgetParameters params;
	ResizeGripParameters grip;
	grip.edge = (RezlooksWindowEdge)edge;

	int lx, ly;

	g_return_if_fail (window != NULL);

	if (edge != GDK_WINDOW_EDGE_SOUTH_EAST)
		return; // sorry... need to work on this :P
	
	cr = rezlooks_begin_paint (window, area);

	rezlooks_set_widget_parameters (widget, style, state_type, &params);	

	rezlooks_draw_resize_grip (cr, colors, &params, &grip,
	                             x, y, width, height);

	cairo_destroy (cr);

	return;
}

static void
draw_arrow (GtkStyle  *style,
                       GdkWindow     *window,
                       GtkStateType   state_type,
                       GtkShadowType  shadow,
                       GdkRectangle  *area,
                       GtkWidget     *widget,
                       const gchar   *detail,
                       GtkArrowType   arrow_type,
                       gboolean       fill,
                       gint           x,
                       gint           y,
                       gint           width,
                       gint           height)
{
	RezlooksStyle  *rezlooks_style = REZLOOKS_STYLE (style);
	RezlooksColors *colors = &rezlooks_style->colors;
	cairo_t *cr = rezlooks_begin_paint (window, area);
		
	sanitize_size (window, &width, &height);

	if (DETAIL ("arrow"))
	{
		WidgetParameters params;
		ArrowParameters  arrow;
		
		rezlooks_set_widget_parameters (widget, style, state_type, &params);
		arrow.type = CL_ARROW_NORMAL;
		arrow.direction = (RezlooksDirection)arrow_type;
		
/*		cairo_rectangle (cr, x, y, width, height);
		cairo_set_source_rgb (cr, 1, 0, 0);
		cairo_fill (cr);
	*/	
		if (cl_is_combo_box (widget))
		{
			arrow.type = CL_ARROW_COMBO;
			y -= 2;
			height += 4;
			x += 1;
		}
		
		rezlooks_draw_arrow (cr, colors, &params, &arrow,
		                       x, y, width, height);
	}
	else
	{
		//printf("draw_arrow: %s %s\n", detail, widget ? G_OBJECT_TYPE_NAME (widget) : "null");

		parent_class->draw_arrow (style, window, state_type, shadow, area,
		                          widget, detail, arrow_type, fill,
		                          x, y, width, height);
	}
	
	cairo_destroy (cr);
}

static void
rezlooks_style_init_from_rc (GtkStyle * style,
			       GtkRcStyle * rc_style)
{
	RezlooksStyle *rezlooks_style = REZLOOKS_STYLE (style);
	double shades[] = {1.15, 0.95, 0.896, 0.82, 0.7, 0.665, 0.5, 0.45, 0.4};
	CairoColor spot_color;
	CairoColor bg_normal;
	double contrast;
	int i;
	
	parent_class->init_from_rc (style, rc_style);

	bg_normal.r = style->bg[0].red   / 65535.0;
	bg_normal.g = style->bg[0].green / 65535.0;
	bg_normal.b = style->bg[0].blue  / 65535.0;

	contrast = REZLOOKS_RC_STYLE (rc_style)->contrast;
	
	rezlooks_style->menubarstyle      = REZLOOKS_RC_STYLE (rc_style)->menubarstyle;
	rezlooks_style->menubaritemstyle  = REZLOOKS_RC_STYLE (rc_style)->menubaritemstyle;
	rezlooks_style->menuitemstyle     = REZLOOKS_RC_STYLE (rc_style)->menuitemstyle;
	rezlooks_style->has_scrollbar_color = REZLOOKS_RC_STYLE (rc_style)->has_scrollbar_color;
	rezlooks_style->animation         = REZLOOKS_RC_STYLE (rc_style)->animation;
	
	if (rezlooks_style->has_scrollbar_color)
		rezlooks_style->scrollbar_color = REZLOOKS_RC_STYLE (rc_style)->scrollbar_color;
	
	/* Lighter to darker */
	for (i = 0; i < 9; i++)
	{
		shade (&bg_normal, &rezlooks_style->colors.shade[i],
		       (shades[i]-0.7) * contrast + 0.7);
	}
		
	spot_color.r = style->bg[GTK_STATE_SELECTED].red   / 65535.0;
	spot_color.g = style->bg[GTK_STATE_SELECTED].green / 65535.0;
	spot_color.b = style->bg[GTK_STATE_SELECTED].blue  / 65535.0;
	
	shade (&spot_color, &rezlooks_style->colors.spot[0], 1.42);
	shade (&spot_color, &rezlooks_style->colors.spot[1], 1.05);
	shade (&spot_color, &rezlooks_style->colors.spot[2], 0.65);
	
	for (i=0; i<5; i++)
	{
		rezlooks_gdk_color_to_rgb (&style->bg[i],
		                             &rezlooks_style->colors.bg[i].r,
		                             &rezlooks_style->colors.bg[i].g,
		                             &rezlooks_style->colors.bg[i].b);

		rezlooks_gdk_color_to_rgb (&style->base[i],
		                             &rezlooks_style->colors.base[i].r,
		                             &rezlooks_style->colors.base[i].g,
		                             &rezlooks_style->colors.base[i].b);
	}
}

static void
gdk_cairo_set_source_color_alpha (cairo_t  *cr,
                GdkColor *color, float alpha)
{
  g_return_if_fail (cr != NULL);
  g_return_if_fail (color != NULL);

  cairo_set_source_rgba (cr,
            color->red / 65535.,
            color->green / 65535.,
            color->blue / 65535.,
	        alpha);
}

static void
draw_focus (GtkStyle *style, GdkWindow *window, GtkStateType state_type,
            GdkRectangle *area, GtkWidget *widget, const gchar *detail,
            gint x, gint y, gint width, gint height)
{
	cairo_t *cr;
	gboolean free_dash_list = FALSE;
	gint line_width = 1;
	gint8 *dash_list = "\1\1";

	if (widget)
	{
		gtk_widget_style_get (widget,
				      "focus-line-width", &line_width,
				      "focus-line-pattern",
				      (gchar *) & dash_list, NULL);

		free_dash_list = TRUE;
	}

	if (detail && !strcmp (detail, "add-mode"))
	{
		if (free_dash_list)
			g_free (dash_list);

		dash_list = "\4\4";
		free_dash_list = FALSE;
	}

	sanitize_size (window, &width, &height);

	cr = gdk_cairo_create (window);

	if (detail && !strcmp (detail, "colorwheel_light"))
		cairo_set_source_rgb (cr, 0., 0., 0.);
	else if (detail && !strcmp (detail, "colorwheel_dark"))
		cairo_set_source_rgb (cr, 1., 1., 1.);
	else
		gdk_cairo_set_source_color_alpha (cr, &style->fg[state_type],
						  0.7);

	cairo_set_line_width (cr, line_width);

	if (dash_list[0])
	{
		gint n_dashes = strlen (dash_list);
		gdouble *dashes = g_new (gdouble, n_dashes);
		gdouble total_length = 0;
		gdouble dash_offset;
		gint i;

		for (i = 0; i < n_dashes; i++)
		{
			dashes[i] = dash_list[i];
			total_length += dash_list[i];
		}

		/* The dash offset here aligns the pattern to integer pixels
		 * by starting the dash at the right side of the left border
		 * Negative dash offsets in cairo don't work
		 * (https://bugs.freedesktop.org/show_bug.cgi?id=2729)
		 */
		dash_offset = -line_width / 2.;
		while (dash_offset < 0)
			dash_offset += total_length;

		cairo_set_dash (cr, dashes, n_dashes, dash_offset);
		g_free (dashes);
	}

	if (area)
	{
		gdk_cairo_rectangle (cr, area);
		cairo_clip (cr);
	}

	cairo_rectangle (cr,
			 x + line_width / 2.,
			 y + line_width / 2.,
			 width - line_width, height - line_width);
	cairo_stroke (cr);
	cairo_destroy (cr);

	if (free_dash_list)
		g_free (dash_list);
}

static void
rezlooks_style_copy (GtkStyle * style, GtkStyle * src)
{
	RezlooksStyle * cl_style = REZLOOKS_STYLE (style);
	RezlooksStyle * cl_src = REZLOOKS_STYLE (src);
	
	cl_style->colors              = cl_src->colors;
	cl_style->progressbarstyle    = cl_src->progressbarstyle;
	cl_style->menubarstyle        = cl_src->menubarstyle;
	cl_style->menubaritemstyle    = cl_src->menubaritemstyle;
	cl_style->menuitemstyle       = cl_src->menuitemstyle;
	cl_style->listviewitemstyle   = cl_src->listviewitemstyle;
	cl_style->scrollbar_color     = cl_src->scrollbar_color;
	cl_style->has_scrollbar_color = cl_src->has_scrollbar_color;
	cl_style->animation           = cl_src->animation;
	
	parent_class->copy (style, src);
}

static void
rezlooks_style_unrealize (GtkStyle * style)
{
	parent_class->unrealize (style);
}

static GdkPixbuf *
set_transparency (const GdkPixbuf *pixbuf, gdouble alpha_percent)
{
	GdkPixbuf *target;
	guchar *data, *current;
	guint x, y, rowstride, height, width;

	g_return_val_if_fail (pixbuf != NULL, NULL);
	g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

	/* Returns a copy of pixbuf with it's non-completely-transparent pixels to
	   have an alpha level "alpha_percent" of their original value. */

	target = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

	if (alpha_percent == 1.0)
		return target;
	width = gdk_pixbuf_get_width (target);
	height = gdk_pixbuf_get_height (target);
	rowstride = gdk_pixbuf_get_rowstride (target);
	data = gdk_pixbuf_get_pixels (target);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			/* The "4" is the number of chars per pixel, in this case, RGBA,
			   the 3 means "skip to the alpha" */
			current = data + (y * rowstride) + (x * 4) + 3; 
			*(current) = (guchar) (*(current) * alpha_percent);
		}
	}

	return target;
}

static GdkPixbuf*
scale_or_ref (GdkPixbuf *src,
              int width,
              int height)
{
	if (width == gdk_pixbuf_get_width (src) &&
	    height == gdk_pixbuf_get_height (src)) {
		return g_object_ref (src);
	} else {
		return gdk_pixbuf_scale_simple (src,
                                        width, height,
                                        GDK_INTERP_BILINEAR);
	}
}

static GdkPixbuf *
render_icon (GtkStyle            *style,
             const GtkIconSource *source,
             GtkTextDirection     direction,
             GtkStateType         state,
             GtkIconSize          size,
             GtkWidget           *widget,
             const char          *detail)
{
	int width = 1;
	int height = 1;
	GdkPixbuf *scaled;
	GdkPixbuf *stated;
	GdkPixbuf *base_pixbuf;
	GdkScreen *screen;
	GtkSettings *settings;
	
	/* Oddly, style can be NULL in this function, because
	 * GtkIconSet can be used without a style and if so
	 * it uses this function.
	 */
	
	base_pixbuf = gtk_icon_source_get_pixbuf (source);
	
	g_return_val_if_fail (base_pixbuf != NULL, NULL);
	
	if (widget && gtk_widget_has_screen (widget)) {
		screen = gtk_widget_get_screen (widget);
		settings = gtk_settings_get_for_screen (screen);
	} else if (style->colormap) {
		screen = gdk_colormap_get_screen (style->colormap);
		settings = gtk_settings_get_for_screen (screen);
	} else {
		settings = gtk_settings_get_default ();
		GTK_NOTE (MULTIHEAD,
			  g_warning ("Using the default screen for gtk_default_render_icon()"));
	}
	
  
	if (size != (GtkIconSize) -1 && !gtk_icon_size_lookup_for_settings (settings, size, &width, &height)) {
		g_warning (G_STRLOC ": invalid icon size '%d'", size);
		return NULL;
	}

	/* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
	 * leave it alone.
	 */
	if (size != (GtkIconSize)-1 && gtk_icon_source_get_size_wildcarded (source))
		scaled = scale_or_ref (base_pixbuf, width, height);
	else
		scaled = g_object_ref (base_pixbuf);
	
	/* If the state was wildcarded, then generate a state. */
	if (gtk_icon_source_get_state_wildcarded (source)) {
		if (state == GTK_STATE_INSENSITIVE) {
			stated = set_transparency (scaled, 0.3);
#if 0
			stated =
				gdk_pixbuf_composite_color_simple (scaled,
								   gdk_pixbuf_get_width (scaled),
								   gdk_pixbuf_get_height (scaled),
								   GDK_INTERP_BILINEAR, 128,
								   gdk_pixbuf_get_width (scaled),
								   style->bg[state].pixel,
								   style->bg[state].pixel);
#endif
			gdk_pixbuf_saturate_and_pixelate (stated, stated,
							  0.1, FALSE);
			
			g_object_unref (scaled);
		} else if (state == GTK_STATE_PRELIGHT) {
			stated = gdk_pixbuf_copy (scaled);      
			
			gdk_pixbuf_saturate_and_pixelate (scaled, stated,
							  1.2, FALSE);
			
			g_object_unref (scaled);
		} else {
			stated = scaled;
		}
	}
	else
		stated = scaled;
  
  return stated;
}

static void
rezlooks_style_init (RezlooksStyle * style)
{
}

static void
rezlooks_style_class_init (RezlooksStyleClass * klass)
{
	GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);
	
	parent_class = g_type_class_peek_parent (klass);

	style_class->copy             = rezlooks_style_copy;
	style_class->unrealize        = rezlooks_style_unrealize;
	style_class->init_from_rc     = rezlooks_style_init_from_rc;
	style_class->draw_handle      = draw_handle;
	style_class->draw_slider      = draw_slider;
	style_class->draw_shadow_gap  = draw_shadow_gap;
	style_class->draw_focus       = draw_focus;
	style_class->draw_box         = draw_box;
	style_class->draw_shadow      = draw_shadow;
	style_class->draw_box_gap     = draw_box_gap;
	style_class->draw_extension   = draw_extension;
	style_class->draw_option      = draw_option;
	style_class->draw_check       = draw_check;
	style_class->draw_flat_box    = draw_flat_box;
	style_class->draw_tab         = draw_tab;
	style_class->draw_vline       = draw_vline;
	style_class->draw_hline       = draw_hline;
	style_class->draw_resize_grip = draw_resize_grip;
	style_class->draw_arrow       = draw_arrow;
	style_class->render_icon      = render_icon;
}

GType rezlooks_type_style = 0;

void
rezlooks_style_register_type (GTypeModule * module)
{
	static const GTypeInfo object_info =
	{
		sizeof (RezlooksStyleClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) rezlooks_style_class_init,
		NULL,         /* class_finalize */
		NULL,         /* class_data */
		sizeof (RezlooksStyle),
		0,            /* n_preallocs */
		(GInstanceInitFunc) rezlooks_style_init,
		NULL
	};

	rezlooks_type_style = g_type_module_register_type (module,
	                                                     GTK_TYPE_STYLE,
	                                                     "RezlooksStyle",
	                                                     &object_info, 0);
}
