# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(SYMMETRY_WP_SRC
        SymmetryMainHeader.cpp
        SymmetryModule.cpp
        SymmolWorkPanel.cpp
        )

# program windows --------------------------------------------------------------
SET(SYMMETRY_WP_UIS
        SymmolWorkPanel.ui
        )

# program resources ------------------------------------------------------------
SET(SYMMETRY_WP_RESOURCES
        SymmetryWP.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(SYMMETRY_WP_SRC ${SYMMETRY_WP_RESOURCES})
QT5_WRAP_UI(SYMMETRY_WP_SRC ${SYMMETRY_WP_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(symmetry_wp_shared SHARED ${SYMMETRY_WP_SRC})
ADD_DEPENDENCIES(symmetry_wp_shared nemesis_core_shared)

SET_TARGET_PROPERTIES(symmetry_wp_shared PROPERTIES
                        OUTPUT_NAME SymmetryWP
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(symmetry_wp_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${OPENGL_glu_LIBRARY}
                ${OPENGL_gl_LIBRARY}
                ${SCIMAFIC_CLIB_NAME}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

QT5_USE_MODULES(symmetry_wp_shared Core Widgets Gui Script OpenGL)

INSTALL(TARGETS
            symmetry_wp_shared
        DESTINATION
            lib
        )

