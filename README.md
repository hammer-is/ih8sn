# ih8sn

ih8sn allows you to modify system properties at runtime. It can be installed with ADB root or recovery. Proceed at your own risk.

## Disclaimer:

```
- The user takes sole responsibility for any damage that might arise due to use of this tool.
- This includes physical damage (to device), injury, data loss, and also legal matters.
- The developers cannot be held liable in any way for the use of this tool.
```

## If you want to add your device, do a PR with your device config.

## Requirements

- Android platform tools
- Android device

## Installation

### 1: Download ih8sn

Check the "Releases" section on the right.

### 2: [Optional] Configure ih8sn.conf inside the zip for your device

- Modify ih8sn.conf for your device and save it as ih8sn.conf.`<codename>` in etc.
  - **\#** at the beginning of the line will ignore the contents
  - line is prefixed with **init** or **boot_completed** to for the commands to be executed at either the init stage or the boot_completed stage
  - **set** uses regex to find matching property names and one command can update multiple properties to the same value. E.g. set,ro\\.(|boot\\.|bootimage\\.)theend=123 will match ro.theend, ro.boot.theend and ro.bootimage.theend and set all found properties to 123.
  - **add** updates a property or inserts it if it does not exists.
  - **replace** searches for regex matches in the value of all properties. E.g. replace,userdebug=user will replace the substring userdebug with user in all properties. If multiple replace commands are used they will be applied on after another making it possible to replace multiple substrings in same property value.
  - **delete** uses regex to find all properties with matching names and delete them. E.g. delete,.\*unsafe.\* will delete all propeties containing the text unsafe like ro.someunsafe.property and ro.boot.unsafe.theend.

### 3: Push the files to your device (and optionally test the .conf used)

#### 1. ADB root
```
- Extract ih8sn zip in your PC.
- Enable usb debugging and rooted debugging in developer options in your phone. 
```
Run the script according to your system.

Windows :
```
.\push.ps1 -use_remount -simulation
```
Linux :
```
./push.sh --use_remount --simulation
```
**simulation** runs the application in a mode where no properties are changed or deleted but only simulates the changes and outputs to stdout
```
adb root
adb shell "system/bin/ih8sn init simulation"
adb shell "system/bin/ih8sn boot_completed simulation"
```
#### 2. Recovery method
```
Reboot to recovery and select Apply update -> Apply from ADB
```
Run this in terminal.
```
adb sideload ih8sn*.zip
```

### 4: Reboot your device 
<br>

## Notes: 
```
- Run script or flash zip again to uninstall.
- Spoofing is preserved after ota updates if possible.
```
