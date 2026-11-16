#include "TextScene.h"

TextScene::TextScene(Vector2 origin, const char *bgHexCode, const char *backgroundPath,
                     const char *titlePath, const char *promptPath, Color backgroundTint,
                     const char *musicPath, bool loopMusic)
    : Scene{ origin, bgHexCode }, mBackgroundPath{ backgroundPath },
      mTitlePath{ titlePath }, mPromptPath{ promptPath }, mBackgroundTint{ backgroundTint },
      mMusicPath{ musicPath }, mLoopMusic{ loopMusic }
{}

TextScene::~TextScene() { shutdown(); }

void TextScene::initialise()
{
    mGameState.nextSceneID = NO_SCENE;

    mGameState.player = nullptr;
    mGameState.map    = nullptr;

    mBackgroundTexture = LoadTexture(mBackgroundPath);
    mTitleTexture      = LoadTexture(mTitlePath);
    mPromptTexture     = LoadTexture(mPromptPath);

    // screens have diff music
    if (mMusicPath != nullptr)
    {
        mGameState.bgm = LoadMusicStream(mMusicPath);
        mGameState.bgm.looping = mLoopMusic;

        SetMusicVolume(mGameState.bgm, MUSIC_VOLUME);
        PlayMusicStream(mGameState.bgm);
    }

    mBlinkTimer = 0.0f;
}

void TextScene::update(float deltaTime)
{
    if (mMusicPath != nullptr) UpdateMusicStream(mGameState.bgm);

    mBlinkTimer += deltaTime;
    if (mBlinkTimer >= BLINK_PERIOD) mBlinkTimer = 0.0f;
}

void TextScene::drawCentred(Texture2D texture, float centreY, float scale)
{
    float width  = texture.width  * scale;
    float height = texture.height * scale;

    DrawTexturePro(
        texture,
        { 0.0f, 0.0f,
          static_cast<float>(texture.width),
          static_cast<float>(texture.height) },
        { mOrigin.x, centreY, width, height },
        { width / 2.0f, height / 2.0f },
        0.0f, WHITE
    );
}

void TextScene::drawBackground()
{
    float windowWidth  = mOrigin.x * 2.0f;
    float windowHeight = mOrigin.y * 2.0f;

    float scale = ceilf(fmaxf(windowWidth  / mBackgroundTexture.width,
                              windowHeight / mBackgroundTexture.height));

    float sourceWidth  = windowWidth  / scale;
    float sourceHeight = windowHeight / scale;

    DrawTexturePro(
        mBackgroundTexture,
        { (mBackgroundTexture.width  - sourceWidth)  / 2.0f,
          (mBackgroundTexture.height - sourceHeight) / 2.0f,
          sourceWidth, sourceHeight },
        { 0.0f, 0.0f, windowWidth, windowHeight },
        { 0.0f, 0.0f }, 0.0f, mBackgroundTint
    );
}

void TextScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    drawBackground();

    drawCentred(mTitleTexture, mOrigin.y - TITLE_OFFSET, TEXT_SCALE);

    if (mBlinkTimer < BLINK_PERIOD / 2.0f)
        drawCentred(mPromptTexture, mOrigin.y + PROMPT_OFFSET, TEXT_SCALE);
}

void TextScene::shutdown()
{
    UnloadTexture(mBackgroundTexture);
    mBackgroundTexture = {};
    UnloadTexture(mTitleTexture);
    mTitleTexture = {};
    UnloadTexture(mPromptTexture);
    mPromptTexture = {};

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
}
