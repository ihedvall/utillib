---
layout: default
title: Listen Daemon
---
# Listen Service/Daemon

The listen daemon is an application that simply run listen servers. On Windows operating system it is launched as
a service by the 'serviced.exe' application while on Linux it runs as a daemon.

The application will also create a shared memory list of all listen servers on this computer. This will later 
be used by the companion listen window application, letting the user select which servers to connect to.

![Overview Listen Functionality](/assets/img/listen.png)

## Configuration

The application is normally run as a service. Windows services are a bit tricky when dealing of command line 
arguments which complicates the configuration of the service. The 'serviceexplorer.exe' simplifies the command 
line argument problem. The executable accept one or more configuration file as input arguments. 

The simplest and recommended way, is to launch the 'listend.exe' executable without any command line argument. 
It will upon startup, search for a 
'listend.xml' configuration file in the '../conf' relative to the '...AppData/Local/UtilLib/bin' installation 
directory. If no config file is found, the executable will start all known listen servers.

## Configuration File

The configuration file is a simple XML formatted file. The file shall have a root tag 'Listend' and one 
child tag 'ListenServer' for each server. Each 'ListenServer' tags may have the following property tags. 

| Property    | Type                 | Description                                                                                                                           |
|-------------|----------------------|---------------------------------------------------------------------------------------------------------------------------------------|
| ShareName   | String (optional)    | Shared memory name                                                                                                                    |
| Name        | String (required)    | Display name of the server                                                                                                            |
| Description | String (recommended) | General description                                                                                                                   | 
| PreText     | String (recommended) | Pre-text short string                                                                                                                 |
| HostName    | String (optional)    | By default set to 127.0.0.1 which allow only users<br/> on this machine to connect.<br/>Use 0.0.0.0 to allow remote users to connect. |
| Port        | Integer (required)   | IP port number.<br/> Ports above 49152 is traditionally used for listen servers.                                                      |
| LogLevel    | String (required)    | Log level menu text.<br/>Add attribute 'level' for assigning a number to the menu.<br/>Add one tag for each menu.                     |


