add_subdirectory(Core)
add_subdirectory(Pipeline)
add_subdirectory(Frontend)
add_subdirectory(Image)
add_subdirectory(Octave)
add_subdirectory(Algorithm)
#add_subdirectory(Wrapper)

configure_file(Config.h.in Config.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Config.h DESTINATION include/mvp)
