# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(CRYSTAL_TOOLS_SRC
        CrystalToolsMainHeader.cpp
        CrystalToolsModule.cpp
        SuperCellBuilderWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(CRYSTAL_TOOLS_UIS
        SuperCellBuilderWorkPanel.ui
        )

# program resources ------------------------------------------------------------
SET(CRYSTAL_TOOLS_RESOURCES
        CrystalToolsModule.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(CRYSTAL_TOOLS_SRC ${CRYSTAL_TOOLS_RESOURCES})
QT5_WRAP_UI(CRYSTAL_TOOLS_SRC ${CRYSTAL_TOOLS_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(crystal_tools_shared SHARED ${CRYSTAL_TOOLS_SRC})
ADD_DEPENDENCIES(crystal_tools_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(crystal_tools_shared PROPERTIES
                        OUTPUT_NAME CrystalTools
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(crystal_tools_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(crystal_tools_shared Core Widgets Gui Script OpenGL)

INSTALL(TARGETS
            crystal_tools_shared
        DESTINATION
            lib
        )

