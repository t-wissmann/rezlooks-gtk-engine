/* Rezlooks theme engine
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
 * Modified by Kulyk Nazar <schamane@myeburg.net>
 */

#include "rezlooks_style.h"
#include "rezlooks_rc_style.h"

#include "animation.h"

static void      rezlooks_rc_style_init         (RezlooksRcStyle      *style);
#ifdef HAVE_ANIMATION
static void      rezlooks_rc_style_finalize     (GObject                *object);
#endif
static void      rezlooks_rc_style_class_init   (RezlooksRcStyleClass *klass);
static GtkStyle *rezlooks_rc_style_create_style (GtkRcStyle             *rc_style);
static guint     rezlooks_rc_style_parse        (GtkRcStyle             *rc_style,
						   GtkSettings            *settings,
						   GScanner               *scanner);
static void      rezlooks_rc_style_merge        (GtkRcStyle             *dest,
						   GtkRcStyle             *src);


static GtkRcStyleClass *parent_class;

GType rezlooks_type_rc_style = 0;

enum
{
  TOKEN_SCROLLBARCOLOR = G_TOKEN_LAST + 1,
  TOKEN_CONTRAST,
  TOKEN_SUNKENMENU,
  TOKEN_PROGRESSBARSTYLE,
  TOKEN_MENUBARSTYLE,
  TOKEN_MENUBARITEMSTYLE,
  TOKEN_MENUITEMSTYLE,
  TOKEN_LISTVIEWITEMSTYLE,
  TOKEN_ANIMATION,
  
  TOKEN_TRUE,
  TOKEN_FALSE
};

static struct
  {
    const gchar        *name;
    guint               token;
  }
theme_symbols[] =
{
  { "scrollbar_color",   TOKEN_SCROLLBARCOLOR  },
  { "contrast", 		 TOKEN_CONTRAST  },
  { "sunkenmenubar",     TOKEN_SUNKENMENU },
  { "progressbarstyle",  TOKEN_PROGRESSBARSTYLE },
  { "menubarstyle",      TOKEN_MENUBARSTYLE },
  { "menubaritemstyle",  TOKEN_MENUBARITEMSTYLE },
  { "menuitemstyle",     TOKEN_MENUITEMSTYLE },
  { "listviewitemstyle", TOKEN_LISTVIEWITEMSTYLE },
  { "animation",	     TOKEN_ANIMATION },
  
  { "TRUE",	TOKEN_TRUE },
  { "FALSE",	TOKEN_FALSE }
};


void
rezlooks_rc_style_register_type (GTypeModule *module)
{
  static const GTypeInfo object_info =
  {
    sizeof (RezlooksRcStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) rezlooks_rc_style_class_init,
    NULL,           /* class_finalize */
    NULL,           /* class_data */
    sizeof (RezlooksRcStyle),
    0,              /* n_preallocs */
    (GInstanceInitFunc) rezlooks_rc_style_init,
    NULL
  };
  
  rezlooks_type_rc_style = g_type_module_register_type (module,
						     GTK_TYPE_RC_STYLE,
						     "RezlooksRcStyle",
						     &object_info, 0);
}

static void
rezlooks_rc_style_init (RezlooksRcStyle *rezlooks_rc)
{
  rezlooks_rc->has_scrollbar_color = FALSE;
  rezlooks_rc->contrast = 1.0;
  rezlooks_rc->sunkenmenubar = 1;
  rezlooks_rc->progressbarstyle = 0;
  rezlooks_rc->menubarstyle = 0;
  rezlooks_rc->menubaritemstyle = 0;
  rezlooks_rc->menuitemstyle = 0;
  rezlooks_rc->listviewitemstyle = 1;
  rezlooks_rc->animation = FALSE;
}

#ifdef HAVE_ANIMATION
static void
rezlooks_rc_style_finalize (GObject *object)
{
	/* cleanup all the animation stuff */
	rezlooks_animation_cleanup ();
	
	if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
		G_OBJECT_CLASS (parent_class)->finalize(object);
}
#endif


static void
rezlooks_rc_style_class_init (RezlooksRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);
#ifdef HAVE_ANIMATION
  GObjectClass    *g_object_class = G_OBJECT_CLASS (klass);
#endif

  parent_class = g_type_class_peek_parent (klass);

  rc_style_class->parse = rezlooks_rc_style_parse;
  rc_style_class->create_style = rezlooks_rc_style_create_style;
  rc_style_class->merge = rezlooks_rc_style_merge;

#ifdef HAVE_ANIMATION
  g_object_class->finalize = rezlooks_rc_style_finalize;
#endif
}

static guint
theme_parse_boolean (GtkSettings *settings,
                     GScanner     *scanner,
                     gboolean *retval)
{
  guint token;
  token = g_scanner_get_next_token(scanner);
  
  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;
  
  token = g_scanner_get_next_token(scanner);
  if (token == TOKEN_TRUE)
    *retval = TRUE;
  else if (token == TOKEN_FALSE)
    *retval = FALSE;
  else
    return TOKEN_TRUE;

  return G_TOKEN_NONE;
}

static guint
theme_parse_color(GtkSettings  *settings,
		  GScanner     *scanner,
		  GdkColor     *color)
{
  guint token;

  /* Skip 'blah_color' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

  return gtk_rc_parse_color (scanner, color);
}

static guint
theme_parse_contrast(GtkSettings  *settings,
		     GScanner     *scanner,
		     double       *contrast)
{
  guint token;

  /* Skip 'contrast' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_FLOAT)
    return G_TOKEN_FLOAT;

  *contrast = scanner->value.v_float;
  
  return G_TOKEN_NONE;
}

static guint
theme_parse_int (GtkSettings  *settings,
		         GScanner     *scanner,
		         guint8       *progressbarstyle)
{
  guint token;

  /* Skip 'sunkenmenubar' */
  token = g_scanner_get_next_token(scanner);

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_EQUAL_SIGN)
    return G_TOKEN_EQUAL_SIGN;

  token = g_scanner_get_next_token(scanner);
  if (token != G_TOKEN_INT)
    return G_TOKEN_INT;

  *progressbarstyle = scanner->value.v_int;
  
  return G_TOKEN_NONE;
}

static guint
rezlooks_rc_style_parse (GtkRcStyle *rc_style,
			   GtkSettings  *settings,
			   GScanner   *scanner)
		     
{
  static GQuark scope_id = 0;
  RezlooksRcStyle *rezlooks_style = REZLOOKS_RC_STYLE (rc_style);

  guint old_scope;
  guint token;
  guint i;
  
  /* Set up a new scope in this scanner. */

  if (!scope_id)
    scope_id = g_quark_from_string("rezlooks_theme_engine");

  /* If we bail out due to errors, we *don't* reset the scope, so the
   * error messaging code can make sense of our tokens.
   */
  old_scope = g_scanner_set_scope(scanner, scope_id);

  /* Now check if we already added our symbols to this scope
   * (in some previous call to rezlooks_rc_style_parse for the
   * same scanner.
   */

  if (!g_scanner_lookup_symbol(scanner, theme_symbols[0].name))
    {
      g_scanner_freeze_symbol_table(scanner);
      for (i = 0; i < G_N_ELEMENTS (theme_symbols); i++)
	g_scanner_scope_add_symbol(scanner, scope_id,
				   theme_symbols[i].name,
				   GINT_TO_POINTER(theme_symbols[i].token));
      g_scanner_thaw_symbol_table(scanner);
    }

  /* We're ready to go, now parse the top level */

  token = g_scanner_peek_next_token(scanner);
  while (token != G_TOKEN_RIGHT_CURLY)
    {
      switch (token)
	{
	case TOKEN_SCROLLBARCOLOR:
	  token = theme_parse_color (settings, scanner, &rezlooks_style->scrollbar_color);
	  rezlooks_style->has_scrollbar_color = TRUE;
	  break;
	case TOKEN_CONTRAST:
	  token = theme_parse_contrast (settings, scanner, &rezlooks_style->contrast);
	  break;
	case TOKEN_SUNKENMENU:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->sunkenmenubar);
	  break;
	case TOKEN_PROGRESSBARSTYLE:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->progressbarstyle);
	  break;
	case TOKEN_MENUBARSTYLE:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->menubarstyle);
	  break;
	case TOKEN_MENUBARITEMSTYLE:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->menubaritemstyle);
	  break;
	case TOKEN_MENUITEMSTYLE:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->menuitemstyle);
	  break;
	case TOKEN_LISTVIEWITEMSTYLE:
	  token = theme_parse_int (settings, scanner, &rezlooks_style->listviewitemstyle);
	  break;
	case TOKEN_ANIMATION:
	  token = theme_parse_boolean (settings, scanner, &rezlooks_style->animation);
	  break;
	default:
	  g_scanner_get_next_token(scanner);
	  token = G_TOKEN_RIGHT_CURLY;
	  break;
	}

      if (token != G_TOKEN_NONE)
	return token;

      token = g_scanner_peek_next_token(scanner);
    }

  g_scanner_get_next_token(scanner);

  g_scanner_set_scope(scanner, old_scope);

  return G_TOKEN_NONE;
}

static void
rezlooks_rc_style_merge (GtkRcStyle *dest,
			   GtkRcStyle *src)
{
	RezlooksRcStyle *dest_w, *src_w;
	
	parent_class->merge (dest, src);
	
	if (!REZLOOKS_IS_RC_STYLE (src))
		return;
	
	src_w = REZLOOKS_RC_STYLE (src);
	dest_w = REZLOOKS_RC_STYLE (dest);
	
	dest_w->contrast          = src_w->contrast;
	dest_w->sunkenmenubar     = src_w->sunkenmenubar;
	dest_w->progressbarstyle  = src_w->progressbarstyle;
	dest_w->menubarstyle      = src_w->menubarstyle;
	dest_w->menubaritemstyle  = src_w->menubaritemstyle;
	dest_w->menuitemstyle     = src_w->menuitemstyle;
	dest_w->listviewitemstyle = src_w->listviewitemstyle;

	if (src_w->has_scrollbar_color)
	{
		dest_w->has_scrollbar_color = TRUE;
		dest_w->scrollbar_color = src_w->scrollbar_color;
	}
	
	dest_w->animation = src_w->animation;

}


/* Create an empty style suitable to this RC style
 */
static GtkStyle *
rezlooks_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (REZLOOKS_TYPE_STYLE, NULL));
}
