INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7 8 9 10 11
                  lp1-lolita-exr57 lp1-luxura-lp02 lp1-pescy-p60
                  lp1-spire-s101 lp1-sprite-p1rs lp1-vieringe-vr8 Robocar
                  tracks)

ROBOT_DATA(NAME udp_Roborace
           FILES udp_Roborace.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)			   
