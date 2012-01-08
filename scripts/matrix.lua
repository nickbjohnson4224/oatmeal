-- Matrix class

Matrix = {}
Matrix.mt = {}

function Matrix.sum(m)
	local sum = 0

	for k, v in ipairs(m) do
		for k1, v1 in ipairs(v) do
			sum = sum + v1
		end
	end

	return sum
end

function Matrix.map(m, f)
	local m1 = {}
	
	for k, v in ipairs(m) do
		m1[k] = {}
		for k1, v1 in ipairs(v) do
			m1[k][k1] = f(v1)
		end
	end

	Matrix.init(m1)
	m1.rows = m.rows
	m1.cols = m.cols
	return m1
end

function Matrix.zip(m1, m2, f)
	local m3 = {}

	assert(m1.cols == m2.cols and m1.rows == m2.rows, "dimension mismatch")

	for i = 1, m1.rows do
		m3[i] = {}
		for j = 1, m1.cols do
			m3[i][j] = f(m1[i][j], m2[i][j])
		end
	end

	Matrix.init(m3)
	m3.rows = m1.rows
	m3.cols = m1.cols
	return m3
end

function Matrix.print(m)
	for k, v in ipairs(m) do
		print(unpack(v))
	end
end

function Matrix.t(m)
	return Matrix.generate(m.cols, m.rows, function(i,j) return m[j][i] end)
end

function Matrix.mul(a, b)

	if type(a) == "number" then
		return Matrix.generate(b.rows, b.cols, 
			function(i, j)
				return a * b[i][j]
			end)
	end

	if type(b) == "number" then
		return Matrix.generate(a.rows, a.cols,
			function(i, j)
				return a[i][j] * b
			end)
	end

	if (a.cols ~= b.rows) then
		print("dimension mismatch")
		print(a.cols, a.rows)
		print(b.cols, b.rows)
		error("dimension mismatch")
	end

	return Matrix.generate(a.rows, b.cols,
		function(i, j)
			local sum = 0
			for k = 1, a.cols do
				sum = sum + a[i][k] * b[k][j]
			end
			return sum
		end)
end

function Matrix.add(a, b)
	return Matrix.zip(a, b, function(x, y) return x + y end)
end

function Matrix.neg(m)
	return m:map(function(x) return -x end)
end

function Matrix.sub(a, b)
	return Matrix.zip(a, b, function(x, y) return x - y end)
end

function Matrix.less(a, b)
	return a:norm() < b:norm()
end

function Matrix.greater(a, b)
	return a:norm() > b:norm()
end

function Matrix.equal(a, b)
	return (a - b):norm() == 0
end

function Matrix.init(m)
	setmetatable(m, {
		__index = Matrix,
		__mul = Matrix.mul,
		__add = Matrix.add,
		__sub = Matrix.sub,
		__unm = Matrix.neg,
		__lt  = Matrix.less,
		__gt  = Matrix.greater,
		__eq  = Matrix.equal
		})
end

function Matrix.norm(m)
	return math.sqrt(m:map(function (x) return x * x end):sum())
end

function Matrix.dot(m, n)
	return (m:t() * n)[1][1]
end

function Matrix.row(m, i)
	return Matrix.new(m[i])
end

function Matrix.col(m, i)
	return Matrix.row(m:t(), i):t()
end

function Matrix.generate(r, c, f)
	local m = {}

	for i = 1, r do
		m[i] = {}
		for j = 1, c do
			m[i][j] = f(i, j)
		end
	end

	m.rows = r
	m.cols = c

	Matrix.init(m)
	return m
end

function Matrix.zeros(r, c)
	return Matrix.generate(r, c, function (i, j) return 0 end)
end

function Matrix.ones(r, c)
	return Matrix.generate(r, c, function (i, j) return 1 end)
end

function Matrix.rand(r, c)
	math.randomseed(os.time())
	return Matrix.generate(r, c, function (i, j) return math.random() end)
end

function Matrix.unit(r, n)
	return Matrix.generate(r, 1, function (i, j) if i == n then return 1 else return 0 end end)
end

function Matrix.eye(n)
	return Matrix.generate(n, n,
		function(i, j)
			if i == j then
				return 1
			else
				return 0
			end
		end)
end

function Matrix.new(...)
	local m = {}

	local i = 1
	for k, v in ipairs(arg) do

		print(v)
		if type(v) == "number" then
			m[i] = { v }
			m.cols = 1
		end
		if type(v) == "table" then
			local j = 1
			m[i] = {}
			for k1, v1 in ipairs(v) do
				m[i][j] = v1
				j = j + 1
			end
			m.cols = j - 1
		end

		i = i + 1
	end
	m.rows = i - 1

	print(m, m.cols, m.rows)
	Matrix.init(m)
	return m
end
