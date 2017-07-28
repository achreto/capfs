Capability File System
======================


About
-----

The capability file system (CAP-FS) allows to mount a file system that is 
backed with byte addressible memory that is protected using capabilities for
direct memory accesses or alternatlively also for RDMA style accesses.


License
-------

Please see LICENSE file.


Contributors
------------

Please see AUTHORS file.


How to use
----------

After building and installing, CAP-FS can be used with the following
command:

    $ cap-fs <options> mountpoint

The file system can be unmounted again using:

    $ fusermount -u mountpoint


Dependencies
------------

To build and use cap-fs, please install the dependencies

 * fuse3: https://github.com/libfuse/libfuse
 * libglib2.0-dev
 * gthread-2.0-dev
 * python3-pytest
 * protobuf-dev
 * meson
 
Note, depending on your setup, libfuse may not be in your library paths. If so,
use LD_LIBRARY_PATH environment variable e.g.

    $ export LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/


Building CAP-FS
---------------

After the dependencies are ready, you can use Meson to build cap-fs. First,
create a temporary build directory (here in the cap-fs source directory) and 
run Meson where $source points ot the CAP-FS sources:

    $ md build; cd build
    $ meson $source
    
    

After Meson configured CAP-FS you can now build CAP-FS using Ninja:

    $ ninja
    $ sudo ninja install

