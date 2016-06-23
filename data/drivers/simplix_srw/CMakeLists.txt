INCLUDE(../../../cmake/macros.cmake)

# The data for the SRW clone of the ubiquitous Simplix module.

SET(ROBOT_SUBDIRS 0 1 2 3 4 5 6 7
                  srw-sector-p4
                  srw-kongei-kk8s
                  srw-boxer-ts
                  srw-kanagawa-gtr
                  tracks)

ROBOT_DATA(NAME simplix_srw
           FILES simplix_srw.xml default.xml logo.rgb readme.txt
           SUBDIRS ${ROBOT_SUBDIRS} PATTERNS *.xml logo.rgb *.png *.jpg)
