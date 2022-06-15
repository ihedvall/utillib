---
layout: default
title: Time Functions
---
# Time Functions
The 'util/timestamp.h' include file consist of a large number of time and date conversion routine. 
The standard C++ lack support of several time and date functions, especially formatting of local date and time. 
It seems always return the US date and time formats.

The decision is to use an unsigned 64-bt integer that represent nanoseconds since midnight 1970-01-01 UTC. Note that 
most operating systems defined the time_t as seconds since 1970. 

All internal usage and storage of times and dates shall always be related to the UTC time. Local time and dates are 
converted upon presentation.
