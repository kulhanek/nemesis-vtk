# ==============================================================================
# Nemesis CMake File
# ==============================================================================

INSTALL(DIRECTORY
            plugins/definitions
        DESTINATION
            etc/plugins/
        )

INSTALL(DIRECTORY
            graphics
        DESTINATION
            etc/
        FILES_MATCHING PATTERN
            "*.xml"
        )

INSTALL(DIRECTORY
            workpanels
        DESTINATION
            etc/
        FILES_MATCHING PATTERN
            "*.xml"
        )

INSTALL(DIRECTORY
            desktop
        DESTINATION
            etc/
        FILES_MATCHING PATTERN
            "*.xml"
        )

INSTALL(DIRECTORY
            desktop
        DESTINATION
            etc/
        FILES_MATCHING PATTERN
            "*.dsk"
        )
