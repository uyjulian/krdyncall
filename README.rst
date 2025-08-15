krdyncall
=========

This Kirikiri (2/Z) plugin provides a interface to the dyncall library.

Building
--------

After cloning submodules and placing ``ncbind`` and ``tp_stub`` in the
parent directory, a simple ``make`` will generate ``krdyncall.dll``.

How to use
----------

After ``Plugins.link("krdyncall.dll");`` is used, the additional
functions will be exposed.

License
-------

This project is licensed under the MIT license. Please read the
``LICENSE`` file for more information.
