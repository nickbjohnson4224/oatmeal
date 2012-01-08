#!/usr/bin/env lua

dofile "scripts/table.lua"

records = tonumber(arg[1])

table = {}

table.schema = {}
for i = 1, #arg-2 do
	table.schema[i] = "x" .. tostring(i)
end
table.schema[#arg-1] = "y"

theta = {}
for i = 1, #arg-1 do
	theta[i] = tonumber(arg[i+1])
end

table.data = {}
math.randomseed(os.time())
for i = 1, records do
	table.data[i] = {}
	local y = theta[1]
	for index, column in ipairs(table.schema) do
		if column ~= "y" then
			local x = math.random() * 100 - 50
			table.data[i][column] = x
			y = y + theta[index+1] * x
		end
	end
	table.data[i].y = 1 / (1 + math.exp(-y)) > .5 and 1 or 0
end

write_table(io.stdout, table)
