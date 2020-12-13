# What's here?
My research into the HID++ protocol as it pertains to the Logitech G700s

The "wireshark" folder contains a plugin I wrote for [Wireshark](https://www.wireshark.org/) that will extract the hid++ payload and display the binary in a reasonably nice formatted way. As well as a bunch of saved packets that examine the communication that goes on between the mouse and PC
I also make use of the hid++ dissector for wireshark written for the libratbag project: https://github.com/libratbag/ratbag-toolbox/blob/master/dissectors/hidpp-dissector.lua

Inside the "raw" folder you'll find a bunch of simple dumps of data made using my plugin for wireshark

# What do you know so far?
In LGS, when you're in "Automatic game detection mode" (i.e. the software will automatically switch your profile and settings based on which application is currently in focus, allowing you to specify different profiles on a game-by-game basis), the software only appears to transmit settings like DPI, USB polling rate, etc to the mouse itself. Any changes to button bindings do not generate a message over USB at all.
You can also rebind any mouse button (The term "rebind" in this case has a whole host of options you can choose from), even the left/right mouse buttons can be rebound.
The mouse itself just sends a standard physical button event for the button the user pressed, which leads me to believe that LGS intercepts all clicks from the mouse and submits a new event to windows with whatever the user specified in the profile.
This assertion is backed up by the fact that "testing" a profile in LGS will display a "delay" with keypresses. Curiously however, there is no delay displayed for buttons rebound to a standard (left/right/middle/scroll) mouse button.

# What resources have you been making use of?
@cvuchener's excellent repositories and research into the HID++ protocol:
* https://github.com/cvuchener/g500
* https://github.com/cvuchener/hidpp
Libratbag:
* https://github.com/libratbag/libratbag