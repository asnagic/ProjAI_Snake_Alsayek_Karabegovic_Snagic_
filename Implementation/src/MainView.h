#pragma once
#include <gui/View.h>
#include <gui/Timer.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include "ViewSnake.h"
#include "Difficulty.h"
#define TIMER_ID_SNAKE 100


const float FPS = 50.0f;
const float dT = 1.0f / FPS;

class MainView : public gui::View
{
private:
    ViewSnake _snake;
    gui::GridLayout _layout;
    gui::Timer _timer;

    std::function<void()>* _pUpdateMenuAndTB;
    std::function<void(td::INT1, int, int)>* _pUpdateStatusBar;
    std::function<void(td::INT1)>* _pGameEvent;
    std::function<void()> _fnStartGame;   // canvas ga zove kad se izabere mod

public:
    MainView(
        std::function<void()>* pUpdateMenuAndTB,
        std::function<void(td::INT1, int, int)>* pUpdateStatusBar,
        std::function<void(td::INT1)>* pGameEvent)
        : _snake(pUpdateMenuAndTB, pUpdateStatusBar)
        , _layout(1, 1)
        , _timer(this, dT, false)
        , _pUpdateMenuAndTB(pUpdateMenuAndTB)
        , _pUpdateStatusBar(pUpdateStatusBar)
        , _pGameEvent(pGameEvent)
    {
        gui::GridComposer composer(_layout);
        composer.appendRow(_snake);
        setLayout(&_layout);

        _snake.setFocus(true);

        _fnStartGame = std::bind(&MainView::startGameLoop, this);
        _snake.setStartGameCallback(&_fnStartGame);
    }

    // Pokrece tajmer nakon sto je mod izabran na pocetnom ekranu
    void startGameLoop()
    {
        if (!_timer.isRunning())
            _timer.start();

        if (_pUpdateMenuAndTB) (*_pUpdateMenuAndTB)();
    }

    bool isAIMode() const { return _snake.isAI(); }

    void focusOnCanvas()
    {
        _snake.setFocus(true);
    }

    bool isRunning() const
    {
        return _snake.isGameInProgress();
    }

    void setGameDifficulty(Difficulty d) {
        _snake.setDifficulty(d);
    }
    void startStop()
    {
        // Provjera da li je zmija mrtva koristeći novu metodu koju smo dodali u ViewSnake
        if (_snake.isGameOver())
        {
            _snake.newGame();
            _timer.start();
            if (_pUpdateMenuAndTB) (*_pUpdateMenuAndTB)();
            return;
        }

        if (_timer.isRunning())
        {
            _timer.stop();
        }
        else
        {
            _timer.start();
        }

        if (_pUpdateMenuAndTB) (*_pUpdateMenuAndTB)();
    }

    void activateSpeedBoost() {
        _snake.setSpeedMultiplier(2.0);
    }

    void addExtraTime(int seconds) {
        _snake.addExtraTime(seconds);
    }

    void setAIMode(bool enabled) {
        _snake.setAI(enabled);
    }

    void setBombsEnabled(bool enabled) {
        _snake.enableBombs(enabled);
    }
    
protected:
    bool onTimer(gui::Timer* pTimer) override
    {
        _snake.timeStep();
        return true;
    }
};

