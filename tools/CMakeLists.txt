include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(xfutil_example xfutil_example.cpp)
target_link_libraries(xfutil_example xfutil pthread)



