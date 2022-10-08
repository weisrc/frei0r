# Replace «rpc» by the library name of your filter
# and add the filter directory in src/filter/CMakeLists.txt.
set (SOURCES rpc.cpp)
set (TARGET rpc)

if (MSVC)
  set (SOURCES ${SOURCES} ${FREI0R_DEF})
endif (MSVC)

add_library (${TARGET}  MODULE ${SOURCES})

# No «lib» prefix (name.so instead of libname.so)
set_target_properties (${TARGET} PROPERTIES PREFIX "")

# Do not install a rpc effect ;)
# (Can be installed for testing though.)
install (TARGETS ${TARGET} LIBRARY DESTINATION ${LIBDIR})
