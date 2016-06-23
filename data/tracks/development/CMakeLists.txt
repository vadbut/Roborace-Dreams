INCLUDE(../../../cmake/macros.cmake)

# Work-in-progress tracks.
IF(NOT OPTION_OFFICIAL_ONLY)

	SD_ADD_SUBDIRECTORY(showroom)
	SD_ADD_SUBDIRECTORY(testtrack)
	SD_ADD_SUBDIRECTORY(straight-10)
	SD_ADD_SUBDIRECTORY(forza-osg)
	SD_ADD_SUBDIRECTORY(garage)
   SD_ADD_SUBDIRECTORY(tgs)

ENDIF()
