#!/bin/sh

mcs -out:game.dll -target:module game.cs
mcs -out:myclass.dll -addmodule:game.dll -target:module myclass.cs
mcs -out:test.dll -addmodule:game.dll,myclass.dll  test.cs