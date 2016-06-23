INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_SUBDIRS 0 1 67gp-upping-ut19
                  tracks)

ROBOT_DATA(NAME dandroid_67GP
           FILES dandroid_67GP.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg *.csv)
