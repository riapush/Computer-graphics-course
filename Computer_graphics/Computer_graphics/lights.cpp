#include "lights.h"
#include <algorithm>

bool Lights::Add(const Light& info)
{
    if (lights.size() >= maxLightNumber)
        return false;
    lights.push_back(info);
    return true;
}

std::size_t Lights::GetNumber()
{
    return lights.size();
}

std::vector<DirectX::XMFLOAT4> Lights::GetPositions()
{
    std::vector<DirectX::XMFLOAT4> res;
    res.reserve(lights.size());
    std::for_each(
        lights.begin(),
        lights.end(),
        [&res](auto& lightInfo) {res.push_back(lightInfo.position); });
    return res;
}

std::vector<DirectX::XMFLOAT4> Lights::GetColors()
{
    std::vector<DirectX::XMFLOAT4> res;
    res.reserve(lights.size());
    std::for_each(
        lights.begin(),
        lights.end(),
        [&res](auto& lightInfo) {res.push_back(lightInfo.color); });
    return res;
}