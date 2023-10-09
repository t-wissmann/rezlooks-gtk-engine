/* Rezlooks Engine
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifdef HAVE_ANIMATION
#include <gtk/gtk.h>

#define CL_IS_PROGRESS_BAR(widget) GTK_IS_PROGRESS_BAR(widget) && widget->allocation.x != -1 && widget->allocation.y != -1
#define ANIMATION_DELAY 100
#define CHECK_ANIMATION_TIME 0.5

void     rezlooks_animation_progressbar_add (GtkWidget *progressbar);
void     rezlooks_animation_connect_checkbox (GtkWidget *widget);
gboolean rezlooks_animation_is_animated (GtkWidget *widget);
gdouble  rezlooks_animation_elapsed (gpointer data);
void     rezlooks_animation_cleanup ();
#endif /* HAVE_ANIMATION */
