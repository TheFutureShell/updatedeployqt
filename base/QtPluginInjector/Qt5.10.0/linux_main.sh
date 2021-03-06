#!/usr/bin/env bash
wget -q "https://download.qt.io/archive/qt/5.10/5.10.0/submodules/qtbase-everywhere-src-5.10.0.tar.xz"
tar -xvf qtbase-everywhere-src-5.10.0.tar.xz
cp --force qxcbmain.cpp qtbase-everywhere-src-5.10.0/src/plugins/platforms/xcb/qxcbmain.cpp
echo "include(../../../../../../QtPluginInjector.pri)" >> qtbase-everywhere-src-5.10.0/src/plugins/platforms/xcb/xcb-plugin.pro 
cd qtbase-everywhere-src-5.10.0
./configure -shared -release -optimize-size -silent -opensource -confirm-license -opengl -nomake examples -qt-xcb -qt-xkbcommon -sm -qt-libpng -no-libjpeg -no-icu -qt-zlib -qt-pcre -gtk -system-freetype -qt-harfbuzz
make -j$(nproc)
make install
rm -rf qtbase-everywhere-src-5.10.0* # cleanup
