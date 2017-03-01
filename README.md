# Table of Contents

- [About](#about)
- [Building the project](#building-the-project)
  - [Build environment](#build-environment)
  - [Build configurations](#build-configurations)
  - [Build instructions](#build-instructions)
- [Configuring the setup](#configuring-the-setup)
  - [Setup resources](#setup-resources)
  - [The main setup configuration file](#the-main-configuration-file)
  - [Setup schemes](#setup-schemes)
  - [Setup screens (dialogs)](#setup-screen-dialog)
    - [Screen types](#screen-types)
    - [Screen sequence](#screen-sequence)
    - [Screen attributes](#screen-attributes)
    - [Screen resource](#screen-resource)
  - [String table](#string-table)
  - [Online installers](#online-installers)
  - [Offline installers](#offline-installers)

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
| Debug_Online | :heavy_check_mark: | Download installer from *remote sites* |
| Debug_Offline | :heavy_check_mark: | Drop installer from *embedded resource* |
| Release_Online | | Download installer from *remote sites* |
| Release_Offline | | Drop installer from *embedded resource* |

## Build instructions ##
* Open [SetupProject/SetupProject.sln](SetupProject/SetupProject.sln).
* Build the project like any other Visual Studio projects.

# Configuring the setup #
## Setup resources ##
All the setup resources (like images, HTML, CSS, setup files ...) are embedded in the final setup executable's [resources](https://en.wikipedia.org/wiki/Resource_(Windows)). Each resource entry is referenced by its **name** and **type**.

All resource files used by the project are stored in the [SetupProject/SetupProject/res](SetupProject/SetupProject/res) directory. To add a new resource entry, modify or delete an existing one, user can edit the resource descriptor file located at [SetupProject/SetupProject/res/SetupProject.rc2](SetupProject/SetupProject/res/SetupProject.rc2).

Each resource entry is referenced in this documentation by the following notation: **RES_TYPE**/**RES_NAME**.

## The main setup configuration file ##
The main setup configuration file is the resource entry named **SETUP**/**CONFIG**. This is an XML file having the following structure:
- **setup_config** (root entry, the default scheme ID is specified here)
    - **scheme 1**
    - **scheme 2**
    - ...
    
In XML format:
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

Each setup scheme describes a complete setup progress. The configuration file may contain multiple setup schemes but **only one setup scheme** is fed to the setup framework to perform the full installation from A to Z.

The chosen scheme's ID is specified in the "**scheme**" attribute of the root entry **setup_config**. The setup framework will then scan all the schemes and find the one whom the ID matches to proceed. The structure of an setup scheme is described in the next section.

## Setup schemes ##
The setup scheme has the following structure:

- **scheme**
  - **setup_screens**: sequence of setup screens (dialogs). Details are described [here](#setup_screens_dialogs).
    - **screen 1**: an setup screen entry.
    - **screen 2**: an setup screen entry.
    - ...
  - **text**: the string table used the graphical user interface. Details are described [here](#string-table).
  - **installer**: the online/offline installers.
    - **online_installer**: list of online installers. Details are described [here](#online-installers)
      - **installer1**: an online installer entry.
      - **installer2**: an online installer entry.
      - ...
    - **offline_installer**: list of offline installers. Details are described [here](#offline-installers)
      - **installer1**: an offline installer entry.
      - **installer2**: an offline installer entry.
      - ...

In XML format:
```xml
<scheme id="my_id">
    <setup_screens>
        <!-- Sequence of setup screens.
        They will be displayed in the same order as described in this XML -->
        <screen id="screen1"></screen>
        <screen id="screen2"></screen>
    </setup_screens>
    
    <text></text> <!-- String table -->
    <installer>
        <online_installer>
            <!-- Online installer entries -->
            <installer></installer>
            <installer></installer>
        </online_installer>
        
        <offline_installer>
            <!-- Offline installer entries -->
            <installer></installer>
            <installer></installer>
        </offline_installer>
    </installer>
</scheme>
```

## Setup screen (dialog) ##
### Screen types ###
There are several predefined types of dialog:

| Type | Description | Sample |
| :--- | :--- | :--- |
| **config** | Allowing user to change the setup configuration (create shortcuts, make system auto start ...). |
| **eula** | Displaying End User License Agreement (EULA). |
| **main** | The main setup screen displaying installing progress and status. |
| **message** | Helper screen displaying information message, error message ... |
| **confirm** | Helper screen displaying confirmation message (yes/no question). |

### Screen sequence ###
The "*message*" and "*confirm*" screens are just helper screens and are not included in the setup sequence. The **main** screen is always the latest screen to be shown. Thus other screen types (**config** and **eula**) are optional and are displayed in the same order as described in the configuration xml file. Some examples of screen sequence configuration are shown below.

#### Example #1: config - eula - main ####
```xml
<setup_screens>
    <screen id="config" ...>...</screen>
    <screen id="eula" ...>...</screen>
    <screen id="main" ...>...</screen>
</setup_screens>
```

#### Example #2: eula - config - main ###
```xml
<setup_screens>
    <screen id="eula" ...>...</screen>
    <screen id="config" ...>...</screen>
    <screen id="main" ...>...</screen>
</setup_screens>
```

#### Example #3: eula - main ###
```xml
<setup_screens>
    <!-- The "config" screen is omitted -->
    <screen id="eula" ...>...</screen>
    <screen id="main" ...>...</screen>
</setup_screens>
```

#### Example #4: config - main ###
```xml
<setup_screens>
    <!-- The "eula" screen is omitted -->
    <screen id="config" ...>...</screen>
    <screen id="main" ...>...</screen>
</setup_screens>
```

#### Example #5: main ###
```xml
<setup_screens>
    <!-- Both the "config" screen and "eula" screen are omitted -->
    <screen id="main" ...>...</screen>
</setup_screens>
```

### Screen attributes ###
Each setup screen (dialog) entry has the following attributes:

| Attribute name | Required | Type | Meaning |
| :--- | :--- | :--- | :--- |
| id | :heavy_check_mark | text | The screen type. It must be one of the [predefined types](#screen-types). |
| resource | :heavy_check_mark: | text | The **resource name** of the [HTML file](#screen-resource) describing the screen's components and layout. |
| no_border | | text | Set to "*true*" or "*yes*" to indicate that this screen has no border, set to "*false*" or "*no*" otherwise. This value is set to "*false*" if not explicitly specified. |
| top_most | | text | Set to "true" or "yes" to indicate that this screen should be top-most, set to "*false*" or "*no*" otherwise. The value is set to "*false*" if not explicitly specified. |
| title | | text | Specify the text ID of the text to be set as screen's title (This is the text ID, **not the text it-self**. See the [String table](#string-table) section below for more information). |
| width | | text | An integer number representing the width, in pixels, of the screen. |
| height | | text | An integer number representing the height, in pixels, of the screen. |
|

### Screen resource ###
Screen resource is an HTML file with CSS supported describing the screen's components and layout. In general, the screen resource is quite flexible and can be in any form. However, each screen type requires some HTML control elements being rightly configured in order to properly work.

## String table ##
The string table stores strings used by the graphical user interface. Each string has its own unique ID and may contain multiple content entries, each entry represents text in a specified language. Below is an simple example:

```xml
<?xml version="1.0" ?>
<text_table>
    <!-- Each text has its own unique ID   -->
    <text id="accept">
        <!-- Each text may contain multiple text entries,
        each entry represents text in a specified language -->
        
        <!-- Entry with no lang(language) attribute is a "default" entry,
         it will be used in case no specific language provided or 
         the text for the given language not found -->
        <entry>Accept</entry>
        
        <!-- Entry represents text in Vietnamese -->
        <entry lang="vn">Chấp nhận</entry>
        
        <!-- Entry represents text in American English -->
        <entry lang="en_US">Accept</entry>
    </text>
    
    <text id="yes">
        <entry lang="vn">Có</entry>
        <entry lang="en_US">Yes</entry>
    </text>
    
    <text id="ok">
        <entry>OK</entry>
    </text>
</text_table>

```

## Online installers ##

## Offline installers ##