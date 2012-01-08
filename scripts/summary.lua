#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = {}

output.schema = {}
for i in pairs(input.schema) do
	output.schema = input.schema[i]
end

count = #input.data

output.data = {}
output.data[1] = {}
for i in pairs(input.data) do

	for key in pairs(input.data[i]) do
		if output.data[1][key] then
			output.data[1][key] = output.data[1][key] + input.data[i][key]
		end
	end

end

for key in pairs(output.data[1]) do
	output.data[1][key] = output.data[1][key] / count
end

write_table(io.stdout, table)
