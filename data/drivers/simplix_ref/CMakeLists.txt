INCLUDE(../../../cmake/macros.cmake)

# The data for the Ref clone of the ubiquitous Simplix module.

SET(ROBOT_SUBDIRS 0
                  ref-sector-p4
                  )

ROBOT_DATA(NAME simplix_ref
           FILES simplix_ref.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)			   
