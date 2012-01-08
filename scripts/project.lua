#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}
filter = {}

output.schema = {}
for i = 1, #arg do
	output.schema[i] = arg[i]
	filter[arg[i]] = true
end

output.data = {}
for index in pairs(input.data) do
	output.data[index] = {}

	for key in pairs(input.data[index]) do
		if filter[key] then
			output.data[index][key] = input.data[index][key]
		end
	end
end

write_table(io.stdout, output)
