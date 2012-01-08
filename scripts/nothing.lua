#!/usr/bin/env luajit

dofile "scripts/oatmeal.lua"

table = oatmeal.read_table(io.stdin)
oatmeal.write_table(io.stdout, table)
