INCLUDE(../../../cmake/macros.cmake)

# The data for the LP1 clone of the ubiquitous Simplix module.

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 
                  lp1-pescy-p60 lp1-vieringe-vr8
                  )

ROBOT_DATA(NAME simplix_lp1
           FILES simplix_lp1.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)			   
