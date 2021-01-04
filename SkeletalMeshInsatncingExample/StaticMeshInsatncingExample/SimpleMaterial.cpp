#include "SimpleMaterial.h"

void SimpleMaterial::Destory()
{
	if (m_diffuseTex != nullptr)
	{
		m_diffuseTex->Destroy();
	}
}