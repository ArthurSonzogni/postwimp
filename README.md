postwimp
========

Description in progress

installing the required library
===============================

you need
* cmake - version 2.8 or later
* boost - with system and thread components

On Debian-based systems (like Ubuntu), this will install the required components:

sudo apt-get install
* cmake
* libboost-dev
* libboost-program-option-dev
* libboost-system-dev
* libboost-thread-dev

There is some other libraries I forget.

installing the development version of PolyVox
=======================================

You need:
* cmake

You need the development version of Polyvox
you can type theses commands:
* git clone https://bitbucket.org/volumesoffun/polyvox.git
* cd polyvox
* git checkout origin/develop
* cmake .
* sudo make install

Keyboard
========

* moving   : Z,Q,S,D,A,E
* rotating : J,L,K,I,U,O
* put glue : simply move the mouse.
* change color of the glue : SPACE.
