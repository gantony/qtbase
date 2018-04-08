# qtbase
Qt Base (Core, Gui, Widgets, Network, ...)

This is a fork of the **wip/webassembly** branch that compiles qtbase to WebAssembly using Emscripten in order to allow full Qt applications to run in a browser.

The objective of this repository is to compile qtbase to asm.js using Emscripten in order to expose parts or Qt applications to JavaScript applications.
While the original repository provides support to QML-based UI, this forked version has a reduces scope that does not aim to support any UI mechanism. Thos objective is to compile Qt-base logic/processing/models that can be used with an existing UI built around web technologies (JavaScript and the DOM). Examples of such "modules" will be provided in a separate repository.

## How to build?

* Install the latest Emscripten SDK and source your environment to use it
* Shadow build:
  * mkdir build-qtbase-asmjs
  * cd build-qtbase-asmjs
  * ../qtbase/configure -xplatform emscripten -confirm-license -opensource -developer-build -release -static -no-thread -nomake tests -nomake examples -no-dbus -no-headersclean -no-feature-networkinterface -system-libpng -no-ssl -no-warnings-are-errors -no-opengl -no-openssl -system-zlib -no-gif -qt-zlib -qt-libpng -qt-libjpeg -no-accessibility -prefix /home/antony/dev/build-qt-webassembly/install -no-sql-psql -no-sql-mysql -no-sql-odbc -no-sql-tds -no-sql-oci -no-sql-db2 -no-sql-sqlite -no-sql-sqlite2 -no-sql-ibase -no-opengl
  * make
  * make install
  
 ## How to use?
 
 * export PATH=/home/path/to/put/build-qtbase-asmjs/install/bin/:$PATH
 * Build a smaple program 
 
 
