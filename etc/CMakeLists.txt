# ==============================================================================
# Nemesis CMake File
# ==============================================================================

INSTALL(DIRECTORY
            plugins/definitions
        DESTINATION
            etc/plugins/
        )

INSTALL(FILES
            plugins/ConnectPlugins.sh
        DESTINATION
            etc/plugins/
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
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
