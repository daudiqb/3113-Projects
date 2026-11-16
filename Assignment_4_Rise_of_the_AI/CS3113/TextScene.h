#ifndef TEXTSCENE_H
#define TEXTSCENE_H

#include "Scene.h"

class TextScene : public Scene
{
private:
    const char *mBackgroundPath;
    const char *mTitlePath;
    const char *mPromptPath;
    Color mBackgroundTint; 
    const char *mMusicPath;   
    bool mLoopMusic;

    Texture2D mBackgroundTexture = {};
    Texture2D mTitleTexture = {};
    Texture2D mPromptTexture = {};

    float mBlinkTimer = 0.0f;

    void drawCentred(Texture2D texture, float centreY, float scale);
    void drawBackground();

public:
    static constexpr float TEXT_SCALE    = 3.0f,
                           TITLE_OFFSET  = 40.0f,
                           PROMPT_OFFSET = 50.0f,
                           BLINK_PERIOD  = 1.0f,
                           MUSIC_VOLUME  = 0.2f;

    TextScene(Vector2 origin, const char *bgHexCode, const char *backgroundPath,
              const char *titlePath, const char *promptPath, Color backgroundTint = WHITE,
              const char *musicPath = nullptr, bool loopMusic = true);
    ~TextScene();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // TEXTSCENE_H
