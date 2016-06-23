INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_NAME "hymie")

SET(ROBOT_FILES )
SET(ROBOT_SUBDIRS 1 2 3 4 5 6 7 8 9 10 
                  sc-boxer-96 sc-spirit-300 sc-cavallo-360 sc-fmc-gt4
                  sc-lynx-220 sc-murasama-nsx svg)

ROBOT_DATA(NAME ${ROBOT_NAME}
           FILES ${ROBOT_FILES}
           SUBDIRS ${ROBOT_SUBDIRS})

