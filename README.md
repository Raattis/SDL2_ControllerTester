# SDL2 Controller Tester by Raattis
give me feedback on PICO-8 forums

## Usage
1. Unzip exe and SDL2.dll to a folder
2. Run exe

Supports hot-swapping controllers.

## Explanation
The application automatically visualizes the values of every Joysticks' every button, axis and hat. Different joysticks' values are displayed on top of eachother so if you want to only see info on one joystick unplug the other ones.

Press C to change from Joystick mode to GameController mode. (To return to joystick mode restart the program.)

Joystick's layout is raw output from the device where as GameControllers can have virtual button mappings and is designed to simulate an XInput device. For example, GamepadTool (http://www.generalarcade.com/gamepadtool/) takes Joystick layout and maps it to a GameController.
