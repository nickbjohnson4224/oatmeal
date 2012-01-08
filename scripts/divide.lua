#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}

output.schema = {}
for i in pairs(input.schema) do
	if input.schema[i] ~= arg[1] then
		output.schema[i] = input.schema[i]
	end
end

output.data = {}
for i in pairs(input.data) do
	local d = tonumber(input.data[i][arg[1]])
	output.data[i] = {}
	for k in pairs(input.data[i]) do
		if k ~= arg[1] then
			output.data[i][k] = tonumber(input.data[i][k]) / d
		end
	end
end

write_table(io.stdout, output)
