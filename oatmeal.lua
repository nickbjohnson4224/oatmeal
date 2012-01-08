-- schema/datum reading/writing

local function split(string, sep)
	local fields = {}
	local pattern = string.format("([^%s]+)", sep)
	string:gsub(pattern, function (c) fields[#fields+1] = c end)
	return fields
end

function read_schema(file)
	return split(file:read("*line"), "\t")
end

function read_datum(file, schema)
	local line = file:read("*line")
	local datum = {}

	if line == nil then return nil end

	for i,v in ipairs(split(line, "\t")) do
		datum[schema[i]] = v
	end

	return datum
end

function write_schema(file, schema)
	
	for i = 1, #schema - 1 do
		file:write(schema[i] .. "\t")
	end
	file:write(schema[#schema] .. "\n")
end

function write_datum(file, schema, datum)
	
	for i = 1, #schema - 1 do
		file:write(datum[schema[i]] .. "\t")
	end
	file:write(datum[schema[#schema]] .. "\n")
end

function read_table(file)
	local data = {}
	local schema = {}

	schema = read_schema(file)
	while true do
		local datum = read_datum(file, schema)
		if datum == nil then break end
		data[#data+1] = datum
	end

	return { data = data, schema = schema }
end

function write_table(file, table)
	
	write_schema(file, table.schema)
	for index, datum in ipairs(table.data) do
		write_datum(file, table.schema, datum)
	end
end

-- table manipulation

function filter_table(table, filter)
	local newtable = {}
	
	newtable.schema = {}
	for index in pairs(table.schema) do
		newtable.schema[index] = table.schema[index]
	end

	newtable.data = {}
	for index, datum in ipairs(table.data) do
		if filter(datum) then
			newtable.data[#newtable.data+1] = datum
		end
	end

	return newtable
end

function map_table(table, newschema, func)
	local newtable = {}

	newtable.schema = newschema

	newtable.data = {}
	for index, datum in ipairs(table.data) do
		newtable.data[#newtable.data+1] = func(datum)
	end

	return newtable
end
