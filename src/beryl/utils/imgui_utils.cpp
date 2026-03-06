module;

#include <cstdint>

#include <imgui.h>

export module beryl.utils.imgui;

export namespace beryl::utils::imgui
{
    [[nodiscard]] inline ImTextureID GetImID(uint64_t id) 
    {
        return static_cast<ImTextureID>(static_cast<uintptr_t>(id));
    }
}