# ==============================================================================
# Nemesis CMake File
# ==============================================================================

# objects in library -----------------------------------------------------------
SET(SURFMODELS_SRC
        SurfaceModelsMainHeader.cpp
        SurfaceModelsModule.cpp
        BALL/STRUCTURE/triangulatedSES.C
        BALL/STRUCTURE/SESEdge.C
        BALL/STRUCTURE/solventAccessibleSurface.C
        BALL/STRUCTURE/triangle.C
        BALL/STRUCTURE/SASEdge.C
        BALL/STRUCTURE/triangulatedSAS.C
        BALL/STRUCTURE/triangleEdge.C
        BALL/STRUCTURE/trianglePoint.C
        BALL/STRUCTURE/SESFace.C
        BALL/STRUCTURE/SASFace.C
        BALL/STRUCTURE/SASVertex.C
        BALL/STRUCTURE/SESVertex.C
        BALL/STRUCTURE/triangulatedSurface.C
        BALL/STRUCTURE/solventExcludedSurface.C
        )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# program windows --------------------------------------------------------------
SET(SURFMODELS_UIS
        )

# program resources ------------------------------------------------------------
SET(SURFMODELS_RESOURCES
        SurfaceModelsModule.qrc
        )

# QT5 related binding ----------------------------------------------------------
QT5_ADD_RESOURCES(SURFMODELS_SRC ${SURFMODELS_RESOURCES})
#QT5_WRAP_UI(SURFMODELS_SRC ${BUILD_PROJECT_UIS})

# Nemesis uses plugins, so dynamic linking is obligatory!

# create shared library --------------------------------------------------------
ADD_LIBRARY(surfmodel_shared SHARED ${SURFMODELS_SRC})

SET_TARGET_PROPERTIES(surfmodel_shared PROPERTIES
                        OUTPUT_NAME SurfaceModels
                        CLEAN_DIRECT_OUTPUT 1
                        VERSION ${NEMESIS_VERSION})

TARGET_LINK_LIBRARIES(surfmodel_shared
                NemesisCore
                plusula
                ${QT_LIBRARIES}
                ${HIPOLY_LIB_NAME}
                ${SYSTEM_LIBS}
                )

INSTALL(TARGETS
            surfmodel_shared
        DESTINATION
            lib
        )

