# Open-Source-Gaming-Peripheral-Hub
This is an attempt to make an open source alternative to software like "Logitech Gaming Software"

## Why does this exist?
I own a Logitech G700s gaming mouse, and the official software constantly breaks for me in that I frequently have to close and open it for it to not break the current mouse settings. Also, it looks like I'm SOL for offial LGS support because both my mouse is discontinued, and LGS has been deprecated in favour of their new "Hub" software (which does not support the G700s).

## What is the current state of the project?
It's in R&D, meaning any code right now is experimental and the actual software you might be hoping to find here **does not yet exist**.

## What are your goals for this project?
To start with, I want this to entirely replace LGS for my G700s. Meaning that LGS will no longer be required in any way to use your mouse.
Next, I'd like to extend the software to support all mice that use the HID++ protocol (So a bunch of other Logitech mice)
After that, if it's feasible, I'd like to add support for mice from other manufacturers (Steelseries, Razor, Corsair etc)

Functionality-wise, I need this software to support profiles, changing DPI and button bindings etc. I don't really have any plans for anything RGB related (as the G700s has no RGB)

## How will this project be structured?
I'm hoping to make a platform independent "core", which contains all of the important functionality, and a windows specific gui to sit on top. Ultimately the point of this project is to make a windows app. If you want something for Linux right now, check out [libratbag](https://github.com/libratbag/libratbag)
