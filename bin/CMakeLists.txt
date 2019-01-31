# ==============================================================================
# NEMESIS CMake File
# ==============================================================================

INSTALL(FILES
            nemesis
        DESTINATION
            bin
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
        )
