INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7 8 9 10 11
                  ls2-bavaria-g3gtr ls2-bavaria-nb ls2-boxer-96rsr
                  ls2-cavallo-360r ls2-morley-r900 ls2-vulture-v6rs2
                  tracks)

ROBOT_DATA(NAME dandroid_ls2
           FILES dandroid_ls2.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg *.csv)
