#!/usr/bin/env luajit

dofile "scripts/matrix.lua"
dofile "scripts/gradient.lua"
dofile "scripts/fit.lua"

-- linear cost function

function linearCost(theta, X, Y)
	return (X * theta)
		:zip(Y, function(x, y) return (y - x) ^ 2 end)
		:sum() * (1 / X.rows)
end

function linearGrad(theta, X, Y)
	return (X:t() * ((X * theta) - Y)) * (2 / X.rows)
end
	
function linearPrediction(theta, x)
	return (x * theta)[1][1]
end

-- data input

dofile "table.lua"

input = read_table(io.stdin)

io.stderr:write("read schema:\n")
write_schema(io.stderr, input.schema)

io.stderr:write("predicting variable " .. input.schema[#input.schema] .. "\n")

if #arg == 1 then
	test_set_size = tonumber(arg[1])
else
	test_set_size = 100
end

X = Matrix.zeros(#input.data - test_set_size, #input.schema)
Y = Matrix.zeros(#input.data - test_set_size, 1)

TX = Matrix.zeros(test_set_size, #input.schema)
TY = Matrix.zeros(test_set_size, 1)

for index = 1, #input.data - TX.rows do
	for j = 1, #input.schema do
		X[index][j+1] = tonumber(input.data[index][input.schema[j]])
	end
	X[index][1] = 1
	Y[index][1] = tonumber(input.data[index][input.schema[#input.schema]])
end

for index = 1, TX.rows do
	for j = 1, #input.schema do
		TX[index][j+1] = tonumber(input.data[index + #input.data - TX.rows][input.schema[j]])
	end
	TX[index][1] = 1
	TY[index][1] = tonumber(input.data[index + #input.data - TX.rows][input.schema[#input.schema]])
end

theta = fit(X, Y, linearCost, linearGrad)
print("theta:")
theta:print()
print("")

print("test set cost:")
print(linearCost(theta, TX, TY))
