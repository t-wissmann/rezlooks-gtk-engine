#include "rezlooks_draw.h"
#include "rezlooks_style.h"
#include "rezlooks_types.h"

#include "support.h"

#include <cairo.h>

#define M_PI 3.14159265358979323846


/* KEEP IN MIND, MIRROR TAKES PLACE BEFORE ROTATION */
/* ROTATES ANTI-CLOCKWISE, I THINK :P */
/* TODO: Do I really need THREE matrices? */
static void
rotate_mirror_translate (cairo_t *cr, double radius, double x, double y,
                         boolean mirror_horizontally, boolean mirror_vertically)
{
	cairo_matrix_t matrix_rotate;
	cairo_matrix_t matrix_mirror;
	cairo_matrix_t matrix_result;
	
	double r_cos = cos(radius);
	double r_sin = sin(radius);
	
	cairo_matrix_init (&matrix_rotate, r_cos,
	                                   r_sin,
	                                   r_sin,
	                                   r_cos,
	                                   x, y);
	
	cairo_matrix_init (&matrix_mirror, mirror_horizontally ? -1 : 1,
	                                   0,
	                                   0,
	                                   mirror_vertically ? -1 : 1,
									   0, 0);

	cairo_matrix_multiply (&matrix_result, &matrix_mirror, &matrix_rotate);

	cairo_set_matrix (cr, &matrix_result);
}

void
rezlooks_draw_button (cairo_t *cr,
                        const RezlooksColors *colors,
                        const WidgetParameters *params,
                        int x, int y, int width, int height)
{
	double xoffset = 0, yoffset = 0;
	const CairoColor *fill            = &colors->bg[params->state_type];
	const CairoColor *border_normal   = &colors->shade[5];
	const CairoColor *border_disabled = &colors->shade[4];

	CairoColor hilight;
	shade (fill, &hilight, 1.1);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1.0);

	if (params->xthickness == 3)
		xoffset = 1;
	if (params->ythickness == 3)
		yoffset = 1;

	/* Drawing the border */
	if (params->disabled)
	{
		cairo_set_source_rgb (cr, border_disabled->r,
		                          border_disabled->g,
		                          border_disabled->b);
	}
	else
	{
		cairo_set_source_rgb (cr, border_normal->r,
		                          border_normal->g,
		                          border_normal->b);

	}
	
	cairo_rectangle (cr, xoffset + 0.5, yoffset + 0.5,
                         width-(xoffset*2)-1, height-(yoffset*2)-1);
	cairo_stroke (cr);

	/* drawing the inner border */
	cairo_rectangle (cr, xoffset + 0.5, yoffset + 0.5,
                         width-(xoffset*2)-1, height-(yoffset*2)-1);
	cairo_set_source_rgb (cr, hilight.r, hilight.g, hilight.b);
	cairo_fill (cr);

	/* filling in the button background */
	cairo_rectangle (cr, xoffset+2, yoffset+2,
	                     width-(xoffset*2)-4,
	                     height-(yoffset*2)-4);
	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);
	cairo_fill (cr);
}

void
rezlooks_draw_menubar_button (cairo_t *cr,
                        const RezlooksColors *colors,
                        const WidgetParameters *params,
                        int x, int y, int width, int height)
{
	double xoffset = 0, yoffset = 0;
	const CairoColor *fill            = &colors->bg[params->state_type];
	const CairoColor *border_normal   = &colors->shade[6];
	const CairoColor *border_disabled = &colors->shade[4];

	CairoColor hilight;
	shade (fill, &hilight, 1.1);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1.0);

	if (params->xthickness == 3)
		xoffset = 1;
	if (params->ythickness == 3)
		yoffset = 1;

	/* Drawing the border */
	cairo_set_source_rgb (cr, border_normal->r,
							  border_normal->g,
							  border_normal->b);
	
	cairo_rectangle (cr, xoffset + 0.5, yoffset + 0.5,
                         width-(xoffset*2)-1, height-(yoffset*2)-1);
	cairo_stroke (cr);

	/* drawing the inner border */
	cairo_rectangle (cr, xoffset + 0.5, yoffset + 0.5,
                         width-(xoffset*2)-1, height-(yoffset));
	cairo_set_source_rgb (cr, hilight.r, hilight.g, hilight.b);
	cairo_fill (cr);

	/* filling in the button background */
	cairo_rectangle (cr, xoffset+2, yoffset+2,
	                     width-(xoffset*2)-4,
	                     height-(yoffset));
	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);
	cairo_fill (cr);
}

void
rezlooks_draw_entry (cairo_t *cr,
                       const RezlooksColors *colors,
                       const WidgetParameters *params,
                       int x, int y, int width, int height)
{
	CairoColor *base = (CairoColor*)&colors->base[params->state_type];
	CairoColor *border;
	
	if (params->focus)
		border = (CairoColor*)&colors->spot[2];
	else
		border = (CairoColor*)&colors->shade[params->disabled ? 4 : 6];

	cairo_translate (cr, x+0.5, y+0.5);
	cairo_set_line_width (cr, 1.0);
	
	/* Fill the background (shouldn't have to) */
	cairo_rectangle (cr, -0.5, -0.5, width, height);
	cairo_set_source_rgb (cr, params->parentbg.r, 
	                          params->parentbg.g, 
	                          params->parentbg.b);
	cairo_fill (cr);

	/* Fill the entry's base color (why isn't is large enough by default?) */
	cairo_rectangle (cr, 1.5, 1.5, width-4, height-4);
	cairo_set_source_rgb (cr, base->r, 
	                          base->g, 
	                          base->b);
	cairo_fill (cr);
	
	/* Draw the border */
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_rectangle (cr, 1, 1, width-3, height-3);
	cairo_stroke (cr);

	/* Draw the focused border */
	if (params->focus)
	{
		cairo_rectangle (cr, 2, 2, width-5, height-5);
		cairo_set_source_rgb (cr, colors->spot[1].r, colors->spot[1].g, colors->spot[1].b);
		cairo_stroke (cr);
	}
}

void
rezlooks_draw_spinbutton (cairo_t *cr,
                            const RezlooksColors *colors,
                            const WidgetParameters *params,
                            int x, int y, int width, int height)
{
	rezlooks_draw_button (cr, colors, params, x, y, width, height);
	
	/* Seperator */
	cairo_move_to (cr, params->xthickness,         (height/2));
	cairo_line_to (cr, width-params->xthickness-1, (height/2));
	cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.3);
	cairo_stroke (cr);

	cairo_move_to (cr, params->xthickness,         (height/2)+1);
	cairo_line_to (cr, width-params->xthickness-1, (height/2)+1);
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.7);
	cairo_stroke (cr);	
}

void
rezlooks_draw_spinbutton_down (cairo_t *cr,
                                 const RezlooksColors *colors,
                                 const WidgetParameters *params,
                                 int x, int y, int width, int height)
{
	cairo_pattern_t *pattern;
	
	cairo_translate (cr, x+1, y+1);
	
	cairo_rectangle (cr, 1, 1, width-4, height-4);
	
	cairo_set_source_rgb (cr, colors->bg[params->state_type].r,
	                          colors->bg[params->state_type].g, 
	                          colors->bg[params->state_type].b);
	
	cairo_fill_preserve (cr);
	
	pattern = cairo_pattern_create_linear (0, 0, 0, height);
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, 0.0, 0.0, 0.0, 0.3);
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, 0.0, 0.0, 0.0, 0.0);
	
	cairo_set_source (cr, pattern);
	cairo_fill (cr);
	
	cairo_pattern_destroy (pattern);
}

static void
rezlooks_scale_draw_gradient (cairo_t *cr,
                                const CairoColor *c1,
                                const CairoColor *c2,
                                int x, int y, int width, int height,
                                boolean horizontal)
{
	cairo_set_source_rgb (cr, c1->r, c1->g, c1->b);

	cairo_rectangle (cr, x, y, width, height);	
	cairo_fill (cr);
	
	cairo_rectangle (cr, x, y, width, height);	
	cairo_set_source_rgb (cr, c2->r, c2->g, c2->b);
	cairo_stroke (cr);
}

#define TROUGH_SIZE 6
void
rezlooks_draw_scale_trough (cairo_t *cr,
                              const RezlooksColors *colors,
                              const WidgetParameters *params,
                              const SliderParameters *slider,
                              int x, int y, int width, int height)
{
	int     fill_x, fill_y, fill_width, fill_height; /* Fill x,y,w,h */
	int     trough_width, trough_height;
	double  translate_x, translate_y;
	int     fill_size = slider->fill_size;

	if (slider->horizontal)
	{
		if (fill_size > width-3)
			fill_size = width-3;

		fill_x        = slider->inverted ? width - fill_size - 3 : 0;
		fill_y        = 0;
		fill_width    = fill_size;			
		fill_height   = TROUGH_SIZE-2;
		
		trough_width  = width-3;
		trough_height = TROUGH_SIZE-2;
		
		translate_x   = x + 0.5;
		translate_y   = y + 0.5 + (height/2) - (TROUGH_SIZE/2);
	}
	else
	{
		if (fill_size > height-3)
			fill_size = height-3;

		fill_x        = 0;
		fill_y        = slider->inverted ? height - fill_size - 3 : 0;
		fill_width    = TROUGH_SIZE-2;
		fill_height   = fill_size;			
		
		trough_width  = TROUGH_SIZE-2;
		trough_height = height-3;
		
		translate_x   = x + 0.5 + (width/2) - (TROUGH_SIZE/2);
		translate_y  = y + 0.5;
	}

	cairo_set_line_width (cr, 1.0);
	cairo_translate (cr, translate_x, translate_y);
	
	cairo_translate (cr, 1, 1);
	rezlooks_scale_draw_gradient (cr, &colors->shade[1],
	                                    &colors->shade[3], /* border */
	                                    0, 0, trough_width, trough_height,
	                                    slider->horizontal);
	
	rezlooks_scale_draw_gradient (cr, &colors->spot[1],
	                                    &colors->spot[2], /* border */
	                                    fill_x, fill_y, fill_width, fill_height,
	                                    slider->horizontal);
}

void
rezlooks_draw_slider_button (cairo_t *cr,
                               const RezlooksColors *colors,
                               const WidgetParameters *params,
                               const SliderParameters *slider,
                               int x, int y, int width, int height)
{
	CairoColor      *border = (CairoColor*)&colors->shade[6];

	cairo_set_line_width (cr, 1.0);
	
	if (slider->horizontal)
		rotate_mirror_translate (cr, 0, x+0.5, y+0.5, FALSE, FALSE);
	else
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x+0.5, y+0.5, FALSE, FALSE);
		height = width;
		width = tmp;
	}
	
	cairo_rectangle (cr, 0.5, 0.5, width-2, height-2);

	/* Fill the damn thing */
	cairo_set_source_rgb (cr, colors->bg[params->state_type].r,
	                          colors->bg[params->state_type].g,
	                          colors->bg[params->state_type].b);
	cairo_fill (cr);

	/* Draw the border */
	cairo_rectangle (cr, 1, 1, width-3, height-3);
	cairo_set_source_rgb (cr, border->r,
	                          border->g,
	                          border->b);
	cairo_stroke (cr);
}

void
rezlooks_draw_progressbar_trough (cairo_t *cr,
                                    const RezlooksColors *colors,
                                    const WidgetParameters *params,
                                    int x, int y, int width, int height)
{
	CairoColor      *border = (CairoColor*)&colors->spot[2];
	cairo_pattern_t *pattern;
	
	cairo_set_line_width (cr, 1.0);
	
	/* Fill with bg color */
	cairo_set_source_rgb (cr, colors->shade[params->state_type].r,
	                          colors->bg[params->state_type].g,
	                          colors->bg[params->state_type].b);
	
	cairo_rectangle (cr, x, y, width, height);	
	cairo_fill (cr);

	/* Create trough box */
	cairo_rectangle (cr, x+1, y+1, width-2, height-2);
	cairo_set_source_rgb (cr, colors->shade[1].r, colors->shade[1].g, colors->shade[1].b );
	cairo_fill (cr);

	/* Draw border */
	cairo_rectangle (cr, x+0.5, y+0.5, width-1, height-1);
	cairo_set_source_rgb (cr, border->r,
	                          border->g,
	                          border->b);
	cairo_stroke (cr);
}

void
rezlooks_draw_progressbar_fill (cairo_t *cr,
                                  const RezlooksColors *colors,
                                  const WidgetParameters *params,
                                  const ProgressBarParameters *progressbar,
                                  int x, int y, int width, int height,
                                  gint offset)
{
	boolean		is_horizontal = progressbar->orientation < 2;
	double		tile_pos = 0;
	double		stroke_width;
	int			x_step;

	cairo_rectangle (cr, x, y, width, height);
	cairo_clip (cr);
	cairo_new_path (cr);

	if (is_horizontal)
	{
		if (progressbar->orientation == CL_ORIENTATION_LEFT_TO_RIGHT)
			rotate_mirror_translate (cr, 0, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, 0, x+width, y, TRUE, FALSE);
	}
	else
	{
		int tmp = height;
		height  = width;
		width   = tmp;

		if (progressbar->orientation == CL_ORIENTATION_TOP_TO_BOTTOM)
			rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		else
			rotate_mirror_translate (cr, M_PI/2, x, y+width, TRUE, FALSE);
	}
	
	stroke_width = height*2;

	x_step = (((float)stroke_width/10)*offset);

	cairo_set_line_width (cr, 1.0);
	
	cairo_save (cr);
	cairo_rectangle (cr, 0, 0, width, height);
		
	/* Draw fill */
	cairo_set_source_rgb (cr, colors->spot[1].r,
	                          colors->spot[1].g,
	                          colors->spot[1].b);
	cairo_fill (cr);
	
	/* Draw strokes */
	while (tile_pos <= width+x_step-2)
	{
		cairo_move_to (cr, stroke_width/2-x_step, 0);
		cairo_line_to (cr, stroke_width-x_step,   0);
		cairo_line_to (cr, stroke_width/2-x_step, height);
		cairo_line_to (cr, -x_step, height);
		
		cairo_translate (cr, stroke_width, 0);
		tile_pos += stroke_width;
	}

	cairo_set_source_rgba (cr, colors->spot[2].r,
	                           colors->spot[2].g,
	                           colors->spot[2].b,
	                           0.15);
	
	cairo_fill (cr);
	
	cairo_restore (cr);
	
	cairo_set_source_rgba (cr, colors->spot[0].r, colors->spot[0].g, colors->spot[0].b, 0.5);
	cairo_rectangle (cr, 1.5, 0.5, width-3, height-1);
	cairo_stroke (cr);
	
	
	/* Start/End of bar line */
	cairo_set_source_rgb (cr, colors->spot[2].r,
	                           colors->spot[2].g,
	                           colors->spot[2].b);
	cairo_move_to (cr, 0.5, 0);
	cairo_line_to (cr, 0.5, height);
	cairo_stroke (cr);
	cairo_set_source_rgb (cr, colors->spot[2].r,
	                           colors->spot[2].g,
	                           colors->spot[2].b);
	cairo_move_to (cr, width+0.5, 0);
	cairo_line_to (cr, width+0.5, height);
	cairo_stroke (cr);
}

void
rezlooks_draw_optionmenu (cairo_t *cr,
                            const RezlooksColors *colors,
                            const WidgetParameters *params,
                            const OptionMenuParameters *optionmenu,
                            int x, int y, int width, int height)
{
	int offset = params->ythickness + 1;
	
	rezlooks_draw_button (cr, colors, params, x, y, width, height);
	
	cairo_set_line_width  (cr, 1.0);
	cairo_translate       (cr, optionmenu->linepos, 0.5);
	
	cairo_move_to         (cr, 0.0, offset);
	cairo_line_to         (cr, 0.0, height - offset - params->ythickness + 1);
	cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.2);
	cairo_stroke          (cr);
	
	cairo_move_to         (cr, 1.0, offset);
	cairo_line_to         (cr, 1.0, height - offset - params->ythickness + 1);
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.8);
	cairo_stroke          (cr);
}

void
rezlooks_draw_menubar (cairo_t *cr,
                         const RezlooksColors *colors,
                         const WidgetParameters *params,
                         int x, int y, int width, int height,
						 int menubarstyle)
{
	CairoColor lower;
	cairo_pattern_t *pattern;
	
	cairo_translate (cr, x, y);
	cairo_rectangle (cr, 0, 0, width, height);

	if (menubarstyle > 0) {
		/* Draw the gradient */
		if (menubarstyle == 2)
			shade (&colors->bg[0], &lower, 0.8);
		else
			shade (&colors->bg[0], &lower, 0.95);
		pattern = cairo_pattern_create_linear (0, 0, 0, height);
		cairo_pattern_add_color_stop_rgb (pattern, 0.0, colors->bg[0].r,
	                                                    colors->bg[0].g,
	                                                    colors->bg[0].b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0, lower.r,
	                                                    lower.g,
	                                                    lower.b);
		cairo_set_source      (cr, pattern);
		cairo_fill            (cr);
		cairo_pattern_destroy (pattern);
	} else {
		/* Flat fill */
		cairo_set_source_rgb (cr, colors->bg[0].r,
				                  colors->bg[0].g,
								  colors->bg[0].b);
		cairo_fill (cr);
	}
	if (menubarstyle == 2) {
		int counter = -height;
		cairo_set_line_width  (cr, 1.0);
		shade (&colors->bg[0], &lower, 0.8);
		cairo_set_source_rgb  (cr, lower.r, lower.g, lower.b);
		while (counter < width) {
			cairo_move_to     (cr, counter, height);
			cairo_line_to     (cr, counter+height, 0);
			cairo_stroke      (cr);
			counter += 3;
		}
	}
	
	/* Draw bottom line */
	cairo_set_line_width  (cr, 1.0);
	cairo_move_to         (cr, 0, height-0.5);
	cairo_line_to         (cr, width, height-0.5);
	cairo_set_source_rgb  (cr, colors->shade[3].r,
	                           colors->shade[3].g,
	                           colors->shade[3].b);
	cairo_stroke          (cr);
}

static void
rezlooks_get_frame_gap_clip (int x, int y, int width, int height, 
                               FrameParameters     *frame,
                               RezlooksRectangle *bevel,
                               RezlooksRectangle *border)
{
	if (frame->gap_side == CL_GAP_TOP)
	{
		REZLOOKS_RECTANGLE_SET ((*bevel),  1.5 + frame->gap_x,  -0.5,
											 frame->gap_width - 3, 2.0);
		REZLOOKS_RECTANGLE_SET ((*border), 0.5 + frame->gap_x,  -0.5,
											 frame->gap_width - 2, 2.0);
	}
	else if (frame->gap_side == CL_GAP_BOTTOM)
	{
		REZLOOKS_RECTANGLE_SET ((*bevel),  1.5 + frame->gap_x,  height - 2.5,
											 frame->gap_width - 3, 2.0);
		REZLOOKS_RECTANGLE_SET ((*border), 0.5 + frame->gap_x,  height - 1.5,
											 frame->gap_width - 2, 2.0);		
	}
	else if (frame->gap_side == CL_GAP_LEFT)
	{
		REZLOOKS_RECTANGLE_SET ((*bevel),  -0.5, 1.5 + frame->gap_x,
											 2.0, frame->gap_width - 3);
		REZLOOKS_RECTANGLE_SET ((*border), -0.5, 0.5 + frame->gap_x,
											 1.0, frame->gap_width - 2);			
	}
	else if (frame->gap_side == CL_GAP_RIGHT)
	{
		REZLOOKS_RECTANGLE_SET ((*bevel),  width - 2.5, 1.5 + frame->gap_x,
											 2.0, frame->gap_width - 3);
		REZLOOKS_RECTANGLE_SET ((*border), width - 1.5, 0.5 + frame->gap_x,
											 1.0, frame->gap_width - 2);			
	}
}

void
rezlooks_draw_frame            (cairo_t *cr,
                                  const RezlooksColors     *colors,
                                  const WidgetParameters     *params,
                                  const FrameParameters      *frame,
                                  int x, int y, int width, int height)
{
	CairoColor *border = frame->border;
	RezlooksRectangle bevel_clip;
	RezlooksRectangle frame_clip;
	
	if (frame->shadow == CL_SHADOW_NONE)
		return;
	
	if (frame->gap_x != -1)
		rezlooks_get_frame_gap_clip (x, y, width, height,
		                               (FrameParameters*)frame,
		                               &bevel_clip, &frame_clip);
	
	cairo_set_line_width (cr, 1.0);
	cairo_translate      (cr, x+0.5, y+0.5);
	
	/* Set clip for the bevel */
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule  (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle      (cr, -0.5, -0.5, width, height);
		cairo_rectangle      (cr, bevel_clip.x, bevel_clip.y, bevel_clip.width, bevel_clip.height);
		cairo_clip           (cr);
		cairo_new_path       (cr);
	}
	
	/* Set clip for the frame */
	cairo_reset_clip     (cr);
	if (frame->gap_x != -1)
	{
		/* Set clip for gap */
		cairo_set_fill_rule  (cr, CAIRO_FILL_RULE_EVEN_ODD);
		cairo_rectangle      (cr, -0.5, -0.5, width, height);
		cairo_rectangle      (cr, frame_clip.x, frame_clip.y, frame_clip.width, frame_clip.height);
		cairo_clip           (cr);
		cairo_new_path       (cr);
	}

	/* Draw frame */
	if (frame->shadow == CL_SHADOW_ETCHED_IN || frame->shadow == CL_SHADOW_ETCHED_OUT)
	{
		cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.2 );
		if (frame->shadow == CL_SHADOW_ETCHED_IN)
			cairo_rectangle       (cr, 0, 0, width-2, height-2);
		else
			cairo_rectangle       (cr, 1, 1, width-2, height-2);
	}
	else
	{
		cairo_set_source_rgb (cr, border->r, border->g, border->b );
		cairo_rectangle      (cr, 0, 0, width-1, height-1);
	}
	cairo_stroke         (cr);
}

void
rezlooks_draw_tab (cairo_t *cr,
                     const RezlooksColors *colors,
                     const WidgetParameters *params,
                     const TabParameters    *tab,
                     int x, int y, int width, int height)
{
	const float RADIUS = 3.0;
	CairoColor          *stripe_fill   = (CairoColor*)&colors->spot[1];
	CairoColor          *stripe_border = (CairoColor*)&colors->spot[2];
	CairoColor          *fill;
	CairoColor          *border1;

	cairo_pattern_t		*pattern;

	int                  corners;
	double               strip_size;

	if (!params->active)
		border1 = (CairoColor*)&colors->shade[6];
	else
		border1 = (CairoColor*)&colors->shade[3];

	/* Set clip */
	cairo_rectangle      (cr, x, y, width, height);
	cairo_clip           (cr);
	cairo_new_path       (cr);

	/* Translate and set line width */	
	cairo_set_line_width (cr, 1.0);
	cairo_translate      (cr, x+0.5, y+0.5);


	/* Make the tabs slightly bigger than they should be, to create a gap */
	/* And calculate the strip size too, while you're at it */
	if (tab->gap_side == CL_GAP_TOP || tab->gap_side == CL_GAP_BOTTOM)
	{
		height += RADIUS;
	 	strip_size = ((100.0/height)*2.0)/100.0;
		
		if (tab->gap_side == CL_GAP_TOP)
		{
			cairo_translate (cr, 0.0, -4.0); /* gap at the other side */
			corners = CL_CORNER_BOTTOMLEFT | CL_CORNER_BOTTOMRIGHT;
		}
		else
			corners = CL_CORNER_TOPLEFT | CL_CORNER_TOPRIGHT;
	}
	else
	{
		width += RADIUS;
	 	strip_size = ((100.0/width)*2.0)/100.0;
		
		if (tab->gap_side == CL_GAP_LEFT) 
		{
			cairo_translate (cr, -4.0, 0.0); /* gap at the other side */
			corners = CL_CORNER_TOPRIGHT | CL_CORNER_BOTTOMRIGHT;		
		}
		else
			corners = CL_CORNER_TOPLEFT | CL_CORNER_BOTTOMLEFT;	
	}
	
	/* Set the fill color */
	if (params->active)
		fill = (CairoColor*)&colors->bg[params->state_type];
	else
		fill = (CairoColor*)&params->parentbg;
	

	/* Set tab shape */
	cairo_rectangle (cr, 0, 0, width-1, height-1);
	
	/* Draw fill */
	cairo_set_source_rgb  (cr, fill->r, fill->g, fill->b);
	cairo_fill_preserve   (cr);
	
	if (params->active)
	{
		cairo_set_line_width (cr, 1);
		cairo_set_source_rgba (cr, 1, 1, 1, 0.4);
		
		if (tab->gap_side == CL_GAP_TOP || tab->gap_side == CL_GAP_BOTTOM)
			cairo_rectangle (cr, 1, 1, width-3, height-3);
		else
			cairo_rectangle (cr, 1, 1, width-3, height-3);
		
		cairo_stroke (cr);
	}
	else
	{
		/* Draw shade */
		pattern = cairo_pattern_create_linear ( tab->gap_side == CL_GAP_LEFT   ? width-2  : 0,
		                                        tab->gap_side == CL_GAP_TOP    ? height-2 : 0,
		                                        tab->gap_side == CL_GAP_RIGHT  ? width    : 0,
		                                        tab->gap_side == CL_GAP_BOTTOM ? height   : 0 );
	
		cairo_rectangle (cr, 0, 0, width-1, height-1);
		
		cairo_pattern_add_color_stop_rgb  (pattern, 0.0,        stripe_fill->r, stripe_fill->g, stripe_fill->b);
		cairo_pattern_add_color_stop_rgb  (pattern, strip_size, stripe_fill->r, stripe_fill->g, stripe_fill->b);
		cairo_pattern_add_color_stop_rgba (pattern, strip_size, 1.0, 1.0, 1.0, 0.5);
		cairo_pattern_add_color_stop_rgba (pattern, 0.8,        1.0, 1.0, 1.0, 0.0);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);
		cairo_pattern_destroy (pattern);
	}

	cairo_rectangle (cr, 0, 0, width-1, height-1);
	
	
	if (params->active)
	{
		cairo_set_source_rgb  (cr, border1->r, border1->g, border1->b);	
		cairo_stroke (cr);
	}
	else
	{
		pattern = cairo_pattern_create_linear ( tab->gap_side == CL_GAP_LEFT   ? width-2  : 2,
		                                        tab->gap_side == CL_GAP_TOP    ? height-2 : 2,
		                                        tab->gap_side == CL_GAP_RIGHT  ? width    : 2,
		                                        tab->gap_side == CL_GAP_BOTTOM ? height   : 2 );
		
		cairo_pattern_add_color_stop_rgb (pattern, 0.0,        stripe_border->r, stripe_border->g, stripe_border->b);
		cairo_pattern_add_color_stop_rgb (pattern, strip_size, stripe_border->r, stripe_border->g, stripe_border->b);
		cairo_pattern_add_color_stop_rgb (pattern, strip_size, border1->r,       border1->g,       border1->b);
		cairo_pattern_add_color_stop_rgb (pattern, 1.0,        border1->r,       border1->g,       border1->b);
		cairo_set_source (cr, pattern);
		cairo_stroke (cr);
		cairo_pattern_destroy (pattern);
	}
}

void
rezlooks_draw_separator (cairo_t *cr,
                           const RezlooksColors     *colors,
                           const WidgetParameters     *widget,
                           const SeparatorParameters  *separator,
                           int x, int y, int width, int height)
{
	if (separator->horizontal)
	{
		cairo_set_line_width  (cr, 1.0);
		cairo_translate       (cr, x, y+0.5);
		
		cairo_move_to         (cr, 0.0,     0.0);
		cairo_line_to         (cr, width+1, 0.0);
		cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.2);
		cairo_stroke          (cr);
		
		cairo_move_to         (cr, 0.0,   1.0);
		cairo_line_to         (cr, width, 1.0);
		cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.8);
		cairo_stroke          (cr);				
	}
	else
	{
		cairo_set_line_width  (cr, 1.0);
		cairo_translate       (cr, x+0.5, y);
		
		cairo_move_to         (cr, 0.0, 0.0);
		cairo_line_to         (cr, 0.0, height);
		cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.2);
		cairo_stroke          (cr);
		
		cairo_move_to         (cr, 1.0, 0.0);
		cairo_line_to         (cr, 1.0, height);
		cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.8);
		cairo_stroke          (cr);		
	}
}

void
rezlooks_draw_list_view_header (cairo_t *cr,
                                  const RezlooksColors          *colors,
                                  const WidgetParameters          *widget,
                                  const ListViewHeaderParameters  *header,
                                  int x, int y, int width, int height)
{
	CairoColor      *border = (CairoColor*)&colors->shade[3];
	cairo_pattern_t *pattern;
	
	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1.0);
	
	/* Draw highlight */
	if (header->order == CL_ORDER_FIRST)
	{
		cairo_move_to (cr, 0.5, height-1);
		cairo_line_to (cr, 0.5, 0.5);
	}
	else
		cairo_move_to (cr, 0.0, 0.5);
	
	cairo_line_to (cr, width, 0.5);
	
	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.5);
	cairo_stroke (cr);
	
	/* Draw bottom border */
	cairo_move_to (cr, 0.0, height-0.5);
	cairo_line_to (cr, width, height-0.5);
	cairo_set_source_rgb (cr, border->r,
	                          border->g,
	                          border->b);
	cairo_stroke (cr);

	/* Draw resize grip */
	if (header->order != CL_ORDER_LAST || header->resizable)
	{
		SeparatorParameters separator;
		separator.horizontal = FALSE;
		
		rezlooks_draw_separator (cr, colors, widget, &separator,
		                           width-1.5, 4.0, 2, height-8.0);
	}
}

/* We can't draw transparent things here, since it will be called on the same
 * surface multiple times, when placed on a handlebox_bin or dockitem_bin */
void
rezlooks_draw_toolbar (cairo_t *cr,
                         const RezlooksColors          *colors,
                         const WidgetParameters          *widget,
                         int x, int y, int width, int height)
{
	CairoColor *light  = (CairoColor*)&colors->shade[0];
	CairoColor *dark   = (CairoColor*)&colors->shade[3];
	
	cairo_set_line_width (cr, 1.0);
	
	/* Draw highlight */
	cairo_translate      (cr, x, y);
	cairo_move_to        (cr, 0, 0.5);
	cairo_line_to        (cr, width-1, 0.5);
	cairo_set_source_rgb (cr, light->r, light->g, light->b);
	cairo_stroke         (cr);

	/* Draw shadow */
	cairo_move_to        (cr, 0, height-0.5);
	cairo_line_to        (cr, width-1, height-0.5);
	cairo_set_source_rgb (cr, dark->r, dark->g, dark->b);
	cairo_stroke         (cr);
}

void
rezlooks_draw_menuitem (cairo_t *cr,
                          const RezlooksColors          *colors,
                          const WidgetParameters          *widget,
                          int x, int y, int width, int height)
{
	CairoColor *fill   = (CairoColor*)&colors->spot[1];
	CairoColor *border = (CairoColor*)&colors->spot[2];

	cairo_translate      (cr, x, y);
	cairo_set_line_width (cr, 0.0);

	cairo_rectangle      (cr, 0, 0, width, height);

	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);
	cairo_fill_preserve  (cr);

	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke         (cr);
}

void
rezlooks_draw_scrollbar_trough (cairo_t *cr,
                                  const RezlooksColors           *colors,
                                  const WidgetParameters           *widget,
                                  const ScrollBarParameters        *scrollbar,
                                  int x, int y, int width, int height)
{
	CairoColor      *bg     = (CairoColor*)&colors->shade[1];
	CairoColor      *border = (CairoColor*)&colors->shade[3];
	
	cairo_set_line_width (cr, 1);
	
	if (scrollbar->horizontal)
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
		height = width;
		width = tmp;
	}
	else
	{
		cairo_translate (cr, x, y);	
	}

	/* Draw fill */
	cairo_rectangle (cr, 1, 0, width-2, height);
	cairo_set_source_rgb (cr, bg->r, bg->g, bg->b);
	cairo_fill (cr);

	/* Draw border */
	cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke (cr);
}

void
rezlooks_draw_scrollbar_stepper (cairo_t *cr,
                                   const RezlooksColors             *colors,
                                   const WidgetParameters           *widget,
                                   const ScrollBarParameters        *scrollbar,
                                   const ScrollBarStepperParameters *stepper,
                                   int x, int y, int width, int height)
{
	CairoColor *border = (CairoColor*)&colors->shade[3];
	CairoColor *bg     = (CairoColor*)&colors->bg[widget->state_type];
	
	cairo_translate      (cr, x, y);
	cairo_set_line_width (cr, 1);
	
	cairo_rectangle      (cr, 1, 1, width-2, height-2);
	
	cairo_set_source_rgb (cr, bg->r, bg->g, bg->b);
	cairo_fill           (cr);

	cairo_rectangle      (cr, 0.5, 0.5, width-1, height-1);
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	cairo_stroke         (cr);
}

void
rezlooks_draw_scrollbar_slider (cairo_t *cr,
                                   const RezlooksColors          *colors,
                                   const WidgetParameters          *widget,
                                   const ScrollBarParameters       *scrollbar,
                                   int x, int y, int width, int height)
{
	if (scrollbar->junction & CL_JUNCTION_BEGIN)
	{
		if (scrollbar->horizontal)
		{
			x -= 1;
			width += 1;
		}
		else
		{
			y -= 1;
			height += 1;
		}
	}
	if (scrollbar->junction & CL_JUNCTION_END)
	{
		if (scrollbar->horizontal)
			width += 1;
		else
			height += 1;
	}

	if (scrollbar->has_color)
	{
		CairoColor *border  = (CairoColor*)&colors->shade[3];
		CairoColor  fill    = scrollbar->color;
		CairoColor  hilight;
			
		if (scrollbar->horizontal)
		{
			cairo_translate (cr, x, y);	
		}
		else
		{
			int tmp = height;
			rotate_mirror_translate (cr, M_PI/2, x, y, FALSE, FALSE);
			height = width;
			width = tmp;
		}

		if (widget->prelight)
			shade (&fill, &fill, 1.05);

		cairo_set_line_width (cr, 1);

		shade (&fill, &hilight, 1.1);

		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
		cairo_set_source_rgb (cr, border->r, border->g, border->b);
		cairo_stroke (cr);		

		cairo_rectangle (cr, 0.5, 0.5, width-1, height-1);
		cairo_set_source_rgb (cr, hilight.r, hilight.g, hilight.b);
		cairo_fill (cr);

		cairo_rectangle (cr, 2, 2, width-4, height-4);
		cairo_set_source_rgb (cr, fill.r, fill.g, fill.b);
		cairo_fill (cr);
	}
	else
	{
		ScrollBarStepperParameters stepper;
		stepper.stepper = CL_STEPPER_UNKNOWN;

		rezlooks_draw_scrollbar_stepper (cr, colors, widget, scrollbar, &stepper, x, y, width, height);
	}
	
}

void
rezlooks_draw_statusbar (cairo_t *cr,
                           const RezlooksColors          *colors,
                           const WidgetParameters          *widget,
                           int x, int y, int width, int height)
{
	cairo_set_line_width  (cr, 1);
	cairo_translate       (cr, x, y+0.5);
	cairo_move_to         (cr, 0, 0);
	cairo_line_to         (cr, width, 0);
	cairo_set_source_rgba (cr, 0, 0, 0, 0.2);
	cairo_stroke          (cr);

	cairo_translate       (cr, 0, 1);
	cairo_move_to         (cr, 0, 0);
	cairo_line_to         (cr, width, 0);
	cairo_set_source_rgba (cr, 1, 1, 1, 0.8);
	cairo_stroke          (cr);
}

void
rezlooks_draw_menu_frame (cairo_t *cr,
                            const RezlooksColors          *colors,
                            const WidgetParameters          *widget,
                            int x, int y, int width, int height)
{
	CairoColor *border = (CairoColor*)&colors->shade[5];
	cairo_translate      (cr, x, y);
	cairo_set_line_width (cr, 1);
	cairo_rectangle      (cr, 0.5, 0.5, width-1, height-1);
	cairo_set_source_rgb (cr, border->r, border->g, border->b);
	
	cairo_stroke         (cr);
}

void
rezlooks_draw_handle (cairo_t *cr,
                        const RezlooksColors            *colors,
                        const WidgetParameters          *widget,
                        const HandleParameters          *handle,
                        int x, int y, int width, int height)
{
	CairoColor *bar   = (CairoColor*)&colors->shade[3];

	if (handle->horizontal)
	{
		int tmp = height;
		rotate_mirror_translate (cr, M_PI/2,
		                         x + 0.5 + width/2 - height/2,
		                         y + height/2 - width/2,
		                         FALSE, FALSE);
		height = width;
		width = tmp;
	}
	else
	{
		cairo_translate (cr, x, y + 0.5);
	}

	cairo_set_line_width (cr, 1);

	cairo_set_source_rgb (cr, bar->r, bar->g, bar->b);

	cairo_move_to        (cr, 2.5, (height/2)-3);
	cairo_line_to        (cr, width-2.5, (height/2)-3);
	cairo_stroke         (cr);

	cairo_move_to        (cr, 2.5, (height/2));
	cairo_line_to        (cr, width-2.5, (height/2));
	cairo_stroke         (cr);

	cairo_move_to        (cr, 2.5, (height/2)+3);
	cairo_line_to        (cr, width-2.5, (height/2)+3);
	cairo_stroke         (cr);
}

static void
rezlooks_draw_normal_arrow (cairo_t *cr, CairoColor *color,
                              double x, double y, double width, double height)
{
	const int ARROW_WIDTH  = 10.0;
	const int ARROW_HEIGHT = 5.0;

	cairo_set_line_width (cr, 1);
	
	cairo_move_to   (cr, -ARROW_WIDTH/2, -ARROW_HEIGHT/2);
	cairo_line_to   (cr, 0, ARROW_HEIGHT/2);
	cairo_line_to   (cr, ARROW_WIDTH/2, -ARROW_HEIGHT/2);
	
	cairo_set_source_rgb (cr, color->r, color->g, color->b);
	cairo_fill (cr);	
}

static void
rezlooks_draw_combo_arrow (cairo_t *cr, CairoColor *fill,
                             double x, double y, double width, double height)
{
	const int ARROW_WIDTH   = 7.0;
	const int ARROW_HEIGHT  = 5.0;
	const int ARROW_SPACING = 8;
	
	cairo_set_line_width (cr, 1);

	y -= ARROW_SPACING/2;
	
	cairo_move_to (cr, -ARROW_WIDTH/2, y + ARROW_HEIGHT/2);
	cairo_line_to   (cr, 0, y + -ARROW_HEIGHT/2);
	cairo_line_to   (cr, ARROW_WIDTH/2, y + ARROW_HEIGHT/2);
	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);	
	cairo_fill (cr);
	
	y += ARROW_SPACING;
	
	cairo_move_to (cr, -ARROW_WIDTH/2, y + -ARROW_HEIGHT/2);
	cairo_line_to   (cr, 0, y + ARROW_HEIGHT/2);
	cairo_line_to   (cr, ARROW_WIDTH/2, y + -ARROW_HEIGHT/2);
	cairo_set_source_rgb (cr, fill->r, fill->g, fill->b);	
	cairo_fill (cr);
}

static void
_rezlooks_draw_arrow (cairo_t *cr, CairoColor *color,
                        RezlooksDirection dir, RezlooksArrowType type,
                        double x, double y, double width, double height)
{
	double rotate;
	
	if (dir == CL_DIRECTION_LEFT)
		rotate = M_PI*1.5;
	else if (dir == CL_DIRECTION_RIGHT)
		rotate = M_PI*0.5;
	else if (dir == CL_DIRECTION_UP)
		rotate = M_PI;
	else
		rotate = 0;
	
	if (type == CL_ARROW_NORMAL)
	{		
		rotate_mirror_translate (cr, rotate, x, y, FALSE, FALSE);		
		rezlooks_draw_normal_arrow (cr, color, 0, 0, width, height);
	}
	else if (type == CL_ARROW_COMBO)
	{
		cairo_translate (cr, x, y);
		rezlooks_draw_combo_arrow (cr, color, 0, 0, width, height);
	}
}

void
rezlooks_draw_arrow (cairo_t *cr,
                       const RezlooksColors          *colors,
                       const WidgetParameters          *widget,
                       const ArrowParameters           *arrow,
                       int x, int y, int width, int height)
{
	CairoColor *color = (CairoColor*)&colors->shade[7];
	gdouble tx, ty;
	
	if (arrow->direction == CL_DIRECTION_DOWN || arrow->direction == CL_DIRECTION_UP)
	{
		tx = x + width/2;
		ty = (y + height/2) + 0.5;
	}
	else
	{
		tx = (x + width/2) + 0.5;
		ty = y + height/2;
	}
	
	if (widget->disabled)
	{
		_rezlooks_draw_arrow (cr, (CairoColor*)&colors->shade[0],
		                        arrow->direction, arrow->type,
		                        tx+0.5, ty+0.5, width, height);
		
		color = (CairoColor*)&colors->shade[4];
	}

	cairo_identity_matrix (cr);
	
	_rezlooks_draw_arrow (cr, color, arrow->direction, arrow->type,
	                        tx, ty, width, height);
}

void
rezlooks_draw_resize_grip (cairo_t *cr,
                             const RezlooksColors          *colors,
                             const WidgetParameters          *widget,
                             const ResizeGripParameters      *grip,
                             int x, int y, int width, int height)
{
 	CairoColor *dark = (CairoColor*)&colors->shade[4];
	int lx, ly;
 
	cairo_set_line_width (cr, 1);

	for (ly=0; ly<4; ly++) // vertically, four rows of dots
	{
		for (lx=0; lx<=ly; lx++) // horizontally
		{
			int ny = (3.5-ly) * 3;
			int nx = lx * 3;

			cairo_set_source_rgba (cr, 1, 1, 1, 1.2);
			cairo_rectangle (cr, x+width-nx-1, y+height-ny-1, 2, 2);
			cairo_fill (cr);

			cairo_set_source_rgba (cr, 0, 0, 0, 0.4);
			cairo_rectangle (cr, x+width-nx-1, y+height-ny-1, 1, 1);
			cairo_fill (cr);
		}
	}
}
