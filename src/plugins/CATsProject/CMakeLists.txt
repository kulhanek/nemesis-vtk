# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(CATS_PROJECT_SRC
        CATsProjectMainHeader.cpp
        CATsProjectModule.cpp
        CATsProject.cpp
        CATsProjectWindow.cpp
        )

# program windows --------------------------------------------------------------
SET(CATS_PROJECT_UIS
        CATsProjectWindow.ui
        )

# program resources ------------------------------------------------------------
SET(CATS_PROJECT_RESOURCES
        CATsProject.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(CATS_PROJECT_SRC ${CATS_PROJECT_RESOURCES})
QT5_WRAP_UI(CATS_PROJECT_SRC ${CATS_PROJECT_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(cats_project_shared SHARED ${CATS_PROJECT_SRC})
ADD_DEPENDENCIES(cats_project_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(cats_project_shared PROPERTIES
                        OUTPUT_NAME CATsProject
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(cats_project_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(cats_project_shared Core Widgets Script)

INSTALL(TARGETS
            cats_project_shared
        DESTINATION
            lib
        )

