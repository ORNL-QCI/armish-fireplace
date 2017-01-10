# armish-fireplace

## Introduction

Armish-fireplace is a middleware container. It allows asynchronous rx and tx for the client implemented with a multithreaded buffer. It contains modules which organizes module-specific processing units. These processing units are the heart of the processing armish-fireplace does. A processing unit may be a hardware device driver or a hardware simulation driver. Processing units can be loaded/unloaded at runtime changing functionality. 

## Installation

Armish-fireplace requires the following dependencies:
* CMake for build process management
* zmq for the client interface
* cppzmq for C++ bindings to zmq
* RapidJSON for the client interface
* (optional) Doxygen for code documentation

Ensure all required items are installed on your system before attempting to build. If you do not have binaries available, each is available on Github:
* [CMake](https://github.com/Kitware/CMake)
* [0MQ](https://github.com/zeromq/libzmq)
* [Doxygen](https://github.com/doxygen/doxygen)

The rest of the dependencies are implemented as [Git submodules](https://git-scm.com/docs/git-submodule):
* RapidJSON
* cppzmq

For each submodule, it will be necessary to clone the codebase into the armish-fireplace root directory. To do so, execute the follwing:
```shell
$ git submodule update --init
```

Thus, a typical initial installation would look something like:
```shell
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ../
$ make
$ sudo make install
```

A typical update install would look like:
```shell
$ git pull
$ git submodule foreach git pull origin master
$ make
$ sudo make install
```

Various build options are available when running *cmake ../*

* -DBUILD_RELEASE={ON/OFF}

	Specify if you wish to build for release. If *NO*, then debug is selected and no compiler optimizations are made while assert statements are not stripped from the resulting binary. If *YES*, then release is selected and maximum compiler optimizations are made while assert statements are stripped from the resulting binary. It is important to note that once you run _cmake -BUILD_TYPE ../_ in a directory, CMake will cache the value even if you change it later. The easiest way around this is having two build directories, one for debug and one for release.

	Unless you are testing changes to the code, you should just stick with the default value.

* -DUSE_STATIC={ON/OFF}

	Specify if you wish to use static libraries when building.

## Running

Configuration at startup via command line arguments configures the client interface as well as the module and processing unit properties 

parameter | name | type | required | default
--- | --- | --- | --- | ---
-i | *Input Server Endpoint* | string | yes | *none*
-o | *Output Server Endpoint* | string | yes | *none*
-m | *module name* | string | yes | *none*
-n | *module parameters* | string | no | empty
-t | *processing unit name* | string | yes | *none*
-u | *processing unit parameters* | string | no | empty

A word of caution: If you wish to set *interface* to localhost, you __must__ use 127.0.0.1 as zmq will not correctly parse the former. ZMQ respons to IP addresses of the interface or the interface name, another alternative for localhost on Linux is normally "lo".

The *module parameters* and *processing unit parameters* depend on the module and processing unit selected. The format may vary, so see the processing unit in the module chosen to view what is required.

## Documentation

To generate code documentation, execute the following:
```shell
$ doxygen docs.conf
```
This will generate and save documentation in docs/ which may be viewed by pointing your web browser to docs/html/index.html.

## Programming

### Style
Variables use camelCase while everything else use underscore_seperated_words.

### Overview

Armish-fireplace consists of three main components:

* Server interface
* Buffers
* Module & processing units

The server interface is what the client connects to. The buffers stored incoming and outgoing data to the client. The module contains processing units, which define the behavior of the middleware. A processing unit is either a hardware driver or a virtual hardware driver. Thus you may switch between actual hardware and simulated hardware by changing which processing unit is loaded.

### Modules

The default module is named *brazil* and may be found in module/brazil/. Brazil processing units may be found in module/brazil/proc_units. If you wish to add a processing unit, view the existing processing units to see how it is done. After the processing unit files are added, you have to let Brazil know they are there. Do this by adding your processing unit to *brazil::populate_proc_units()*.
