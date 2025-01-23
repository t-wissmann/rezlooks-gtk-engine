# Rezlooks GTK Engine

The purpose of this repository is to preserve the source code of the rezlooks GTK2 engine.
I'm not the author of this, but I've been using this for over a decade (at
least since 2008). Thus I want to prevent the source code from being lost. The
license in the original tarball is

      GNU GENERAL PUBLIC LICENSE
         Version 2, June 1991

The upstream page is on gnome-looks.org:

    https://www.gnome-look.org/p/1080201/


## Content of this repository

  - `rezlooks/` contains the content of the tarball of version 0.6 from the
    above upstream url with patches applied to make it compile on modern
    systems.
  - I once had the la(te)st release tarball `rezlooks-0.6.tar.gz` in the repository;
    of course it is still in the history (e.g. in commit
    `5953b8c261ed51e90c411b1e16ed8da74a0b7b43`)
  - In order to make packaging easy, I've created a release, the current
    tarball is linked [under
    "tags"](https://github.com/t-wissmann/rezlooks-gtk-engine/tags) and here:
    https://github.com/t-wissmann/rezlooks-gtk-engine/archive/refs/tags/v0.6.1.tar.gz


## Compilation

In order to build rezlooks, see the original `README` file by the author in the
`rezlooks` subdirectory or the build instructions in my [PKGBUILD in the
aur](https://aur.archlinux.org/packages/gtk-engine-rezlooks).

## Screenshots

Screenshots can be found [upstream](https://www.gnome-look.org/p/1080201/),
they are also mirrored on [this wiki page](https://github.com/t-wissmann/rezlooks-gtk-engine/wiki).
