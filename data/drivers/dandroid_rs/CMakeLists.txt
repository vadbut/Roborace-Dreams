INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7 8 9
                  rs1-citrus-cs4 rs1-fmc-centre rs1-kenji-08
                  rs2-fai-dot rs2-lion-2m
                  tracks)

ROBOT_DATA(NAME dandroid_rs
           FILES dandroid_rs.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg *.csv)
