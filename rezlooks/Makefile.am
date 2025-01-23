INCLUDES = $(GTK_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = librezlooks.la

EXTRA_DIST = ./src/config.h.in


librezlooks_la_SOURCES =		\
        ./src/rezlooks_rc_style.c	\
        ./src/rezlooks_rc_style.h	\
        ./src/rezlooks_style.c	\
        ./src/rezlooks_style.h	\
        ./src/rezlooks_theme_main.c	\
	./src/support.c			\
	./src/animation.c		\
	./src/animation.h		\
	./src/support.h			\
	./src/rezlooks_draw.c		\
	./src/rezlooks_draw.h		\
	./src/rezlooks_types.h	\
	./src/config.h

librezlooks_la_LDFLAGS = -module -avoid-version -no-undefined
librezlooks_la_LIBADD =  $(GTK_LIBS)

