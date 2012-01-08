#!/usr/bin/env luajit

dofile "scripts/table.lua"

if #arg == 2 then
	input1 = read_table(io.open(arg[1], "r"))
	input2 = read_table(io.open(arg[2], "r"))
elseif #arg == 1 then
	input1 = read_table(io.stdin)
	input2 = read_table(io.open(arg[1], "r"))
else
	error("insufficient arguments")
	os.exit()
end

output = {}

output.schema = {}
for index in pairs(input1.schema) do
	output.schema[#output.schema+1] = input1.schema[index]
end
for index in pairs(input2.schema) do
	output.schema[#output.schema+1] = input2.schema[index]
end

output.data = {}
for index in pairs(input1.data) do
	output.data[index] = {}

	for key in pairs(input1.data[index]) do
		output.data[index][key] = input1.data[index][key]
	end
	for key in pairs(input2.data[index]) do
		output.data[index][key] = input2.data[index][key]
	end
end

write_table(io.stdout, output)
