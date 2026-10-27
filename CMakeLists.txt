cmake_minimum_required(VERSION 3.19)

# the version is taken from the latest debian/changelog entry
file(STRINGS debian/changelog changelog LIMIT_COUNT 1)
string(REGEX MATCH "[0-9.]+" version "${changelog}")

project(qmclient VERSION ${version} LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSSH2 REQUIRED IMPORTED_TARGET libssh2)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(qmclient WIN32 MACOSX_BUNDLE
	main.cpp
	qmclient.cpp qmclient.h
	client.cpp client.h
	leaves.cpp leaves.h leaf.h
	mddelegate.cpp mddelegate.h
	qssh.cpp qssh.h
	treeitem.cpp treeitem.h
	treemodel.cpp treemodel.h
	qmclientwidget.ui
	qmclient.qrc
)

if(WIN32)
	target_sources(qmclient PRIVATE qmclient.rc)
endif()

target_link_libraries(qmclient PRIVATE Qt6::Widgets Qt6::Network PkgConfig::LIBSSH2)
target_compile_definitions(qmclient PRIVATE "VERSION=\"${PROJECT_VERSION}\"")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(qmclient PRIVATE -Wall -Wextra)
endif()

install(TARGETS qmclient
	BUNDLE DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
	install(FILES qmclient.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
	install(FILES icons/qmclient.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
endif()
