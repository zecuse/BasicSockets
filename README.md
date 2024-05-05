# BasicSockets
This is a very simple command line app that can be run on a Windows or Unix system.

## Command Line
The following parameters can be supplied to the command:
|Long name|Short name|Example&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|Description|
|---|---|---|---|
|setup|-s|`--setup client`|Which mode the app will run in. Client and server are that only valid options.|
|ip|-i|`--ip 127.0.0.1`|Which IP address the app will associate with. Valid IPv4 and IPv6 addresses are accepted.|
|port|-p|`--port 8080`|Which port the app will associate with. 0 - 65535 are accepted but whether they work is dependant on your system.|
|type|-t|`--type TCP`|Which protocol the app will use. TCP and UDP are the only valid options.|

The above examples represent the default values used should no other parameters be provided to the command. Additionally, providing `server` as the only parameter to the command will run the app in server mode along with the rest of the default values.

Invalid values will be reverted back to the defaults. The parameters are case-insensitive. An equals symbol (`=`) may be used between a switch and its argument instead of spaces. Unrecognized switches will be ignored.

The app will parse a provided IP address and determine if IPv4 or IPv6 will be used.

## Visual Studio
Using Visual Studio, you can setup startup items that provide command line arguments as if the app were run in a command line interface. In the `Folder View` of the `Solution Explorer`, right click anywhere and select `Open Debug and Launch Settings`.

![Visual Studio's `Open Debug and Launch Settings`](https://github.com/zecuse/BasicSockets/assets/19420775/c4f2576a-eb2c-4d32-81fe-386fd6de2533)

This will create a `launch.vs.json` file within your `.vs` folder where you can fill out different launch items.

![4 Basic launch profiles](https://github.com/zecuse/BasicSockets/assets/19420775/4bdfb549-3a13-4005-bdd5-b1511f4065a3)

The image above will create 4 launch profiles equivalent to launching the app with the following command parameters (Windows examples):
1. BasicSockets.exe
2. BasicSockets.exe server
3. BasicSockets.exe --setup=client --ip=127.0.0.1 --port=8080 --type=UDP
4. BasicSockets.exe -s server -i 127.0.0.1 -p 8080 -t UDP

These profiles will be found in the `Select Startup Item` dropdown of the `Standard` toolbar.

![Launch profiles location](https://github.com/zecuse/BasicSockets/assets/19420775/3601290d-d5ee-4d90-8674-c49e78f81772)
