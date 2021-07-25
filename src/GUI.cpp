#include "GUI.h"

GUI::GUI(Resources* res, Board* board, int width, int height)
    : res(res), board(board), width(width), height(height),
      mouseDown(false), click(false), levelSelectShowing(false),
      levelScroll(0), currentLevel(" ")
{
    std::ifstream levelsFile("assets/levels/levels.index");
    if (!levelsFile.fail())
    {
        while (!levelsFile.eof())
        {
            std::string name;
            std::getline(levelsFile, name);
            levelNames.push_back(name);
        }
    }
	levelsFile.close();
}

GUI::~GUI()
{
}

bool GUI::input(SDL_Event* event)
{
    if (event->type == SDL_MOUSEMOTION)
    {
        mouse = Coordinates(event->motion.x, event->motion.y);
        return false;
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (mouseOverButton() == Button::NONE && !levelSelectShowing)
                return false;
            else
            {
                mouseDown = true;
                return true;
            }
        }
    }
    else if (event->type == SDL_MOUSEBUTTONUP)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            if (mouseDown)
            {
                click = true;
                mouseDown = false;
                return true;
            }
            return false;
        }
    }
    else if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_ESCAPE)
        {
            if (!board->locked())
            {
                levelSelectShowing = !levelSelectShowing;
                return true;
            }
        }
    }
    if (levelSelectShowing)
    {
        if (event->type == SDL_MOUSEWHEEL)
        {
            if (levelSelectShowing)
            {
                if (event->wheel.y > 0)
                {
                    if (levelScroll > 0)
                        levelScroll--;
                }
                else
                {
                    if (levelScroll + 7 < levelNames.size())
                        levelScroll++;
                }
            }
        }
        return true;
    }

    return false;
}

void GUI::update()
{
    if (click)
    {
        if (!board->locked())
        {
            if (levelSelectShowing)
            {
                int level = mouseOverListItem() + levelScroll;
                if (level >= 0 && level < levelNames.size())
                {
                    std::string name = levelNames.at(level);
                    if (name.at(0) == '\\')
                        name = name.substr(1, name.size() - 1);
                    if (board->loadLevel("assets/levels/" + name + ".level"))
                    {
                        currentLevel = name;
                        levelSelectShowing = false;
                        Mix_PlayChannel(-1, res->s.click, 0);
                    }
                }
            }
            else
            {
                switch (mouseOverButton())
                {
                case Button::LEVEL_SELECT:
                    Mix_PlayChannel(-1, res->s.click, 0);
                    levelSelectShowing = true;
                    break;
                case Button::RESET:
                    Mix_PlayChannel(-1, res->s.click, 0);
                    board->reset();
                    break;
                case Button::UNDO:
                    Mix_PlayChannel(-1, res->s.click, 0);
                    board->undo();
                    break;
                case Button::REDO:
                    Mix_PlayChannel(-1, res->s.click, 0);
                    board->redo();
                    break;
                default: break;
                }
            }
        }
        click = false;
    }
    if (board->didWin() && currentLevel.at(0) != '\\')
    {
        std::ofstream ofs("assets/levels/levels.index");
        if (!ofs.fail())
        {
            std::string ending = "";
            for (unsigned int i = 0; i < levelNames.size(); i++)
            {
                std::string name = levelNames.at(i);
                if (name == currentLevel)
                {
                    name = '\\' + name;
                    levelNames.at(i) = name;
                }
                ofs << ending << name;
                ending = "\n";
            }
            currentLevel = '\\' + currentLevel;
        }
        ofs.close();
    }
}

void GUI::draw(SDL_Renderer* ren)
{
    SDL_Texture* levelSelectButton = res->t.levelSelectButton;
    SDL_Texture* resetButton = res->t.resetButton;
    SDL_Texture* undoButton = res->t.undoButton;
    SDL_Texture* redoButton = res->t.redoButton;

    if (mouseDown)
    {
        switch (mouseOverButton())
        {
        case Button::LEVEL_SELECT:
            levelSelectButton = res->t.levelSelectButtonDown;
            break;
        case Button::RESET:
            resetButton = res->t.resetButtonDown;
            break;
        case Button::UNDO:
            undoButton = res->t.undoButtonDown;
            break;
        case Button::REDO:
            redoButton = res->t.redoButtonDown;
            break;
        default: break;
        }
    }

    SDL_Rect dst;
    dst.w = buttonSize * guiScale;
    dst.h = buttonSize * guiScale;

    dst.x = dst.w * 0;
    dst.y = dst.h * 0;
    SDL_RenderCopy(ren, levelSelectButton, nullptr, &dst);
    dst.x = dst.w * 1;
    dst.y = dst.h * 0;
    SDL_RenderCopy(ren, resetButton, nullptr, &dst);
    dst.x = dst.w * 0;
    dst.y = dst.h * 1;
    SDL_RenderCopy(ren, undoButton, nullptr, &dst);
    dst.x = dst.w * 1;
    dst.y = dst.h * 1;
    SDL_RenderCopy(ren, redoButton, nullptr, &dst);

    if (board->didWin())
    {
        std::string winStr = "You Won!";
        SDL_Color textColor;
        textColor.r = 0; textColor.g = 0; textColor.b = 0; textColor.a = 255;
        drawString(ren, winStr, guiScale * guiWidth / 2 + 6, guiScale * guiHeight / 2 + 6, textColor, true);
        textColor.r = 255; textColor.g = 255; textColor.b = 255; textColor.a = 255;
        drawString(ren, winStr, guiScale * guiWidth / 2, guiScale * guiHeight / 2, textColor, true);
    }

    if (levelSelectShowing)
    {
        int listItemOffsetY = guiScale * listItemHeight;
        dst.x = 0;
        dst.w = guiScale * listItemWidth;
        dst.h = listItemOffsetY;
        int i = levelScroll;
        for (int y = 0; y < guiScale * guiHeight; y += listItemOffsetY)
        {
            dst.y = y;
            SDL_Color textColor;
            if (mouseOverListItem() != y / (guiScale * listItemHeight))
            {
                SDL_RenderCopy(ren, res->t.levelListItem, nullptr, &dst);
                textColor.r = 255; textColor.g = 255; textColor.b = 255; textColor.a = 255;
            }
            else
            {
                SDL_RenderCopy(ren, res->t.levelListItemHighlighted, nullptr, &dst);
                textColor.r = 192; textColor.g = 192; textColor.b = 192; textColor.a = 255;
            }
            if (i >= 0 && i < levelNames.size())
            {
                std::string levelStr = levelNames.at(i);
                if (levelStr.size() > 0 && levelStr.at(0) == '\\')
                {
                    SDL_Rect checkRect;
                    checkRect.x = 0;
                    checkRect.y = y;
                    checkRect.w = guiScale * listItemHeight;
                    checkRect.h = checkRect.w;
                    SDL_RenderCopy(ren, res->t.check, nullptr, &checkRect);
                    levelStr = levelStr.substr(1, levelStr.size() - 1);
                }
                drawString(ren, levelStr, guiScale * listItemWidth / 2, y + guiScale * listItemHeight / 2, textColor, true);
            }
            i++;
        }
    }
}

void GUI::drawString(SDL_Renderer* ren, std::string str, int x, int y, SDL_Color color, bool centered)
{
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid(res->f.excluded, str.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
    SDL_FreeSurface(textSurface);
    int tw, th;
    TTF_SizeUTF8(res->f.excluded, str.c_str(), &tw, &th);
    SDL_Rect textDst;
    if (centered)
    {
        textDst.x = x - guiScale * tw / 2;
        textDst.y = y - guiScale * th / 2;
    }
    else
    {
        textDst.x = x;
        textDst.y = y;
    }
    textDst.w = guiScale * tw;
    textDst.h = guiScale * th;
    SDL_RenderCopy(ren, textTexture, nullptr, &textDst);
    SDL_DestroyTexture(textTexture);
}

GUI::Button GUI::mouseOverButton()
{
    int mouseI = static_cast<int>(mouse.x / guiScale) / buttonSize;
    int mouseJ = static_cast<int>(mouse.y / guiScale) / buttonSize;
    if (mouseJ == 0)
    {
        if (mouseI == 0)
            return Button::LEVEL_SELECT;
        else if (mouseI == 1)
            return Button::RESET;
    }
    else if (mouseJ == 1)
    {
        if (mouseI == 0)
            return Button::UNDO;
        else if (mouseI == 1)
            return Button::REDO;
    }
    return Button::NONE;
}

int GUI::mouseOverListItem()
{
    int mouseJ = static_cast<int>(mouse.y / guiScale) / listItemHeight;
    return mouseJ;
}
