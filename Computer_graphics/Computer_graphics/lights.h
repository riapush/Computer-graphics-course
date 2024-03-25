#pragma once

#include <directxmath.h>
#include <stdint.h>
#include <functional>
#include <vector>
static const constexpr std::size_t maxLightNumber = 10;

struct Light
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
};

class Lights
{
public:
	bool Add(const Light& info);
	std::size_t GetNumber();
	std::vector<DirectX::XMFLOAT4> GetPositions();
	std::vector<DirectX::XMFLOAT4> GetColors();

private:
	std::vector<Light> lights;
};