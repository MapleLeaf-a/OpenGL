#include "Component.h"
#include "GameObject.h"

Transform* Component::GetTransform() const
{
    return m_Owner ? &m_Owner->GetTransform() : nullptr; 
}