dofile "scripts/matrix.lua"
dofile "scripts/gradient.lua"

function fit(X, Y, cost, grad)

	-- feature scaling

	local S = Matrix.zeros(3, X.cols)

	for j = 1, X.cols do
		S[2][j] = -math.huge
		S[3][j] = math.huge
	end

	for i = 1, X.rows do
		for j = 1, X.cols do
			S[1][j] = S[1][j] + X[i][j]
			S[2][j] = S[2][j] < X[i][j] and X[i][j] or S[2][j]
			S[3][j] = S[3][j] > X[i][j] and X[i][j] or S[3][j]
		end
	end

	for j = 1, X.cols do
		S[1][j] = S[1][j] / X.rows
	end

	for i = 1, X.rows do
		for j = 2, X.cols do
			X[i][j] = (X[i][j] - S[1][j]) / (S[2][j] - S[3][j])
		end
	end

	-- minimize cost function

	local theta = minimizeGrad(
		function(theta) return cost(theta, X, Y) end,
		function(theta) return grad(theta, X, Y) end,
		X.cols, S)

	-- reverse feature scaling

	for j = 2, X.cols do
		theta[j][1] = theta[j][1] / (S[2][j] - S[3][j])
		theta[1][1] = theta[1][1] - theta[j][1] * S[1][j]
	end

	return theta
end
