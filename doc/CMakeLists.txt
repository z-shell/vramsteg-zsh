cmake_minimum_required (VERSION 2.8)
message ("-- Configuring man pages")
set (man_FILES vramsteg.1)
foreach (man_FILE ${man_FILES})
  configure_file (
    man/${man_FILE}.in
    man/${man_FILE})
endforeach (man_FILE)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/man/ DESTINATION ${VRAMSTEG_MAN1DIR}
                                                    FILES_MATCHING PATTERN "*.1")

