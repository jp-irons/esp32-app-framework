#include "wifi_manager/ProvisioningStateMachine.hpp"

namespace wifi_manager {

ProvisioningStateMachine::ProvisioningStateMachine(
    WiFiManager& wifi,
    credential_store::CredentialStore& store)
    : wifi(wifi)
    , store(store)
    , currentState(ProvisioningState::Idle)
{
}

ProvisioningState ProvisioningStateMachine::state() const
{
    return currentState;
}

} // namespace wifi_manager