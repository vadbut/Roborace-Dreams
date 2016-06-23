INCLUDE(../../../cmake/macros.cmake)

# The data for the MPA12 clone of the ubiquitous SHADOW robot module.

SET(ROBOT_SUBDIRS 0 1 mpa12-murasama tracks)

ROBOT_DATA(NAME usr_mpa12
           FILES usr_mpa12.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
