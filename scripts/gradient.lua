-- Gradient Descent Implementation

function gradientDescent(grad, init, alpha, epsilon)
	local a = init
	local b = init
	local i = 0

	a = b
	b = a - (grad(a) * alpha)

	while (b - a):norm() > epsilon do
		a = b
		b = a - (grad(a) * alpha)

		if (i % 10000 == 0) then
			print("sample: ")
			b:t():print()
			print("grad: ")
			grad(a):print()
			print("")
		end

		i = i + 1
		b:t():print()
	end

	return b
end

function gradientDescentAuto(func, grad, init, epsilon, scale)
	local b = init
	local i = 0
	local alpha = 1
	local g = grad(b)

	while (func(b) > func(b - alpha * g)) do
		alpha = alpha * 3 / 2
	end

	while (func(b) < func(b - alpha * 3 * g)) do
		alpha = alpha * 2 / 3
	end

	alpha = alpha / 50
	print("initial alpha: " .. alpha)

	local g = grad(b)

	b = b - (g * alpha)

	while g:norm() > epsilon do
		g = grad(b)

		if (i % 10000 == 0) then
			alpha = alpha * (1 + math.random())
			if func(b) < func(b - alpha * g) then
				alpha = alpha * .5
			end
		end

		b = b - alpha * g
		if b[1][1] ~= b[1][1] then
			return b, i
		end

		if (i % 2000 == 0) then
			print("alpha: " .. alpha)
			print("sample: ")
	
			local c = b:map(function(x) return 0 end)
			c[1][1] = b[1][1]
			for j = 2, b.rows do
				c[j][1] = (b[j][1] - scale[1][j]) / (scale[2][j] - scale[3][j])
				c[1][1] = c[1][1] - scale[1][j] * b[j][1]
			end
			c:t():print()

			print("cost: ")
			print(func(b))
			print("grad: " .. g:norm())
			g:print()
			print("")
		end
		
		i = i + 1
	end

	return b, i
end

function minimizeGrad(func, grad, dim, scale)
	return gradientDescentAuto(func, grad, Matrix.zeros(dim, 1), 1e-6, scale)
end
