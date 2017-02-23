# Table of Contents

- [About](#about)
- [Building the project](#building-the-project)
  - [Build environment](#build-environment)
  - [Build configurations](#build-configurations)
  - [Build instructions](#build-instructions)
- [Configuring the setup](#configuring-the-setup)
  - [Setup resources](#setup-resourcs)
  - [The main setup configuration file](#the-main-configuration-file1)
  - [Setup schemes](#setup-schemes)
  - [Screens](#screens)
  

# About #
This is a very simple setup framework built on top of the Microsoft Foundation Class (MFC). It allows configuring the setup progress via several XML, HTML and CSS files.

# Building the project #
## Build environment ##
* Microsoft Visual Studio 2013 or higher versions (with the MFC library).
* Windows SDK 7.1 or later.
* MSXML SDK version 6.0 (included in decent versions of Windows SDK).

## Build configurations ##
There are 4 build configurations:

| Configuration name | Has debug information? | Installer type |
| :--- | :--- | :--- |
Debug_Online | :white_check_mark: | Download installer from *remote sites*
Debug_Offline | :white_check_mark: | Drop installer from *embedded resource*
Release_Online | :x: | Download installer from *remote sites*
Release_Offline | :x: | Drop installer from *embedded resource*

## Build instructions ##
* Open [SetupProject/SetupProject.sln](SetupProject/SetupProject.sln).
* Build the project like any other Visual Studio projects.

# Configuring the setup #
## Setup resources ##
All the setup resources (like images, HTML, CSS, setup files ...) are embedded in the final setup executable's [resources](https://en.wikipedia.org/wiki/Resource_(Windows)). Each resource entry is referenced by its **name** and **type**.

All resource files used by the project are stored in the [SetupProject/SetupProject/res](SetupProject/SetupProject/res) directory. To add a new resource entry, modify or delete an existing one, user can edit the resource descriptor file located at [SetupProject/SetupProject/res/SetupProject.rc2](SetupProject/SetupProject.res/SetupProject.rc2).

Each resource entry is referenced in this documentation by the following notation: **RES_TYPE**/**RES_NAME**.

## The main setup configuration file ##
The main setup configuration file is the resource entry named **SETUP**/**CONFIG**. This is an UTF-8 encoded XML file having the following structure:

```xml
<?xml version="1.0" ?>
<setup_config scheme="scheme_id">
    <!-- The first scheme -->
    <scheme id="scheme_id_1">
        <!-- Scheme data -->
    </scheme>

    <!-- The second scheme -->
    <scheme id="scheme_id_2">
        <!-- Scheme data -->    
    </scheme>
    
    <!-- Other setup schemes go here -->
</setup_config>
```

Each setup scheme describes a complete setup progress. The configuration file may contain multiple setup schemes but **only one setup scheme** is fed to the setup framework to perform the installation from A to Z.

The chosen scheme's ID is specified in the "**scheme**" attribute of the root entry **setup_config**. The setup framework will then scan all the schemes and find the one whom the ID matches to proceed. The structure of an setup scheme is described in the next section.

## Setup schemes ##
