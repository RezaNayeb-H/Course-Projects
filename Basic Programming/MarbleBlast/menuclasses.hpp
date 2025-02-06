#ifndef MENUCLASSES_HPP_INCLUDED
#define MENUCLASSES_HPP_INCLUDED


#define TEXT_COLOR_EVERY 255,255,255,255
#define TEXT_FONT_EVERY "pixel.ttf"
#define PASSWORD_FONT "password.ttf"
#define FONT_DIR "./fonts/"
#define TEXT_LINE_SPACING 60
#define TEXT_MARGIN_BOX 5
#include "MusicMaker.cpp"

#define BUTTON_WIDTH 400
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20 //how much space between buttons vertically
#define BUTTON_HIGHLIGHT_COLOR 0xFFFFFF01  //not functional yet


#define b_iter std::vector<Button>::iterator

MusicAndStuffMainClass MUSIC;

Music g_mainmenumusic;
Music g_gamemusic;
Sound g_buttonclick;
Sound g_shotsound;
Sound g_popsound;
Sound g_smallblast;
Sound g_bigblast;



string bs = "something";
void betterText(SDL_Renderer*,string,int,int,int,int,int,int,int,string);

class Button
{

    void (*func)(void);
    void (*otherfunc)(int);
    bool hasFunction;
    SDL_Texture* mtexture;
    SDL_Texture* mhighlighttexture=nullptr;
    SDL_Texture* m_altTexture=nullptr;


public:
    int m_x,m_y,m_h,m_w;
    string *m_text=&bs;
    string content="";
    string m_font=TEXT_FONT_EVERY;
    int numberForScrolling=0;
    bool IsTextBox=false;
    SDL_Rect m_rect;
    Button(int x, int y, int width, int height)
        : hasFunction(false)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_rect.w = width;
        m_rect.h = height;
        m_x=x;
        m_y=y;
        m_h=height;
        m_w=width;
    }
    void loadtexture(string path)
    {
        mtexture = IMG_LoadTexture(gRenderer, (path).c_str());
        if (mtexture==NULL) cout<<"failed loading highlight texture.";
    }
    void change_texture()
    {
        swap(mtexture,m_altTexture);
    }
    void loadtexturehighlight(string path)
    {
        mhighlighttexture = IMG_LoadTexture(gRenderer, (path).c_str());
        if (mhighlighttexture==NULL) cout<<"failed loading texture.";
    }
    void loadalttexture(string path)
    {
        m_altTexture = IMG_LoadTexture(gRenderer, (path).c_str());
        if (m_altTexture==NULL) cout<<"failed loading texture.";
    }
    void render();
    void highlight();
    void setHandlerFunc (void (*handler_func)(void))
    {
        func = handler_func;
        hasFunction = true;
    }
    void setHandlerFunc (void (*handler_func)(int))
    {
        otherfunc = handler_func;
        hasFunction = true;
    }
    void call()
    {
        if (hasFunction) func();
        if(m_altTexture!=nullptr and m_altTexture!=mtexture) {this->change_texture();
                    cerr<<"texture changed";}
    }

    bool isMouseOnButton(int x, int y)
    {
        return (x >= m_rect.x && x <= m_rect.x + m_rect.w
                && y >= m_rect.y && y <= m_rect.y + m_rect.h);
    }
    void clean()
    {
        SDL_DestroyTexture(mtexture);
        SDL_DestroyTexture(mhighlighttexture);
        SDL_DestroyTexture(m_altTexture);
        mtexture=nullptr;
        mhighlighttexture=nullptr;
        m_altTexture=nullptr;
        SDL_RenderClear(gRenderer);
    }

};

class TextBox
{
    SDL_Texture* mtexture;


public:
    vector<string> lines{};
    SDL_Rect m_rect;
    string m_font = TEXT_FONT_EVERY;
    TextBox(int x,int y,int w,int h)
    {
        m_rect.x=x;
        m_rect.y=y;
        m_rect.w=w;
        m_rect.h=h;
    }
    void insertline(string s)
    {
        lines.push_back(s);
    }
    void flush()
    {
        lines.clear();
    }
    void loadtexture(SDL_Renderer* renderer,string path)
    {
        mtexture=IMG_LoadTexture(renderer,path.c_str());
    }
    void render(SDL_Renderer* renderer);
    void clean()
    {
        SDL_DestroyTexture(mtexture);
        mtexture=NULL;
    }
};

class Menu
{
    std::vector<Button> buttons;
    SDL_Texture* defaultBackgroundTexture;
    Music m_music=g_mainmenumusic;

public:
    void (*func)(void);
    bool failed=false;
    vector<TextBox> boxes;
    void addButton(Button button)
    {
        buttons.push_back(button);
    }
    void addBox(TextBox box)
    {
        boxes.push_back(box);
    }
    b_iter begin()
    {
        return buttons.begin();
    }
    b_iter end()
    {
        return buttons.end();
    }

    Button * getButton(int index)
    {
        return buttons.data()+index;
    }

    Button* getButtonFromMouse(int x, int y)
    {
        for (b_iter b = buttons.begin(); b != buttons.end(); ++b)
        {
            if (b->isMouseOnButton(x, y)) return &(*b);
        }
        return NULL; //PLEASE HANDLE THIS YOURSELF
    }
    void loadBGtexture(string path)
    {
        defaultBackgroundTexture=IMG_LoadTexture(gRenderer,(path).c_str());
        if (defaultBackgroundTexture==NULL) cout<<"failed loading texture.";
    }
    void renderBG();
    void renderButtons();
    void renderBoxes();
    void setrefresher(void (*f)(void))
    {
        func = f;
    }
    void refresh()
    {
    for (b_iter b = buttons.begin(); b != buttons.end(); ++b)
    {
        b->clean();
    }
    for (vector<TextBox>::iterator i=boxes.begin();i!=boxes.end();++i)
    {
        i->clean();
    }
    buttons.clear();
    boxes.clear();
    SDL_DestroyTexture(defaultBackgroundTexture);
    func();
//    cerr<<"          "<<buttons.size()<<"             ";
    }
    void loadmusic(string path)
    {
        m_music.load(path);
    }
    void playmusic()
    {
        m_music.play();
    }
    void pausemusic()
    {
        m_music.pause();
    }

};


void Button::render()
{
    SDL_RenderCopy(gRenderer,mtexture,NULL,&m_rect);
    if (IsTextBox) betterText(gRenderer,*m_text,m_rect.x,m_rect.y,TEXT_COLOR_EVERY,30,m_font);
    betterText(gRenderer,content,m_rect.x,m_rect.y,TEXT_COLOR_EVERY,30,TEXT_FONT_EVERY);
}
void TextBox::render(SDL_Renderer* renderer)
{
        int index;
        SDL_RenderCopy(renderer,mtexture,NULL,&m_rect);
        for(vector<string>::iterator i=lines.begin(); i!=lines.end(); ++i)
        {
            index=int(i-lines.begin());
            betterText(renderer,lines[index],m_rect.x,m_rect.y+index*TEXT_LINE_SPACING,TEXT_COLOR_EVERY,20,m_font);   //20 is text size
//            cout<<lines[index];
        }
}

void Button::highlight()
{
//    SDL_Rect recty{m_x,m_y,m_w,m_h};
//    SDL_SetRenderDrawColor(gRenderer,255,255,255,40);
//    SDL_RenderFillRect(gRenderer,&recty);
}

void Menu::renderButtons()
{
    for (b_iter b = buttons.begin(); b != buttons.end(); ++b)
    {
        b->render();
    }

}
void Menu::renderBoxes()
{
    for (vector<TextBox>::iterator i=boxes.begin();i!=boxes.end();++i)
    {
        i->render(gRenderer);
//        cerr<<g_map_name.lines[0];
    }
}
void Menu::renderBG()
{
    SDL_RenderCopy(gRenderer,defaultBackgroundTexture,NULL,NULL);
}
// better function to render ttf text
void betterText(
    SDL_Renderer *renderer, string S,
    int x, int y,
    int r, int g, int b, int a,
    int size, string Font /*def = arial */)
{
    if (!TTF_WasInit())
        TTF_Init();
    TTF_Font *font = TTF_OpenFont((FONT_DIR+Font).c_str(), size);
    SDL_Color color = {r, g, b, a};
    SDL_Surface *surface = TTF_RenderText_Solid(font, S.c_str(), color);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;

    int h, w;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.h = h;
    rect.w = w;
    rect.x = x ;
    rect.y = y ;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    // free resources
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
#endif // MENUCLASSES_HPP_INCLUDED
