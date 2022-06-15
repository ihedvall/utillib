---
layout: default
title: Listen Functions
---
# Listen Functions
The listen functionality is a sort of debugging tool for applications. It transfers debug messages 
from one or more applications to a listen window. The listen window connects to a listen TCP/IP server
which then starts to send listen messages to the connected window. 

If no listen window is connected, no messages are generated at the source. This means that applications can deliver
debugging functionality in production code without loosing performance.

![Overview Listen Functionality](/assets/img/listen.png)

## Listen Objects
An application either create a listen proxy or server. The proxy object communicate with a server object through a 
named shared memory. The listen server in this case, is created by the listen daemon (service on Windows). 
A proxy solution is used when the debug information is spread among many applications typical listen on database calls.

The server solution can be used if listen messages shall be generated only from one application, 
for example a protocol driver. 

A listen window may set a log level. The log level is just a number indicating what type of messages,
the server or proxy shall generate. When a listen client is started, it asks for available log levels. The server 
replies with a list of log levels and short texts for displaying on the window 'Log Level' menu.  

## Listen Message
A listen message consist of time, pre-text and message text. The pre-text is added before the message text and 
is a short description of the application that generated the message.

## Listen Daemon
For convenience, the daemon is a stand-alone non-GUI application that start and supervise listen servers. 
The listen servers communicate with its proxy objects which communicate over shared memory. The daemon also 
holds a list of all available listen servers on the computer.

## Listen Window
The listen window is a simple GUI application that shows messages from one or more listen servers.
