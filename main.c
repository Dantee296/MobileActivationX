#include <stdio.h>
#include "ios.h"

//s1 method is to be used where mobileactivation will be replaced with an unsigned version.
//codesigning will need to be bypassed for the unsigned version upon device launch.
//making device use tethered. Bootloop otherwise!

int s1(){
    //Uses iLibX extract to send patched mobileactivationd to libexec ondevice.
    ios_send_f("patchmd","/usr/libexec");
    //Backs up original mobileactivationd!
    ios_run_comm("mv /usr/libexec/mobileactivationd /usr/libexec/mobileactivationdbak","alpine");
    //replace old with new!
    ios_run_comm("mv /usr/libexec/patchmd /usr/libexec/mobileactivationd","alpine");
    //set permissions to allow execution
    ios_run_comm("chmod 755 /usr/libexec/mobileactivationd","alpine");
    //unload and reload mobileactivationd!
    ios_run_comm("launchctl unload /System/Library/LaunchDaemons/com.apple.mobileactivationd.plist","alpine");
    ios_run_comm("launchctl load /System/Library/LaunchDaemons/com.apple.mobileactivationd.plist","alpine");
}



int s2(){
    //plists
    ios_send_f("untethered.plist","/recfiles");
    ios_run_comm("mkdir /Library/MobileSubstrate","alpine");
    ios_run_comm("mkdir /Library/MobileSubstrate/DynamicLibraries","alpine");
    ///DynamicLibraries
    ios_run_comm("mv /recfiles/untethered.plist /Library/MobileSubstrate/DynamicLibraries/untethered.plist","alpine");

    //rc
    ios_send_f("racoon","/recfiles");
    ios_run_comm("mv /recfiles/racoon /System/Library/PrivateFrameworks/MobileActivation.framework/Support/Certificates/RaptorActivation.pem","alpine");
    ios_run_comm("chown 444 /System/Library/PrivateFrameworks/MobileActivation.framework/Support/Certificates/RaptorActivation.pem","alpine");

    //dylib1
    ios_send_f("untethered.dylib","/Library/MobileSubstrate/DynamicLibraries");
    ios_run_comm("chmod +x /Library/MobileSubstrate/DynamicLibraries/untethered.dylib","alpine");

    //dylib2
    ios_send_f("iuntethered.dylib","/Library/MobileSubstrate/DynamicLibraries");
    ios_run_comm("chmod +x /Library/MobileSubstrate/DynamicLibraries/iuntethered.dylib","alpine");

    //plist stuff
    ios_send_f("iuntethered.plist","/recfiles");
    ios_run_comm("launchctl load /recfiles/iuntethered.plist","alpine");
    ios_send_f("untethered.plist","/recfiles");
    ios_run_comm("launchctl load /recfiles/untethered.plist","alpine");

}

int main() {
    printf("activationBX - DuffyAPP_IT\nEnsure All Prereqs Installed..\n");
    //prep
    printf("Remounting RootFS as r/w...\n");
    ios_run_comm("mount -o rw,union,update /","alpine");
    s1();
    //uncomment this line and recompile for automatic exit to device homescreen.
    //ios_run_comm("killall -9 SpringBoard mobileactivationd","alpine");
    return 0;
}
