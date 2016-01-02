Ensimag Post-WIMP project
========

A 3D painting software with your two hands.

Video
=====

[![Video here](http://img.youtube.com/vi/sjegf5U2250/0.jpg)](http://www.youtube.com/watch?v=sjegf5U2250)

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

Install Polyvox and WiiCPP (see instructions below)


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

installing wiic : a C/C++ libray for controlling wiimotes and nunchunk
=======================================================================
* follow : [instructions](https://github.com/kaendfinger/wiic.git)


Keyboard
========

* moving   : Z,Q,S,D,A,E
* rotating : J,L,K,I,U,O
* put glue : simply move the mouse.
* change color of the glue : SPACE.
