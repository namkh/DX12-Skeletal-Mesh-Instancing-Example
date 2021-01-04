#pragma once

#include "SimpleTexture.h"
#include "SimpleShader.h"
#include "Utils.h"

enum class ExampleMaterialType
{
	EXAMPLE_MAT_TYPE_INVALID,
	EXAMPLE_MAT_INSTANCING1,
	EXAMPLE_MAT_INSTANCING2,
	EXAMPLE_MAT_INSTANCING3,
};

//�����Ͱ� �����Ƿ� ������ �ۼ�
class SimpleMaterial : public UniqueIdentifier
{
public:
	void Destory();

	XMFLOAT4 m_ambient;
	SimpleTexture2D* m_diffuseTex = nullptr;
	float m_uvScale = 1.0f;
	ExampleMaterialType m_exampleMatType = ExampleMaterialType::EXAMPLE_MAT_TYPE_INVALID;
};