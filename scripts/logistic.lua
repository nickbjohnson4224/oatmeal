#!/usr/bin/env luajit

dofile "scripts/matrix.lua"
dofile "scripts/gradient.lua"
dofile "scripts/fit.lua"

-- logistic cost function

function sigmoid(x)
	return 1 / (1 + math.exp(-x))
end

lambda = 0.001

function logisticCost(theta, X, Y)
	return (X * theta)
		:map(sigmoid)
		:zip(Y, function(x, y) return y > .5 and math.log(x) or math.log(1-x) end)
		:map(function (x) if x ~= x or x == 1/0 then return 0 else return x end end)
		:sum() * (-1 / X.rows)

		+

		(theta:map(function(x) return x ^ 2 end):sum()) * (lambda / theta.rows)
end

function logisticGrad(theta, X, Y)
	return (1 / X.rows) * (X:t() * ((X * theta):map(sigmoid) - Y))
		+ (theta * (lambda * 2 / theta.rows))
end

function logisticPrediction(theta, x)
	return (x * theta):map(sigmoid):sum() > .5 and 1 or 0
end

-- data input

dofile "scripts/table.lua"

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

theta = fit(X, Y, logisticCost, logisticGrad)
print("theta:")
for i = 1, theta.rows do
	if i ~= 1 then 
		print(input.schema[i - 1], theta[i][1])
	else
		print("bias", theta[i][1])
	end
end
print("")

successes = 0.0
false_pos = 0.0
false_neg = 0.0
true_pos = 0.0
true_neg = 0.0
for i = 1, TX.rows do
	local test = Matrix.ones(1, TX.cols)
	for j = 1, X.cols do
		test[1][j] = TX[i][j]
	end
	if logisticPrediction(theta, test) == TY[i][1] then
		successes = successes + 1

		if TY[i][1] == 1 then
			true_pos = true_pos + 1
		else
			true_neg = true_neg + 1
		end
	else
		if TY[i][1] == 1 then
			false_neg = false_neg + 1
		else
			false_pos = false_pos + 1
		end
	end
end

print("false positives: ", false_pos)
print("true positives:  ", true_pos)
print("false negatives: ", false_neg)
print("true negatives:  ", true_neg)
print("success rate:    ", successes / TX.rows * 100 .. "%")
print("test set cost:   ", logisticCost(theta, TX, TY))
