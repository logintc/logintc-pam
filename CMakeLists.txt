cmake_minimum_required(VERSION 2.6)
project(logintc-pam)

include(GNUInstallDirs)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(pam_logintc MODULE src/pam_logintc.c)
set_target_properties(pam_logintc PROPERTIES PREFIX "")
target_link_libraries(pam_logintc)

install(TARGETS pam_logintc DESTINATION ${CMAKE_INSTALL_LIBDIR}/security)
