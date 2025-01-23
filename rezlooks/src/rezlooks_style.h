/* Rezlooks Engine
 * Copyright (C) 2005 Richard Stellingwerff.
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
 *
 * Written by Owen Taylor <otaylor@redhat.com>
 * and by Alexander Larsson <alexl@redhat.com>
 * Modified by Richard Stellingwerff <remenic@gmail.com>
 */
#include <gtk/gtkstyle.h>

#include "animation.h"
#include "rezlooks_types.h"

typedef struct _RezlooksStyle RezlooksStyle;
typedef struct _RezlooksStyleClass RezlooksStyleClass;

extern GType rezlooks_type_style;

#define REZLOOKS_TYPE_STYLE              rezlooks_type_style
#define REZLOOKS_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), REZLOOKS_TYPE_STYLE, RezlooksStyle))
#define REZLOOKS_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), REZLOOKS_TYPE_STYLE, RezlooksStyleClass))
#define REZLOOKS_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), REZLOOKS_TYPE_STYLE))
#define REZLOOKS_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), REZLOOKS_TYPE_STYLE))
#define REZLOOKS_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), REZLOOKS_TYPE_STYLE, RezlooksStyleClass))

struct _RezlooksStyle
{
	GtkStyle parent_instance;

	RezlooksColors colors;
	
	guint8   progressbarstyle;
	guint8   menubarstyle;
	guint8   menubaritemstyle;
	guint8   menuitemstyle;
	guint8   listviewitemstyle;
	GdkColor scrollbar_color;
	gboolean has_scrollbar_color;
	gboolean animation;
};

struct _RezlooksStyleClass
{
  GtkStyleClass parent_class;
};


void rezlooks_style_register_type (GTypeModule *module);
