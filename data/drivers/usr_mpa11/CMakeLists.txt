INCLUDE(../../../cmake/macros.cmake)

# The data for the MPA11 clone of the ubiquitous SHADOW robot module.

SET(ROBOT_SUBDIRS 1 2 mpa11-murasama tracks)

ROBOT_DATA(NAME usr_mpa11
           FILES usr_mpa11.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
