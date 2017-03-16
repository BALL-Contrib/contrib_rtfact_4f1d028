RTfact: A generic real-time ray tracing library
-----------------------------------------------

About
-----

RTfact is a generic real-time ray tracing library. It was designed to be able
to support different kinds of rendering algorithms, including ray intersections,
shading, scene management. It uses generic programming and C++ template
techniques extensively in order to achieve high flexibility and code reuse,
while at the same time maintaining maximum performance for each specific
configuration.
RTfact has been originaly developed by Iliyan Georgiev <georgiev@cs.uni-sb.de>
and Felix Klein (lachsen@graphics.cs.uni-saarland.de). It is now maintained by a
team of people from Computer Graphics Chair at Saarland University.

Contact
-------

name:        Lukas Marsalek
e-mail:      marsalek@cs.uni-saarland.de
institution: Computer Graphics Group at Saarland University
             Saarbruecken, Germany

name:	     Iliyan Georgiev
e-mail:	     georgiev@graphics.cs.uni-sb.de
institution: Computer Graphics Group at Saarland University
	     Saarbruecken, Germany

Copyright
---------

The full list of author is in the header of each file. Where missing
refer to the AUTHORS.txt file.

License terms
-------------

The RTfact libraries are available under the terms of the 
GNU General Public License (GPL), version 2 (see LICENSE.GPL).

The SCons based build system is available under the terms of the 
GNU General Public License (GPL), version 2 (see LICENSE.GPL).

The provided external tools are provided with their own licenses. Check
the corresponding web sites for more information.

RTfact Directory Hierarchy
--------------------------

bootstrap		: automation scripts for build system
contrib			: third-party libraries
demos			: small standalone demo applications
doc			: documentation
doxydoc			: dummy directory for doxygen-generated documentation
include/RTfact		: core RTfact headers
include/RTremote	: RTremote C++ API headers
include/RThandle
include/RTpie		: RTpie C COM-like API headers
remote			: implementation of RTremote C++ API
rtpie			: implementation of RTpie C COM-like API
tools			: support tools


