#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL_image.h>
#include<SDL_ttf.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "loader.cpp"

#include "PZUMA.hpp"
#include "menuclasses.hpp"


#define CLRFRMT(color) ClrFrmt::R(color), ClrFrmt::G(color), ClrFrmt::B(color), ClrFrmt::A(color)
#define b_iter std::vector<Button>::iterator

#define BACKGROUND_COLOR 0x000000FF
#define BUTTON_DEFAULT_COLOR 0x00FFFFFF

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720



#define TEXT_COLOR_EVERY 255,255,255,255
#define TEXT_FONT_EVERY "pixel.ttf"
#define PASSWORD_FONT "password.ttf"
#define FONT_DIR "./fonts/"
#define TEXT_LINE_SPACING 60
#define TEXT_MARGIN_BOX 5

#define TEXTURE_RELATIVE_LOCATION "./textures/"   //DIDNT work


enum listofmusic
{
    MAINMENU,

};
enum listofsound
{
    CLICK,

};





using namespace std;


Game_data data;
                        ////////////////////////////////////////////////////////////////////////////here are some important globals
string g_username;
string g_password;


struct ClrFrmt
{
    static Uint8 R(Uint32 value)
    {
        return value>>24;
    }
    static Uint8 G(Uint32 value)
    {
        return value>>16;
    }
    static Uint8 B(Uint32 value)
    {
        return value>>8;
    }
    static Uint8 A(Uint32 value)
    {
        return value;
    }
};



//things are about to get very dependent so we forward declare some useful functions:
void createButtonsforSettings();
void createButtonsformainmenu();
void plainMenuLoop();
void textInputloop();
void betterText(SDL_Renderer*,string,int,int,int,int,int,int,int,string);

Menu *g_ACTIVEMENU;
Menu g_mainmenu;

Menu g_settingsmenu;
Menu g_leaderboard;
Menu g_finalizeGame;
Menu g_signIn;
Menu g_helpmenu;

TextBox g_leadertextnames{WINDOW_WIDTH/3+TEXT_MARGIN_BOX,WINDOW_HEIGHT/3+TEXT_MARGIN_BOX,WINDOW_WIDTH/3,WINDOW_HEIGHT/3};   //text to display when leaderboard is called
TextBox g_leadertextscores{WINDOW_WIDTH*2/3-2*TEXT_MARGIN_BOX,WINDOW_HEIGHT/3+TEXT_MARGIN_BOX,WINDOW_WIDTH/3,WINDOW_HEIGHT/3};
TextBox g_map_name(WINDOW_WIDTH/8+3*TEXT_LINE_SPACING,WINDOW_HEIGHT/8,WINDOW_WIDTH/5,BUTTON_HEIGHT);
TextBox g_mode_name(WINDOW_WIDTH/8+3*TEXT_LINE_SPACING,WINDOW_HEIGHT/8+TEXT_LINE_SPACING*2,WINDOW_WIDTH/5,BUTTON_HEIGHT);
TextBox g_difficulty_text(WINDOW_WIDTH/8+3*TEXT_LINE_SPACING,WINDOW_HEIGHT/8+4*TEXT_LINE_SPACING,WINDOW_WIDTH/5,BUTTON_HEIGHT);
TextBox g_theme_text(WINDOW_WIDTH/8+3*TEXT_LINE_SPACING,WINDOW_HEIGHT/8+TEXT_LINE_SPACING*6.3,WINDOW_WIDTH/5,BUTTON_HEIGHT);

TextBox g_score_text(600,365,WINDOW_WIDTH/5,BUTTON_HEIGHT);
TextBox g_power_text(600,347+2*TEXT_LINE_SPACING,WINDOW_WIDTH/5,BUTTON_HEIGHT);
int g_map_index =0;
int g_mode_index =0;
int g_leaderboard_start_index =0;
int g_difficulty_index =0;

vector<string> maps ={"1","2","random"}; ///////////////////////////////////////////////////delete me in the last minutes
vector<string> modes ={"normal","timed"};
vector<string> diffs = {"beginner","easy","normal","hard","veteran"}; ///////////////////////////////////////////////////delete me in the last minutes
vector<string> themes = {"green","red","blue","dark"};

void failure_message()
{
    SDL_Texture* failuremessage;
    SDL_Rect messagelocation{600,600,50,50};
    failuremessage=IMG_LoadTexture(gRenderer,"./textures/fail.png");
//    SDL_SetTextureBlendMode()
    SDL_RenderCopy(gRenderer,failuremessage,NULL,&messagelocation);
//    cerr<<"failed";
}
bool loopFlag = true;
void GODDAMN_mainloop_mainmenu()
{
    SDL_Event * event = new SDL_Event();
    Button * currentButton;  //mouse is hovering on this
    Button * activeButton;   //mouse has clicked on this
    string inputText;
    bool renderTextNew;

    while(loopFlag)
    {
        renderTextNew = false;
        if (SDL_PollEvent(event))
        {
            if (event->type == SDL_KEYDOWN)
                if (event->key.keysym.sym == SDLK_ESCAPE)
                    loopFlag = false;
            if (event->type == SDL_QUIT )
                loopFlag = false;
            if (event->type == SDL_MOUSEBUTTONDOWN)
            {
                if (event->button.button == SDL_BUTTON_LEFT)
                {
                    if (currentButton != NULL)
                    {
                        currentButton->call();
//                        cerr<<"click";
                            g_buttonclick.play();
                        activeButton=currentButton;
                        inputText=*(activeButton->m_text);
                    }
                }
            }
            if (event->type == SDL_MOUSEMOTION)
                currentButton =
                    g_ACTIVEMENU->getButtonFromMouse(event->motion.x, event->motion.y);
            if( event->type == SDL_KEYDOWN )
            {
                //Handle backspace
                if( event->key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
                {
                    //lop off character
                    inputText.pop_back();
                    renderTextNew = true;
                }
                //Handle copy
                else if( event->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
                {
                    SDL_SetClipboardText( inputText.c_str() );
                }
                //Handle paste
                else if( event->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
                {
                    inputText = SDL_GetClipboardText();
                    renderTextNew = true;
                }
            }
            //Special text input event
            else if( event->type == SDL_TEXTINPUT )
            {
                //Not copy or pasting
                if( !( SDL_GetModState() & KMOD_CTRL && ( event->text.text[ 0 ] == 'c' || event->text.text[ 0 ] == 'C' || event->text.text[ 0 ] == 'v' || event->text.text[ 0 ] == 'V' ) ) )
                {
                    //Append character
                    inputText += event->text.text;
                    renderTextNew = true;
                }
            }
        }

        if(renderTextNew){
                *(activeButton->m_text)=inputText;
        }
        //drawing stage
        SDL_RenderClear(gRenderer);
        g_ACTIVEMENU->renderBG();
        g_ACTIVEMENU->renderButtons();
        g_ACTIVEMENU->renderBoxes();


        if (currentButton != NULL) currentButton->highlight();    //doesnt work. check highlight definition.
        if(g_ACTIVEMENU->failed) failure_message();
        //order of the buttons and highlight is important
        SDL_RenderPresent(gRenderer);
    }
}
//void activate_menu(Menu m)
//{
//    m.refresh();
//    cerr<<"refreshed";
//    g_ACTIVEMENU->pausemusic();
//    g_ACTIVEMENU=&m;
//    cerr<<"activated";
//    m.playmusic();
//    cerr<<"played music";
//}
//void activate_finalizeGame()
//{
//    activate_menu(g_finalizeGame);
//}
//void activate_mainmenu()
//{
//    activate_menu(g_mainmenu);
//}
//void activate_leaderboard()
//{
//    activate_menu(g_leaderboard);
//}
//void activate_settings()
//{
//    activate_menu(g_settingsmenu);
//}
void activate_mainmenu()
{
    g_mainmenu.refresh();
    g_ACTIVEMENU=&g_mainmenu;
    g_ACTIVEMENU->playmusic();
}
void activate_leaderboard()
{
    g_leaderboard.refresh();
//    cout<<g_leadertext;
    g_ACTIVEMENU=&g_leaderboard;
        g_ACTIVEMENU->playmusic();
}
void activate_finalizeGame()
{
    g_finalizeGame.refresh();
    g_ACTIVEMENU=&g_finalizeGame;
        g_ACTIVEMENU->playmusic();
}
void activate_settings()
{
    g_settingsmenu.refresh();
    g_ACTIVEMENU=&g_settingsmenu;
    g_ACTIVEMENU->playmusic();
}
void activate_help()
{
    g_helpmenu.refresh();
    g_ACTIVEMENU=&g_helpmenu;
    g_ACTIVEMENU->playmusic();
}


void exit_game()
{
    data.savedata();
    loopFlag=false;

}
void createButtonsformainmenu()   //button maker, fills each menu with options. we'll need one of these for every menu we have.
{

    g_mainmenu.loadBGtexture("./textures/m.png");
    Button playbutton(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/2-BUTTON_HEIGHT/2,BUTTON_WIDTH,BUTTON_HEIGHT);
    playbutton.loadtexture("./textures/play.png");
    playbutton.setHandlerFunc(activate_finalizeGame);
    g_mainmenu.addButton(playbutton);
    Button settings(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/2-BUTTON_HEIGHT/2+BUTTON_SPACING+BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
    settings.loadtexture("./textures/settings.png");
    settings.setHandlerFunc(activate_settings);
    g_mainmenu.addButton(settings);
    Button leader(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/2-BUTTON_HEIGHT/2+2*BUTTON_SPACING+2*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
    leader.loadtexture("./textures/leader.png");
    leader.setHandlerFunc(activate_leaderboard);
    g_mainmenu.addButton(leader);

}

void createButtonsforhelpmenu()
{
    g_helpmenu.loadBGtexture("./textures/helpscreen.png");
    Button home(WINDOW_WIDTH*7/8,WINDOW_HEIGHT*7/8,50,50);
    home.loadtexture("./textures/home.png");
    home.setHandlerFunc(activate_mainmenu);
    g_helpmenu.addButton(home);


}
void createButtonsforSettings()
{
    g_settingsmenu.loadBGtexture("./textures/m.png");
    Button home(WINDOW_WIDTH*7/8,WINDOW_HEIGHT*7/8,50,50);
    home.loadtexture("./textures/home.png");
    home.setHandlerFunc(activate_mainmenu);
    g_settingsmenu.addButton(home);

    Button exit(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/3+3*BUTTON_SPACING+3*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
    exit.loadtexture("./textures/exit.png");
    exit.setHandlerFunc(exit_game);
    g_settingsmenu.addButton(exit);

    Button sound(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/3+2*BUTTON_SPACING+2*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
        if(wantsound)
        {
        sound.loadtexture("./textures/soundon.png");
        sound.loadalttexture("./textures/soundoff.png");
        }
        else
        {
        sound.loadtexture("./textures/soundoff.png");
        sound.loadalttexture("./textures/soundon.png");
        }

    sound.setHandlerFunc(soundbuttonfunc);
    g_settingsmenu.addButton(sound);

    Button music(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/3+BUTTON_SPACING+BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
        if(wantmusic)
        {
        music.loadtexture("./textures/musicon.png");
        music.loadalttexture("./textures/musicoff.png");
        }
        else
        {
        music.loadtexture("./textures/musicoff.png");
        music.loadalttexture("./textures/musicon.png");
        }
    music.setHandlerFunc(musicbuttonfunc);
    g_settingsmenu.addButton(music);

    Button help(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/3,BUTTON_WIDTH,BUTTON_HEIGHT);
    help.loadtexture("./textures/help.png");
    help.setHandlerFunc(activate_help);
    g_settingsmenu.addButton(help);
//musics
//    g_settingsmenu.loadmusic("./sound/menu.mp3");
}

void loadleadernames()
{
    data.makeleader();
    data.authenticate(g_username,g_password);
    g_leadertextnames.flush();
    string name;
    for (int i=1+g_leaderboard_start_index;i<7+g_leaderboard_start_index;++i)
    {
        name=data.getnames()[i-1];
        g_leadertextnames.insertline(to_string(i)+"  "+name);

    }
//    g_leaderboard.loadmusic("./sound/menu.mp3");
}void loadleadernscores()
{
    g_leadertextscores.flush();
    string name,score;
    for (int i=1+g_leaderboard_start_index;i<7+g_leaderboard_start_index;++i)
    {
        score=to_string(data.gethighscores()[i-1]);
        g_leadertextscores.insertline(score);

    }
}
void down_leaderboard_func()
{
    if (g_leaderboard_start_index<data.getnames().size()-7)
    {
        ++g_leaderboard_start_index;
    }
    g_leaderboard.refresh();
}
void up_leaderboard_func()
{
    if (g_leaderboard_start_index>0)
    {
        --g_leaderboard_start_index;
    }
    g_leaderboard.refresh();

}
void createButtonsforleaderboard()
{
    int index;
    g_leaderboard.loadBGtexture("./textures/m.png");
    Button home(WINDOW_WIDTH*7/8,WINDOW_HEIGHT*7/8,50,50);
    home.loadtexture("./textures/home.png");
    home.setHandlerFunc(activate_mainmenu);
    g_leaderboard.addButton(home);
    Button leaderboardTable(WINDOW_WIDTH/4,WINDOW_HEIGHT/4,WINDOW_WIDTH/2,WINDOW_HEIGHT*2/3);
    leaderboardTable.loadtexture("./textures/leaderTable.png");
    g_leaderboard.addButton(leaderboardTable);
    loadleadernames();
    loadleadernscores();


    Button down(leaderboardTable.m_rect.x-70,leaderboardTable.m_rect.y+leaderboardTable.m_rect.h-70,50,50);
    down.loadtexture("./textures/down.png");
    down.setHandlerFunc(down_leaderboard_func);
     Button up(leaderboardTable.m_rect.x-70,leaderboardTable.m_rect.y+20,50,50);
     up.loadtexture("./textures/up.png");
     up.setHandlerFunc(up_leaderboard_func);

    g_leaderboard.addButton(down);
    g_leaderboard.addBox(g_leadertextnames);
    g_leaderboard.addBox(g_leadertextscores);
    g_leaderboard.addButton(up);

//musics
//    g_signIn.loadmusic("./sound/menu.mp3");
//    cout<<"init";
}
void next_map_func()
{
    if (g_map_index+1<maps.size())
    {
        ++g_map_index;
    }
    g_map_name.flush();
    g_map_name.insertline(maps[g_map_index]);
    g_finalizeGame.refresh();
}
void prev_map_func()
{
    if (g_map_index>0)
    {
        --g_map_index;
    }
    g_map_name.flush();
    g_map_name.insertline(maps[g_map_index]);
    g_finalizeGame.refresh();
}
void next_mode_func()
{
    if (g_mode_index+1<modes.size())
    {
        ++g_mode_index;
    }
    g_mode_name.flush();
    g_mode_name.insertline(modes[g_mode_index]);
    g_finalizeGame.refresh();
}
void prev_mode_func()
{
    if (g_mode_index>0)
    {
        --g_mode_index;
    }
    g_mode_name.flush();
    g_mode_name.insertline(modes[g_mode_index]);
    g_finalizeGame.refresh();
}
void next_diff_func()
{
    if (g_difficulty_index<4)
    {
        ++g_difficulty_index;
    }
    g_difficulty_text.flush();
    g_difficulty_text.insertline(diffs[g_difficulty_index]);
    g_finalizeGame.refresh();
}
void prev_diff_func()
{
    if (g_difficulty_index>0)
    {
        --g_difficulty_index;
    }
    g_difficulty_text.flush();
    g_difficulty_text.insertline(diffs[g_difficulty_index]);
    g_finalizeGame.refresh();
}

void next_theme_func()
{
    if (g_theme_index<2)
    {
        ++g_theme_index;
    }
    g_theme_text.flush();
    g_theme_text.insertline(themes[g_theme_index]);
    g_finalizeGame.refresh();
}
void prev_theme_func()
{
    if (g_theme_index>0)
    {
        --g_theme_index;
    }
    g_theme_text.flush();
    g_theme_text.insertline(themes[g_theme_index]);
    g_finalizeGame.refresh();
}
void won_button_func()
{
    game_ending=-1;
    g_finalizeGame.refresh();
}
void confirm_playbutton_func()
{

    LoadMedia();
    g_mainmenumusic.pause();
    g_gamemusic.play();
    switch (g_mode_index)
    {
        case 0:    SampleGameLoop(g_map_index); break;
        case 1:     TimerGameLoop(g_map_index); break;
    }
    ////need to pass game mode and map to game loop -> global in header is a good idea
    g_mainmenumusic.play();
    data.updatedata(score,power);
    g_finalizeGame.refresh();

}
void createButtonsforfinalzeGame()
{
    g_finalizeGame.loadBGtexture("./textures/finalize.png");
    Button home(WINDOW_WIDTH*7/8,WINDOW_HEIGHT*7/8,50,50);
    home.loadtexture("./textures/home.png");
    home.setHandlerFunc(activate_mainmenu);
    g_finalizeGame.addButton(home);

    Button next_map(g_map_name.m_rect.x+g_map_name.m_rect.w+BUTTON_SPACING+50,g_map_name.m_rect.y,50,50);
    next_map.loadtexture("./textures/right.png");
    next_map.setHandlerFunc(next_map_func);

    Button prev_map(g_map_name.m_rect.x-BUTTON_SPACING-50-3*TEXT_LINE_SPACING,g_map_name.m_rect.y,50,50);
    prev_map.loadtexture("./textures/left.png");
    prev_map.setHandlerFunc(prev_map_func);

    Button next_mode(g_mode_name.m_rect.x+g_mode_name.m_rect.w+BUTTON_SPACING+50,g_mode_name.m_rect.y,50,50);
    next_mode.loadtexture("./textures/right.png");
    next_mode.setHandlerFunc(next_mode_func);

    Button prev_mode(g_mode_name.m_rect.x-BUTTON_SPACING-50-3*TEXT_LINE_SPACING,g_mode_name.m_rect.y,50,50);
    prev_mode.loadtexture("./textures/left.png");
    prev_mode.setHandlerFunc(prev_mode_func);

    Button next_diff(g_difficulty_text.m_rect.x+g_difficulty_text.m_rect.w+BUTTON_SPACING+50,g_difficulty_text.m_rect.y,50,50);
    next_diff.loadtexture("./textures/right.png");
    next_diff.setHandlerFunc(next_diff_func);

    Button prev_diff(g_theme_text.m_rect.x-BUTTON_SPACING-50-3*TEXT_LINE_SPACING,g_difficulty_text.m_rect.y,50,50);
    prev_diff.loadtexture("./textures/left.png");
    prev_diff.setHandlerFunc(prev_theme_func);

    Button next_theme(g_theme_text.m_rect.x+g_theme_text.m_rect.w+BUTTON_SPACING+50,g_theme_text.m_rect.y,50,50);
    next_theme.loadtexture("./textures/right.png");
    next_theme.setHandlerFunc(next_theme_func);

    Button prev_theme(g_theme_text.m_rect.x-BUTTON_SPACING-50-3*TEXT_LINE_SPACING,g_theme_text.m_rect.y,50,50);
    prev_theme.loadtexture("./textures/left.png");
    prev_theme.setHandlerFunc(prev_theme_func);

    Button confirmplay(WINDOW_WIDTH*3/4,WINDOW_HEIGHT/2,3*BUTTON_WIDTH/4-40,3*BUTTON_HEIGHT/4);
    confirmplay.loadtexture("./textures/confirmplay.png");
    confirmplay.setHandlerFunc(confirm_playbutton_func);
//        cerr<<game_ending;
    if(game_ending==-1)
    {
        g_finalizeGame.addBox(g_map_name);
        g_finalizeGame.addButton(prev_map);
        g_finalizeGame.addButton(next_map);
        g_finalizeGame.addButton(next_mode);
        g_finalizeGame.addButton(prev_mode);
        g_finalizeGame.addBox(g_mode_name);
        g_finalizeGame.addButton(confirmplay);
        g_finalizeGame.addButton(prev_diff);
        g_finalizeGame.addButton(next_diff);
        g_finalizeGame.addButton(next_theme);
        g_finalizeGame.addButton(prev_theme);
        g_finalizeGame.addBox(g_difficulty_text);
        g_finalizeGame.addBox(g_theme_text);
    }
    if (game_ending==0)
    {
        Button won(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
        won.loadtexture("./textures/won.png");
        won.setHandlerFunc(won_button_func);

        g_score_text.flush();
        g_score_text.insertline(to_string(score));

        g_power_text.flush();
        g_power_text.insertline(to_string(power));

        g_finalizeGame.addButton(won);
        g_finalizeGame.addBox(g_score_text);
        g_finalizeGame.addBox(g_power_text);
    }

    if (game_ending==1)
    {
        Button lost(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
        lost.loadtexture("./textures/lost.png");
        lost.setHandlerFunc(won_button_func);

        g_score_text.flush();
        g_score_text.insertline(to_string(score));

        g_finalizeGame.addButton(lost);
        g_finalizeGame.addBox(g_score_text);
    }
}
void registerButtonFunc()
{
    if (data.add_user(g_username,g_password,0,0))
    {
        data.authenticate(g_username,g_password);
        g_ACTIVEMENU=&g_mainmenu;
    }
    else
    {
        g_signIn.failed=true;
    }

}
void loginButtonFunc()
{
    if (data.authenticate(g_username,g_password))
    {

        activate_mainmenu();
    }
    else
    {
        g_signIn.failed=true;
    }
}
void createButtonsforsignIn()
{
//buttons
    g_signIn.loadBGtexture("./textures/signIn.png");
    Button username(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/4-BUTTON_HEIGHT/2,BUTTON_WIDTH,BUTTON_HEIGHT);
    username.loadtexture("./textures/nothing");
    username.IsTextBox=true;
    Button password(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/4+BUTTON_HEIGHT/2+BUTTON_SPACING,BUTTON_WIDTH,BUTTON_HEIGHT);
    password.loadtexture("./textures/nothing");
    password.m_font = PASSWORD_FONT;
    password.IsTextBox=true;
    Button login(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/2-BUTTON_HEIGHT/2,BUTTON_WIDTH,BUTTON_HEIGHT);
    login.loadtexture("./textures/login.png");
    Button reg(WINDOW_WIDTH/2-BUTTON_WIDTH/2,WINDOW_HEIGHT/2-BUTTON_HEIGHT/2+BUTTON_SPACING+BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT);
    reg.loadtexture("./textures/register.png");
    username.m_text=&g_username;
    password.m_text=&g_password;
    reg.setHandlerFunc(registerButtonFunc);
    login.setHandlerFunc(loginButtonFunc);
    g_signIn.addButton(reg);
    g_signIn.addButton(login);
    g_signIn.addButton(username);
    g_signIn.addButton(password);
}


int main(int argc, char **argv)
{
    init();
    g_mainmenumusic.load("./sound/menu.mp3");
    g_buttonclick.load("./sound/click.mp3",BUTTONS_CHANNEL);
    g_gamemusic.load("./sound/game.mp3");
    g_shotsound.load("./sound/shot.wav",whatever);
    g_smallblast.load("./sound/smallblast.wav",whatever);
//    g_bigblast.load("./sound/bigblast.wav",whatever);
    g_map_index=0;
    g_map_name.flush();
    g_map_name.insertline(maps[g_map_index]);

    g_mode_index=0;
    g_mode_name.flush();
    g_mode_name.insertline(modes[g_mode_index]);

    g_difficulty_text.insertline(diffs[g_difficulty_index]);
    g_theme_text.insertline(themes[g_theme_index]);

    g_mainmenumusic.play();

    g_mainmenu.setrefresher(createButtonsformainmenu);
    g_signIn.setrefresher(createButtonsforsignIn);
    g_leaderboard.setrefresher(createButtonsforleaderboard);
    g_finalizeGame.setrefresher(createButtonsforfinalzeGame);
    g_settingsmenu.setrefresher(createButtonsforSettings);
    g_helpmenu.setrefresher(createButtonsforhelpmenu);

    data.loaduserdata();

//    data.maw();
    createButtonsformainmenu();
    createButtonsforsignIn();
    g_ACTIVEMENU=&g_signIn;
    GODDAMN_mainloop_mainmenu();
    data.maw();
    data.savedata();
    IMG_Quit();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
    return 0;

}
