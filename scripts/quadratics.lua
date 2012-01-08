#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}

output.schema = {}
for i = 1, #input.schema do
	for j = i, #input.schema do
		output.schema[(i-1) * (#input.schema-i+1) + j] = input.schema[i] .. "*" .. input.schema[j]
	end
end

output.data = {}
for index, datum in ipairs(input.data) do
	output.data[index] = {}
	for i, x in ipairs(input.schema) do
		for j, y in ipairs(input.schema) do
			output.data[index][output.schema[(i-1) * (#input.schema-i+1) + j]] = datum[x] * datum[y]
		end
	end
end

write_table(io.stdout, output)
