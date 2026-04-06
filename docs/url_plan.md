
/               → provisioning/index.html
/index.html     → provisioning/index.html
/provision/*    → provisioning static files
/api/*          → provisioning API handlers

Runtime content
/               → app/index.html
/index.html     → app/index.html
/app/*          → app static files
/api/*          → runtime API handlers


Core APIs
/api/core/info
/api/core/reboot
/api/core/loglevel
/api/core/uptime
/api/core/wifi/status
/api/core/wifi/scan
/api/core/credentials/list
/api/core/credentials/add
/api/core/credentials/delete

/api/provisioning/status
/api/provisioning/scan
/api/provisioning/submit
/api/provisioning/reset

Runtime only
/api/runtime/wifi/status
/api/runtime/wifi/scan
/api/runtime/device/info
