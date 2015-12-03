# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(EXTRA_TOOLS_SRC
        ExtraToolsMainHeader.cpp
        ExtraToolsModule.cpp
        StructureInternalGeometry.cpp
        CompareInternalGeometry.cpp
        AutoRotateWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(EXTRA_TOOLS_UIS
        StructureInternalGeometry.ui
        CompareInternalGeometry.ui
        AutoRotateWorkPanel.ui
        )

# program resources ------------------------------------------------------------
SET(EXTRA_TOOLS_RESOURCES
        ExtraTools.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_WRAP_UI(EXTRA_TOOLS_SRC ${EXTRA_TOOLS_UIS})
QT5_ADD_RESOURCES(EXTRA_TOOLS_SRC ${EXTRA_TOOLS_RESOURCES})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(extra_tools_shared SHARED ${EXTRA_TOOLS_SRC})

SET_TARGET_PROPERTIES(extra_tools_shared PROPERTIES
                        OUTPUT_NAME ExtraTools
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(extra_tools_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                ${EXTRA_TOOLS_LIB}
                )

QT5_USE_MODULES(extra_tools_shared Core Widgets Gui Script OpenGL)

INSTALL(TARGETS
            extra_tools_shared
        DESTINATION
            lib
        )

