cmake_minimum_required(VERSION 3.12)

project(geBVH)

find_package(GPUEngine COMPONENTS geGL geSG geCore REQUIRED)
find_package(glm REQUIRED)

set(mortonShader ${CMAKE_CURRENT_LIST_DIR}/src/kernels/mortonCodeComputation.cs)
set(radixSortShader ${CMAKE_CURRENT_LIST_DIR}/src/kernels/parallelRadixSort.cs)
set(treeShader ${CMAKE_CURRENT_LIST_DIR}/src/kernels/radixTreeBuilder.cs)

add_library(${PROJECT_NAME}
    src/AABB_SAH_BVH.cpp
	src/GeneralCPUBVH.cpp
	src/GeneralGPUBVH.cpp
    src/RadixTree_BVH.cpp)
	
target_compile_definitions(${PROJECT_NAME} PUBLIC "MORTON_KERNEL=\"${mortonShader}\""
												  "RADIX_SORT_KERNEL=\"${radixSortShader}\"" 
												  "TREE_KERNEL=\"${treeShader}\"")	
	
target_include_directories(${PROJECT_NAME}
    PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE src)
	
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

target_link_libraries(${PROJECT_NAME} PUBLIC geSG geGL geCore glm)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/geAd/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}Config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})