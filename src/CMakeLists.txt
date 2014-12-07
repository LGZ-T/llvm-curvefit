include_directories(
	${LLVM_PROF_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR}
	${LLVM_INCLUDE_DIRS} 
	)
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(-fno-rtti)
add_definitions(-Wall --std=c++11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LLVM_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${LLVM_CPP_FLAGS}")

llvm_map_components_to_libraries(LLVM_PRED_LIBRARY bitreader analysis)
add_executable(curve-fit
   curve_fit.cpp get_data.cpp)

target_link_libraries(curve-fit
   ${GSL_LIBRARIES}
   )
