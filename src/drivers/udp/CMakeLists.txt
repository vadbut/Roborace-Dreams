INCLUDE(../../../cmake/macros.cmake)

SET(ROBOT_NAME "udp")

SET(ROBOT_INTERFACE LEGACY WELCOME)

SET(ROBOT_SOURCES 
	src/globaldefinitions.h
	src/CarControl.h
	src/CarControl.cpp
	src/SimpleParser.h
	src/SimpleParser.cpp
    src/udp.cpp
    src/driver.cpp
    src/driver.h
    src/Utils.cpp
    src/Utils.h
    src/Vec2d.h
    src/Vec3d.h
)

# Official-only udp instances.
SET(ROBOT_CLONES udp_36GP udp_67GP udp_lp1 udp_ls1 udp_ls2 udp_mpa1
                 udp_mpa11 udp_mpa12 udp_rs udp_sc udp_srw udp_trb1 udp_Roborace)

# The ubiquitous robot module and its clones.
ROBOT_MODULE(NAME ${ROBOT_NAME} VERSION 2.0.0 SOVERSION 1.0.0
             INTERFACE ${ROBOT_INTERFACE}
             SOURCES ${ROBOT_SOURCES}
             CLONENAMES ${ROBOT_CLONES})

# For data associated to the ubiquitous robot module and its clones,
# see data/drivers/udp*/CMakeLists.txt
