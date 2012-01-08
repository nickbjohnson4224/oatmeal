#!/usr/bin/env luajit

dofile "scripts/table.lua"

input = read_table(io.stdin)

output = map_table(input, { "looplen1", "looplen2", "looplen3" },
	function (datum)
		local newdatum = {}
		newdatum.looplen1 = #datum.loop1
		newdatum.looplen2 = #datum.loop2
		newdatum.looplen3 = #datum.loop3
		return newdatum
	end)

write_table(io.stdout, output)
