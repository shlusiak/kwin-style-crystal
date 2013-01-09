kwin-style-crystal
==================

Crystal window decoration for KDE 3.5 and KDE 4

This is the Crystal KDE window decoration Theme. The theme is pretty simple and clean, but can use KWIN's 
transparency and blur effects.

It is modelled on the crystal fvwm2 style. Check it out:
http://www.fvwm-crystal.org/


See project pages on KDE-Look.org:
- For KDE 3.5: http://kde-look.org/content/show.php/Crystal?content=13969
- For KDE 4.x: http://kde-look.org/content/show.php/crystal?content=75140
- GitHub: https://github.com/shlusiak/kwin-style-crystal/



INSTALLATION
------------

Basic Installation (from the console):
- Step 1 (as user)
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
$ make
- Step 3 (as root)
# make install

If you installed crystal while it was running, you would need to restart kwin for the changes to take effect: kwin --replace



Sascha Hlusiak (mail@saschahlusiak.de)
