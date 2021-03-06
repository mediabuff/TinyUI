#pragma once
#include "DXFramework.h"
#include <functional>

namespace DXFramework
{
	enum WIC_LOADER_FLAGS
	{
		WIC_LOADER_DEFAULT = 0,
		WIC_LOADER_FORCE_SRGB = 0x1,
		WIC_LOADER_IGNORE_SRGB = 0x2,
	};

	IWICImagingFactory* GetWIC();

	//HRESULT __cdecl CreateD2DBitmapFromFile(LPCWSTR fileName, 
	//	ID2D1RenderTarget* pTarget,
	//	ID2D1Bitmap** pBitmap);

	//HRESULT __cdecl CreateD2DBitmapFromMemory(const BYTE* wicData,
	//	size_t wicDataSize,
	//	ID2D1RenderTarget* pTarget,
	//	ID2D1Bitmap** pBitmap);

	HRESULT __cdecl CreateWICBitmapFromFile(LPCWSTR fileName,
		IWICBitmap** pBitmap);

	HRESULT __cdecl CreateWICBitmapFromMemory(const BYTE* wicData,
		size_t wicDataSize,
		IWICBitmap** pBitmap);

	HRESULT __cdecl CreateWICTextureFromMemory(
		ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromFile(
		ID3D11Device* d3dDevice,
		const WCHAR* szFileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromMemory(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const BYTE* wicData,
		size_t wicDataSize,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromFile(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const WCHAR* szFileName,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView,
		size_t maxsize = 0);

	HRESULT __cdecl CreateWICTextureFromMemoryEx(
		ID3D11Device* d3dDevice,
		const BYTE* wicData,
		size_t wicDataSize,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromFileEx(
		ID3D11Device* d3dDevice,
		const WCHAR* szFileName,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromMemoryEx(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const BYTE* wicData,
		size_t wicDataSize,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);

	HRESULT __cdecl CreateWICTextureFromFileEx(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dContext,
		const WCHAR* szFileName,
		size_t maxsize,
		D3D11_USAGE usage,
		UINT bindFlags,
		UINT cpuAccessFlags,
		UINT miscFlags,
		UINT loadFlags,
		ID3D11Resource** texture,
		ID3D11ShaderResourceView** textureView);


	HRESULT __cdecl SaveWICTextureToFile(
		ID3D11DeviceContext* pContext,
		ID3D11Resource* pSource,
		REFGUID guidContainerFormat,
		const WCHAR* fileName,
		const GUID* targetFormat = nullptr,
		std::function<void __cdecl(IPropertyBag2*)> setCustomProps = nullptr);
}


