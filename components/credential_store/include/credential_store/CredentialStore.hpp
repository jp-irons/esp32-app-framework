#pragma once

#include "common/Result.hpp"
#include <string>
#include <vector>

namespace credential_store {

struct WiFiCredential {
    std::string ssid;
    std::string password;
    int priority = 0;
};

class CredentialStore {
public:
    CredentialStore(const char* nvsNamespace = "wifi_creds");
	
	std::size_t count();

    common::Result loadAll(std::vector<WiFiCredential>& out);
    common::Result saveAll(const std::vector<WiFiCredential>& entries);

    common::Result add(const WiFiCredential& entry);
	common::Result store(const WiFiCredential& cred);
	std::vector<WiFiCredential> loadAllSortedByPriority();
	common::Result erase(const std::string& ssid);
	common::Result clear();

private:
    const char* ns;
};

} // namespace credential_store