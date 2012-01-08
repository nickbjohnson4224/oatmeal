#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

math.randomseed(os.time())
for i = #input.data, 1, -1 do
	j = math.random(i)
	input.data[i], input.data[j] = input.data[j], input.data[i]
end

write_table(io.stdout, input)
