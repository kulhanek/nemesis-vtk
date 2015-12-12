# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(BIOTOOLS_SRC
        BioToolsMainHeader.cpp
        BioToolsModule.cpp
        ResidueWorkPanel.cpp
        ResidueListWorkPanel.cpp
        StructurePaletteWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(BIOTOOLS_UIS
        ResidueWorkPanel.ui
        ResidueListWorkPanel.ui
        StructurePaletteWorkPanel.ui
        )

# program resources ------------------------------------------------------------
SET(BIOTOOLS_RESOURCES
        BioToolsModule.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(BIOTOOLS_SRC ${BIOTOOLS_RESOURCES})
QT5_WRAP_UI(BIOTOOLS_SRC ${BIOTOOLS_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(biotools_shared SHARED ${BIOTOOLS_SRC})
ADD_DEPENDENCIES(biotools_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(biotools_shared PROPERTIES
                        OUTPUT_NAME BioTools
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(biotools_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(biotools_shared Core Widgets Gui Script OpenGL)

INSTALL(TARGETS
            biotools_shared
        DESTINATION
            lib
        )

