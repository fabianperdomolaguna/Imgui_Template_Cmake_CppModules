module;

#include <string>
#include <string_view>

export module beryl.core:layer;

export namespace beryl::core
{
    class Layer
    {
    protected:
        std::string m_layer_name;

    public:
        explicit Layer(std::string_view name) 
            : m_layer_name(name) {}

        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnRender() {}
        std::string_view GetName() const { return m_layer_name; }
    };
}