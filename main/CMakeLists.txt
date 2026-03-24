idf_component_register(
    SRCS
        app_main.cpp
        ApplicationContext.cpp
        WiFiApiHandler.cpp
        ProvisioningServer.cpp
        ProvisioningApiHandler.cpp
        RuntimeServer.cpp
        CredentialStore.cpp
        CredentialApiHandler.cpp
        EmbeddedFiles.cpp
        EmbeddedFilesRegister.cpp
        serveEmbedded.cpp

    REQUIRES wifi_manager json
    
    INCLUDE_DIRS
        .

    EMBED_FILES
        ui/favicon.ico
        ui/provision/index.html
        ui/provision/provision.js
        ui/provision/tailwind.css
)