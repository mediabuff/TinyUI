#include "stdafx.h"
#include "GameScene.h"


GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

BOOL GameScene::Initialize(DX11& dx11)
{
	m_captureTask.Reset(new DX11CaptureTask(&dx11, *this));
	return m_captureTask != NULL;
}

void GameScene::SetConfig(const TinyString& className, const TinyString& exeName, const TinyString& dllName)
{
	m_captureTask->SetConfig(className, exeName, dllName);
}

BOOL GameScene::BeginScene()
{
	return m_captureTask->Submit();
}

void GameScene::EndScene()
{
	m_captureTask->Close();
}

BOOL GameScene::Render(const DX11& dx11)
{
	return DX11Image::Render(dx11);
}

LPCSTR GameScene::GetClassName()
{
	return TEXT("GameScene");
}