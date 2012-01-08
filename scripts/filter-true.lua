#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}

output.schema = {}
for i in pairs(input.schema) do
	output.schema = input.schema[i]
end
