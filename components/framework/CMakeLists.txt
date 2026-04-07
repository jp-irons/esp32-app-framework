idf_component_register(
    SRCS
        FrameworkContext.cpp

    INCLUDE_DIRS
        include

    REQUIRES
        core_api
        credential_store
        http
        static_assets
        wifi_manager
)