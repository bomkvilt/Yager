@echo off
set dir=./build/windows

if not exist "%dir%" mkdir "%dir%"

pushd "%dir%"
cmake -Wno-dev -j8 ../..
popd
