#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

struct GameState
{
    Entity *player;
    Map    *map;

    Texture2D background;

    Music bgm;
    Sound jumpSound;
    Sound keySound;
        
    bool playerHit;  // for enemy touching player

    SceneID nextSceneID;
    bool playerDied;
    bool hasKey;

    int  bossHealth; // only the boss room uses this, 0 everywhere else
};

class Scene
{
protected:
    GameState   mGameState;
    Vector2     mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState   getState()           const { return mGameState;        }
    Vector2     getOrigin()          const { return mOrigin;           }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
    
    void clearPlayerHit() { mGameState.playerHit = false; }
};

#endif // SCENE_H
