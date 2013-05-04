FakePlugin for Doomseeker by The Doomseeker Team, 2013.
===============================================================================
CONTENTS:
1. What is This?
2. Basic Configuration.
===============================================================================

1. What is This?
===============================================================================
This plugin is designed to simulate a master server and game servers.
It eliminates the necessity to have a live master server and at least one
game server for development purposes. Developers can utilize this plugin when
creating new features for Doomseeker or testing existing ones. 

FakePlugin imitates master server and game servers by actually opening
UDP sockets in local OS. Master server and each fake game server binds its
own UDP socket, so there is a limit of how many servers can be faked at
the same time.

Sockets perform the most basic operations:
They await for incoming browser challenge, sleep for a few milliseconds and
then respond with a predefined message. The plugin itself is capable
of creating proper challenges, and of interpreting the response messages so
that the fake servers appear properly on the Doomseeker's server list.

2. Basic Configuration
===============================================================================
Several values have been prepared for modification, but still these values
are hardcoded into the plugin itself. Changing these values requires
recompilation.

All values that can be modified are located in file
"responder/respondercfg.cpp". Refer to documentation
in "responder/respondercfg.h" to learn what these settings are used for.

