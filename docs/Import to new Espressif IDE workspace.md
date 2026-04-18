# Import project to Espressif IDE

Espressif IDE already installed.

Open IDE
Browse for workspaces
Create crisp new folder
Select that folder
Launch
Proceed to complete setup to enable full functionality - Yes
Didn't launch EIM (installation manager)
Import projects -> git -> Projects from Git -> Clone uri
Authentication was magically prefilled from elsewhere
Paste https://github.com/jp-irons/esp32-app-framework
Selected main branch
Selected new Eclipse Workspace as location
	C:\Users\jon\Workspace\Eclipse_IDF_Workspace_Test_Build
filename esp32-app-framework
Save -> Next
Cancel
Import projects -> Espressif -> Existing IDF Project
	Browse to location of git folder above.
	Project Name esp32-app-framework
Finish
Selected esp32s3 for 'on:'
Hit Run button (green right arrow)
Built fine
Bit of mucking about to get usb port recognised. Un/replugging did the trick
