#include <SDL2/SDL_mixer.h>
#include<iostream>

using namespace std;

//save in mp3 only, wav and ogg didnt work
enum SoundChannel
{
    whatever = -1,
    BUTTONS_CHANNEL=0,
    GAME_EVENTS_CHANNEL1,
    GAME_EVENTS_CHANNEL2,
    NUMBER_OF_CHANNELS,
};

bool wantsound=true;
bool wantmusic=true;


class Music
{

public:
    Mix_Music* m_music;
    void load(string path)
    {
        m_music = Mix_LoadMUS(path.c_str());
        // Check load
        if( !m_music  )
        {
//            cout << "Failed to load music: " << Mix_GetError() << endl;
        }
    }
    void play()
    {
        int result=1;
        if(wantmusic) result = Mix_PlayMusic( m_music, -1 );

    if ( result != 0 ) {
//        cout << "Failed to play music: " << Mix_GetError() << endl;
    }
    }
    void pause()
    {
        Mix_PauseMusic();
    }

};
class  Sound
{
    Mix_Chunk* m_sound;
    SoundChannel m_channel;
public:

    void load(string path,SoundChannel channel)
    {
        m_sound = Mix_LoadWAV(path.c_str());
        m_channel = channel;
        // Check load
        if( !m_sound  )
        {
//            cout << "Failed to load sound: " << Mix_GetError() << endl;
        }
    }
    void play()
    {
        if(wantsound) Mix_PlayChannel( m_channel, m_sound, 0 );
    }

};

class MusicAndStuffMainClass
{


public:



    MusicAndStuffMainClass()
    {
        Mix_Init(MIX_INIT_OGG|MIX_INIT_MP3);
        int result = Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT,8, 1024 );

// Check load
        if ( result != 0 )
        {
//            cout << "Failed to open audio: " << Mix_GetError() << endl;
        }
    }
    void exit()
    {
        Mix_Quit();
    }
};
//int main()
//{
//    MusicAndStuffMainClass maw;
//    Music m;
//    m.load("menu.mp3");
//    m.play();
//}
