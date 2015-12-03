# ==============================================================================
# NEMESIS CMake File
# ==============================================================================

# we want GUI application under windows
IF(WIN32)
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mwindows")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -mwindows")
ENDIF(WIN32)

# program objects --------------------------------------------------------------
SET(NEMESIS_SRC
        NemesisApplication.cpp
        main.cpp
        )

# add program icon under win32/mingw -------------------------------------------
if(WIN32)
     set(RES_FILES "nemesis.rc")
     SET(NEMESIS_SRC ${NEMESIS_SRC} ${RES_FILES})
endif(WIN32)

# final build ------------------------------------------------------------------
ADD_EXECUTABLE(nemesis ${NEMESIS_SRC})
QT5_USE_MODULES(nemesis Core Widgets Script)

TARGET_LINK_LIBRARIES(nemesis
                NemesisCore
                ${QT_LIBRARIES}
                ${PLUSULA_LIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

INSTALL(TARGETS
            nemesis
        RUNTIME DESTINATION
            bin
        )
