module;

#include <string>

export module Layer;

export class Layer
{
public:
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnRender() {}
    virtual std::string GetName() const = 0;
};