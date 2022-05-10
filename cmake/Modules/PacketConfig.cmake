INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_PACKET Packet)

FIND_PATH(
    PACKET_INCLUDE_DIRS
    NAMES Packet/api.h
    HINTS $ENV{PACKET_DIR}/include
        ${PC_PACKET_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    PACKET_LIBRARIES
    NAMES gnuradio-Packet
    HINTS $ENV{PACKET_DIR}/lib
        ${PC_PACKET_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/PacketTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PACKET DEFAULT_MSG PACKET_LIBRARIES PACKET_INCLUDE_DIRS)
MARK_AS_ADVANCED(PACKET_LIBRARIES PACKET_INCLUDE_DIRS)
