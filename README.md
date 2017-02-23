# About #
This is a very simple setup framework built on top of the Microsoft Foundation Class (MFC). It allows configuring the setup progress via several XML, HTML and CSS files.

# Building the project #
## Build environment ##
* VS 2013 or higher versions (with MFC library)
* Windows SDK 7.1 or later.
* MSXML SDK version 6.0 (included in decent versions of Windows SDK)

## Build instructions ##
* Open [SetupProject/SetupProject.sln](SetupProject/SetupProject.sln).
* Build the project like any other Visual Studio projects.

# Configuring the setup #
## Setup resources ##
All the setup resources (like images, HTML, CSS, setup files ...) are embedded in the final setup executable file's [resources](https://en.wikipedia.org/wiki/Resource_(Windows)). Each resource entry is referenced by its **name** and **type**.

All resource files used by the project are stored in the [SetupProject/SetupProject/res](SetupProject/SetupProject/res) directory. To add a new resource entry, modify or delete an existing one, user can edit the resource descriptor file located at [SetupProject/SetupProject/res/SetupProject.rc2](SetupProject/SetupProject.res/SetupProject.rc2).

Each resource entry is referenced in this documentation by the following notation: **RES_TYPE**/**RES_NAME**.

## The main setup configuration file ##
The main setup configuration file is the resource entry named **SETUP**/**CONFIG**. This is an UTF-8 encoded XML file having the following structure:

```xml
<xml >

</xml>
```
