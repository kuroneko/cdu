# CDU: A CDU Simulator

CDU is loosly adapted from Hoppie's MCDU application and contains code 
genericised for alternate CDU models

The bulk of the code provides a framework for building model specific and
simulator specific CDU simulators on top of SDL2.

This repository also contains a front-end suitable for interfacing with
Precision Simulator X, and an incomplete front-end to interface with the
PS13 Broker.

# License & Copyright

The source code is released under the GNU GPL version 3.

All source code is Copyright (C) 2015, Christopher Collins.

The CDU fonts (Aerowinx MCDU Large & Small) are from Aerowinx
(www.aerowinx.com).  Permission for use with this code has been granted
as long as credit is appropriately attributed and their names remain
unchanged.

Image work was derived from the originals in Hoppie's MCDU application
(http://www.hoppie.nl/mcdu/) which is also released under the GPL.

The principle author reserves the right to release binaries at no cost
under licenses other than the GPL when the GPL is incompatible with the 
distribution mechanism for a given platform.

# Usage

To build the current cdu clients, you will need

* CMake 3.1
* SDL2
* SDL2_ttf
* SDL2_image
* libconfig

After checking out the source tree and initialising the submodules (libpsx)

1. Create and change to a build directory.  (You don't want to do your builds in the source tree)

1. run `cmake ../path/to/src`

1. Run `make`

To prep the tree for testing, link the cdu/resources directory to your build directory, and copy cdu/psx/mcdupsx.conf.sample to mcdupsx.conf, and edit to suit your system(s).

Then, from the directory containing the resources symlink and config file, start the `psx/mcdupsx` binary.

When mcdupsx is running, it'll try to flash up a multicoloured self-test which stems from early iterations of my codebase before I picked up PSX (it imitates Hoppie's MCDU test page, just with colour so I could verify that my colour rendering method worked).  Once it connects to PSX, it'll refresh immediately and display whatever the CDU you've configured it to monitor is showing.

PgUp/PgDn are prev page/next page.  Del is DELETE.  Backspace is CLR.  Enter should be Exec.  SEL/SER are not mapped yet, which makes it hard to use.  You can easily hardcode them in mcdupsx.cpp however.  I'm mostly using this for testing only at the moment.