INCLUDE(../../../cmake/macros.cmake)

# The data for the 67GP clone of the ubiquitous SHADOW robot module.

SET(ROBOT_SUBDIRS 0 1 67gp-upping-ut19 tracks)

ROBOT_DATA(NAME shadow_67GP
           FILES shadow_67GP.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
