#pragma once
#include "DXFramework.h"
#include "DX11.h"
#include "DX11Texture2D.h"

namespace DXFramework
{
	/// <summary>
	/// ͸����ɫ��
	/// </summary>
	class DX11TransparentShader
	{
		DISALLOW_COPY_AND_ASSIGN(DX11TransparentShader)
	public:
		DX11TransparentShader();
		virtual ~DX11TransparentShader();
		BOOL Initialize(DX11& dx11, const CHAR* vsFile, const CHAR* psFile);
		void Render(DX11& dx11, INT indexCount, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, DX11Texture2D* pTexture, FLOAT blend);
	protected:
		TinyComPtr<ID3D11VertexShader>	m_vertexShader;
		TinyComPtr<ID3D11PixelShader>	m_pixelShader;
		TinyComPtr<ID3D11InputLayout>	m_layout;
		TinyComPtr<ID3D11SamplerState>	m_sampleState;
		TinyComPtr<ID3D11Buffer>		m_matrixBuffer;
		TinyComPtr<ID3D11Buffer>		m_transparentBuffer;
	};
}

