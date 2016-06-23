INCLUDE(../../../cmake/macros.cmake)

# The data for the LP1 clone of the ubiquitous Simplix module.

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7 8 9 10 11
                  lp1-lolita-exr57 lp1-luxura-lp02 lp1-pescy-p60
                  lp1-spire-s101 lp1-sprite-p1rs lp1-vieringe-vr8
                  tracks)

ROBOT_DATA(NAME dandroid_lp1
           FILES dandroid_lp1.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)			   
