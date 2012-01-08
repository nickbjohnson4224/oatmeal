#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}
oldname = arg[1]
newname = arg[2]

output.schema = {}
for index in pairs(input.schema) do
	if input.schema[index] == oldname then
		output.schema[index] = newname
	else
		output.schema[index] = input.schema[index]
	end
end

output.data = {}
for index in pairs(input.data) do
	output.data[index] = {}

	for key in pairs(input.data[index]) do
		if key == oldname then
			output.data[index][newname] = input.data[index][oldname]
		else
			output.data[index][key] = input.data[index][key]
		end
	end
end

write_table(io.stdout, output)
