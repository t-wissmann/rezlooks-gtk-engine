#ifndef REZLOOKS_DRAW_H
#define REZLOOKS_DRAW_H

#include "rezlooks_types.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <cairo.h>

void rezlooks_draw_button           (cairo_t *cr,
                                       const RezlooksColors      *colors,
                                       const WidgetParameters      *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_scale_trough     (cairo_t *cr,
                                       const RezlooksColors      *colors,
                                       const WidgetParameters      *widget,
                                       const SliderParameters      *slider,
                                       int x, int y, int width, int height);

void rezlooks_draw_progressbar_trough (cairo_t *cr,
                                       const RezlooksColors      *colors,
                                       const WidgetParameters      *widget,
                                       int x, int y, int width, int height);


void rezlooks_draw_progressbar_fill (cairo_t *cr,
                                       const RezlooksColors      *colors,
                                       const WidgetParameters      *widget,
                                       const ProgressBarParameters *progressbar,
                                       int x, int y, int width, int height, gint offset);

void rezlooks_draw_slider_button    (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SliderParameters     *slider,
                                       int x, int y, int width, int height);

void rezlooks_draw_entry            (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_spinbutton       (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_spinbutton_down  (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_optionmenu       (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       const OptionMenuParameters *optionmenu,
                                       int x, int y, int width, int height);
								   
void rezlooks_draw_inset            (cairo_t *cr,
                                       int width, int height,
                                       double radius, uint8 corners);

void rezlooks_draw_menubar          (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       int x, int y, int width, int height,
									   int menubarstyle);

void rezlooks_draw_tab              (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       const TabParameters        *tab,
                                       int x, int y, int width, int height);

void rezlooks_draw_frame            (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       const FrameParameters      *frame,
                                       int x, int y, int width, int height);

void rezlooks_draw_separator        (cairo_t *cr,
                                       const RezlooksColors     *colors,
                                       const WidgetParameters     *widget,
                                       const SeparatorParameters  *separator,
                                       int x, int y, int width, int height);

void rezlooks_draw_list_view_header (cairo_t *cr,
                                       const RezlooksColors         *colors,
                                       const WidgetParameters         *widget,
                                       const ListViewHeaderParameters *header,
                                       int x, int y, int width, int height);

void rezlooks_draw_toolbar          (cairo_t *cr,
                                       const RezlooksColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_menuitem         (cairo_t *cr,
                                       const RezlooksColors         *colors,
                                       const WidgetParameters         *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_scrollbar_stepper (cairo_t *cr,
                                        const RezlooksColors           *colors,
                                        const WidgetParameters           *widget,
                                        const ScrollBarParameters        *scrollbar,
                                        const ScrollBarStepperParameters *stepper,
                                        int x, int y, int width, int height);

void rezlooks_draw_scrollbar_slider (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height);

void rezlooks_draw_scrollbar_trough (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ScrollBarParameters       *scrollbar,
                                       int x, int y, int width, int height);

void rezlooks_draw_statusbar        (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_menu_frame       (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       int x, int y, int width, int height);

void rezlooks_draw_handle           (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       const HandleParameters          *handle,
                                       int x, int y, int width, int height);

void rezlooks_draw_arrow            (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ArrowParameters           *arrow,
                                       int x, int y, int width, int height);

void rezlooks_draw_resize_grip      (cairo_t *cr,
                                       const RezlooksColors          *colors,
                                       const WidgetParameters          *widget,
                                       const ResizeGripParameters      *grip,
                                       int x, int y, int width, int height);


#endif /* REZLOOKS_DRAW_H */
