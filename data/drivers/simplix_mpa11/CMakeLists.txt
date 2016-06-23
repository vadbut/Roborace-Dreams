INCLUDE(../../../cmake/macros.cmake)

# The data for the MPA11 clone of the ubiquitous SIMPLIX robot module.

SET(ROBOT_SUBDIRS 0 1 
                  mpa11-murasama tracks)

ROBOT_DATA(NAME simplix_mpa11
           FILES simplix_mpa11.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
