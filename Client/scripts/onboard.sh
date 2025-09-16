# Check if script is running with correct permissions
if [ "$(id -u)" -ne 0 ]; then
    echo "Script not running as root exiting"
    exit 2
fi

# Onboard folder
onboardFolder="/opt/sauron"
onboardFilePath="/opt/sauron/onboard.json"

# Generate Tenant GUID and Machine GUID that will serve as machineID
MachineId=$(uuidgen)
echo "MachineId: $MachineId"

TenantId=$(uuidgen)
echo "TenantId: $TenantId"

# Generate onboarding blob
OnboardingBlob=$(cat <<EOF 
{
    "TenantId": "$TenantId",
    "MachineId": "$MachineId"
} 
EOF
)

# Check if onboarding folde exists
if [ ! -d "$onboardFolder" ]; then
    echo "Creating onboard directory since it does not exist"
    echo "Creating in: $onboardFolder"
    mkdir -p "$onboardFolder"
fi

# Now check if the file for onboarding exists
if [ -f "$onboardFilePath" ]; then
    echo "Device is already onboard, please check file: $onboardFilePath"
    exit 1
fi

# Write onboarding blob
echo "$OnboardingBlob" > "$onboardFilePath"

# Device was onboarded
echo "Device was not onboarded, onboarded device: $onboardFilePath"
