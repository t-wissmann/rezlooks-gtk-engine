# Rezlooks GTK Engine

The purpose of this repository is to preserve the source code of the rezlooks GTK2 engine.
I'm not the author of this, but I've been using this for over a decate (at
least since 2008). Thus I want to prevent the source code from being lost. The
license in the original tarball is

      GNU GENERAL PUBLIC LICENSE
         Version 2, June 1991

The upstream page is on gnome-looks.org:

    https://www.gnome-look.org/p/1080201/


## Content of this repository

  - `rezlooks-0.6.tar.gz` is the most recent tarball (version 0.6) from above
    upstream url.
  - `rezlooks-0.6/` contains the content of the tarball of version 0.6
    from the above upstream url
  - Screenshots can be found [upstream](https://www.gnome-look.org/p/1080201/)

## Compilation

In order to build rezlooks on modern platforms, it might be necessary to change `glib/gtimer` to `glib` in `src/animation.c` as I did in the [PKGBUILD in the aur](https://aur.archlinux.org/packages/gtk-engine-rezlooks)

