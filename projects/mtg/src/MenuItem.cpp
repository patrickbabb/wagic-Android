#include "PrecompiledHeader.h"

#include "MenuItem.h"
#include "Translate.h"
#include "WResourceManager.h"
#include "ModRules.h"
#include "JGE.h"

MenuItem::MenuItem(int id, WFont *font, string text, float x, float y, JQuad * _off, JQuad * _on, const char * particle,
                JQuad * particleTex, bool hasFocus) :
    JGuiObject(id), mFont(font), mX(x), mY(y)
{
    mText = _(text);
    updatedSinceLastRender = 1;
    mParticleSys = NULL;
    hgeParticleSystemInfo * psi = WResourceManager::Instance()->RetrievePSI(particle, particleTex);
    if (psi)
    {
        // Scale particle properties for current screen resolution
        psi->fSizeStart        *= SCALE;
        psi->fSizeEnd          *= SCALE;
        psi->fSizeVar          *= SCALE;
        psi->fSpeedMin         *= SCALE;
        psi->fSpeedMax         *= SCALE;
        psi->fGravityMin       *= SCALE;
        psi->fGravityMax       *= SCALE;
        psi->fRadialAccelMin   *= SCALE;
        psi->fRadialAccelMax   *= SCALE;
        psi->fTangentialAccelMin *= SCALE;
        psi->fTangentialAccelMax *= SCALE;
        mParticleSys = NEW hgeParticleSystem(psi);
        mParticleSys->MoveTo(mX, mY);
    }

    mHasFocus = hasFocus;
    lastDt = 0.001f;
    mScale = 1.0f;
    mTargetScale = 1.0f;

    onQuad = _on;
    offQuad = _off;

    if (hasFocus)
        Entering();
}

void MenuItem::Render()
{
    JRenderer * renderer = JRenderer::GetInstance();

    if (mHasFocus)
    {
        PIXEL_TYPE start = ARGB(46,255,255,200);
        if (mParticleSys)
            start = mParticleSys->info.colColorStart.GetHWColor();
        PIXEL_TYPE colors[] = { ARGB(0,0,0,0), start, ARGB(0,0,0,0), start, };
        renderer->FillRect(SCREEN_WIDTH - 400 * SCALE, 0, 400 * SCALE, SCREEN_HEIGHT, colors);
        // set additive blending
        renderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE);
        mParticleSys->Render();
        // set normal blending
        renderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        mFont->SetColor(ARGB(255,255,255,255));
        offQuad->SetColor(ARGB(60,255,255,255));
        float iconScale = SCREEN_HEIGHT / (36.0f * 1.8f);
        renderer->RenderQuad(offQuad, SCREEN_WIDTH - 20 * SCALE, SCREEN_HEIGHT / 2, 0, iconScale, iconScale);
        offQuad->SetColor(ARGB(255,255,255,255));
        onQuad->SetColor(ARGB(255,255,255,255));
        mFont->SetScale(DEFAULT_MENU_FONT_SCALE);
        mFont->DrawString(mText.c_str(), SCREEN_WIDTH / 2, 3 * SCREEN_HEIGHT / 4, JGETEXT_CENTER);
        renderer->RenderQuad(onQuad, mX, mY, 0, mScale * SCALE, mScale * SCALE);
    }
    else
    {
        renderer->RenderQuad(offQuad, mX, mY, 0, mScale * SCALE, mScale * SCALE);
    }
    updatedSinceLastRender = 0;
}

void MenuItem::Update(float dt)
{
    updatedSinceLastRender = 1;
    lastDt = dt;
    if (mScale < mTargetScale)
    {
        mScale += 8.0f * dt;
        if (mScale > mTargetScale)
            mScale = mTargetScale;
    }
    else if (mScale > mTargetScale)
    {
        mScale -= 8.0f * dt;
        if (mScale < mTargetScale)
            mScale = mTargetScale;
    }

    if (mParticleSys)
        mParticleSys->Update(dt);
}

void MenuItem::Entering()
{
    if (mParticleSys)
        mParticleSys->Fire();
    mHasFocus = true;
    mTargetScale = 1.2f;
}

bool MenuItem::Leaving(JButton)
{
    if (mParticleSys)
        mParticleSys->Stop(true);
    mHasFocus = false;
    mTargetScale = 1.0f;
    return true;
}

bool MenuItem::ButtonPressed()
{
    return true;
}

MenuItem::~MenuItem()
{
    SAFE_DELETE(mParticleSys);
}

ostream& MenuItem::toString(ostream& out) const
{
    return out << "MenuItem ::: mHasFocus : " << mHasFocus << " ; mFont : " << mFont << " ; mText : " << mText << " ; mX,mY : "
                    << mX << "," << mY << " ; updatedSinceLastRender : " << updatedSinceLastRender << " ; lastDt : " << lastDt
                    << " ; mScale : " << mScale << " ; mTargetScale : " << mTargetScale << " ; onQuad : " << onQuad
                    << " ; offQuad : " << offQuad << " ; mParticleSys : " << mParticleSys;
}

OtherMenuItem::OtherMenuItem(int id, WFont *font, string text, float x, float y, JQuad * _off, JQuad * _on, JButton _key, bool hasFocus) :
  MenuItem(id, font, text, x, y, _off, _on, "", WResourceManager::Instance()->GetQuad("particles").get(), hasFocus), mKey(_key), mTimeIndex(0)
{

}

OtherMenuItem::~OtherMenuItem()
{

}

void OtherMenuItem::Render()
{
    int alpha = 255;
    if (GetId() == MENUITEM_TROPHIES && options.newAward())
        alpha = (int) (sin(mTimeIndex) * 255);

    float olds = mFont->GetScale();
    float xPos = SCREEN_WIDTH - 100 * SCALE;
    float xTextPos = xPos + 4 * SCALE;
    float yPos = SCREEN_HEIGHT_F - 20.f * SCALE;
    int textAlign = JGETEXT_LEFT;

    switch(mKey)
    {
        case JGE_BTN_PREV:
            xPos = 5;
            xTextPos = xPos + 10 * SCALE;
            textAlign = JGETEXT_LEFT;
            break;
        default:
            break;
    }

    mFont->SetScale(1.0f);
    mFont->SetScale((60.0f * SCALE) / mFont->GetStringWidth(mText.c_str()));

    float btnW = mFont->GetStringWidth(mText.c_str());
    float btnH = mFont->GetHeight();
    float pad  = 3.0f * SCALE;
    float vpad = 1.0f * SCALE;

    JRenderer::GetInstance()->FillRoundRect(xPos+1, yPos + vpad * 0.6f, btnW - pad, btnH - vpad, 5 * SCALE, ARGB(abs(alpha), 5, 5, 5));
    if(!mHasFocus)
    {
        mFont->SetColor(ARGB(abs(alpha),255,255,255));
        JRenderer::GetInstance()->FillRoundRect(xPos, yPos + vpad * 0.5f, btnW - pad, btnH - vpad, 5 * SCALE, ARGB(abs(alpha), 140, 23, 23));
    }
    else
    {
        mFont->SetColor(ARGB(abs(alpha),5,5,5));
        JRenderer::GetInstance()->FillRoundRect(xPos, yPos + vpad * 0.5f, btnW - pad, btnH - vpad, 5 * SCALE, ARGB(abs(alpha), 140, 140, 140));
    }
    JRenderer::GetInstance()->DrawRoundRect(xPos, yPos + vpad * 0.5f, btnW - pad, btnH - vpad, 5 * SCALE, ARGB(abs(alpha-20), 5, 5, 5));
    mFont->DrawString(mText, xTextPos, yPos + vpad * 0.9f, textAlign);
    mFont->SetScale(olds);
}

void OtherMenuItem::Update(float dt)
{
  MenuItem::Update(dt);
  mTimeIndex += 2*dt;
}
