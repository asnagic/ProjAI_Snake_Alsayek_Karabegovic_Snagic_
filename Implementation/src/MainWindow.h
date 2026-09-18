#pragma once
#include <gui/Window.h>
#include <functional>
#include "MenuBar.h"
#include "MainView.h"
#include "StatusBar.h"
#include "DialogSettings.h"
#include "Difficulty.h"
class MainWindow : public gui::Window
{
protected:
    // Slike - Očišćeno i sređeno
    gui::Image _imgStart;
    gui::Image _imgStop;
    gui::Image _imgSpeed;
    gui::Image _imgTimer;
    gui::Image _imgBomb;
    gui::Image _imgEasy;
    gui::Image _imgMedium;
    gui::Image _imgHard;

    // Barovi i View
    MenuBar   _mainMenuBar;
    StatusBar _statusBar;

    // Callback funkcije za komunikaciju sa MainView
    std::function<void()> _fnUpdateMenuAndTB;
    std::function<void(td::INT1, int, int)> _fnUpdateStatusBar;
    std::function<void(td::INT1)> _fnGameEvent;

    // _mainView mora doci POSLIJE callback-ova koje prima u konstruktoru
    MainView  _mainView;

    bool _aiEnabled = false;
    const td::UINT4 _cSettingsDlgID = 17;

protected:
    void onInitialAppearance() override
    {
        auto appProperties = getAppProperties();
        // Učitavamo sačuvanu težinu ili stavljamo Medium kao default
        Difficulty difficulty = (Difficulty)appProperties->getValue("difficulty", (int)Difficulty::Medium);

        // Podešavamo igru i vizuale
        _mainView.setGameDifficulty(difficulty);
        updateDifficultyIcons(difficulty);

        _mainView.setAIMode(_aiEnabled);//MOZDA MI NE TREBA OVO

        // Sinhronizacija kvačica u meniju za izbor moda
        updateAIIcons(_aiEnabled);

        _mainView.focusOnCanvas();
    }

    bool onKeyPressed(const gui::Key& key) override
    {
        if (key.getChar() == ' ' || (int)key.getVirtual() == 32)
        {
            _mainView.startStop();
            return true;
        }
        return false;
    }

    void updateAIIcons(bool aiOn)
    {
        gui::MenuItem* pAI = _mainMenuBar.getItem(20, 0, 0, 40);
        if (pAI) pAI->setChecked(aiOn);

        gui::MenuItem* pPlayer = _mainMenuBar.getItem(20, 0, 0, 41);
        if (pPlayer) pPlayer->setChecked(!aiOn);
    }

    void updateMenuAndTB()
    {
        bool isRunning = _mainView.isRunning();

        // mod se moze promijeniti i sa pocetnog ekrana (strelice), pa citamo stvarno stanje
        _aiEnabled = _mainView.isAIMode();
        updateAIIcons(_aiEnabled);

        // Ažuriranje kvačice u Game meniju (Start/Stop)
        gui::MenuItem* pMenuItem = _mainMenuBar.getItem(20, 0, 0, 10);
        if (pMenuItem)
            pMenuItem->setChecked(isRunning);
    }

    void connectStatusBar()
    {
        // Povezivanje klikova na tekstove/dugmad u status baru
        _statusBar.getEasyBtn().onClick([this]() {
            _mainView.setGameDifficulty(Difficulty::Easy);
            updateDifficultyIcons(Difficulty::Easy);
            _mainView.focusOnCanvas();
            });

        _statusBar.getMediumBtn().onClick([this]() {
            _mainView.setGameDifficulty(Difficulty::Medium);
            updateDifficultyIcons(Difficulty::Medium);
            _mainView.focusOnCanvas();
            });

        _statusBar.getHardBtn().onClick([this]() {
            _mainView.setGameDifficulty(Difficulty::Hard);
            updateDifficultyIcons(Difficulty::Hard);
            _mainView.focusOnCanvas();
            });

        _statusBar.getSettingsBtn().onClick([this]() {
            auto pDlg = getAttachedWindow(_cSettingsDlgID);
            if (pDlg) pDlg->setFocus();
            else {
                auto* pSettingsDlg = new DialogSettings(this, _cSettingsDlgID);
                pSettingsDlg->open();
            }
            });
    }

    void updateDifficultyIcons(Difficulty d)
    {
        // 1. MenuBar kvačice
        gui::MenuItem* pMI_Easy = _mainMenuBar.getItem(20, 0, 0, 20);
        gui::MenuItem* pMI_Med = _mainMenuBar.getItem(20, 0, 0, 21);
        gui::MenuItem* pMI_Hard = _mainMenuBar.getItem(20, 0, 0, 22);

        if (pMI_Easy) pMI_Easy->setChecked(d == Difficulty::Easy);
        if (pMI_Med)  pMI_Med->setChecked(d == Difficulty::Medium);
        if (pMI_Hard) pMI_Hard->setChecked(d == Difficulty::Hard);

        // 2. StatusBar highlight
        int index = (d == Difficulty::Easy) ? 0 : (d == Difficulty::Medium) ? 1 : 2;
        _statusBar.highlightDifficulty(index);
    }

    bool onActionItem(gui::ActionItemDescriptor& aiDesc) override
    {
        auto [menuID, firstSubMenuID, lastSubMenuID, actionID] = aiDesc.getIDs();

        if (menuID == 10 && actionID == 10) { // Settings
            auto pDlg = getAttachedWindow(_cSettingsDlgID);
            if (pDlg) pDlg->setFocus();
            else {
                auto* pSettingsDlg = new DialogSettings(this, _cSettingsDlgID);
                pSettingsDlg->open();
            }
            return true;
        }

        if (menuID == 20) { // Game menu
            switch (actionID) {
            case 10: _mainView.startStop(); return true;
            case 20: _mainView.setGameDifficulty(Difficulty::Easy); updateDifficultyIcons(Difficulty::Easy); return true;
            case 21: _mainView.setGameDifficulty(Difficulty::Medium); updateDifficultyIcons(Difficulty::Medium); return true;
            case 22: _mainView.setGameDifficulty(Difficulty::Hard); updateDifficultyIcons(Difficulty::Hard); return true;
            case 40: // AI MODE
                _aiEnabled = true;
                _mainView.setAIMode(true);
                updateAIIcons(true);
                _mainView.focusOnCanvas();
                return true;

            case 41: // PLAYER MODE
                _aiEnabled = false;
                _mainView.setAIMode(false);
                updateAIIcons(false);
                _mainView.focusOnCanvas();
                return true;
            }
        }
        return false;
    }

    void updateStatusBar(td::INT1 type, int score, int lives)
    {
        _statusBar.updateScore(score);
        _statusBar.setLives(lives); // Proslijeđujemo stvarne živote iz MainView
    }

    void onGameEvent(td::INT1 event)
    {
        // Event 0 bi mogao biti npr. smrt zmije
        if (event == 0) {
            _statusBar.setLives(0);
        }
    }

public:
    MainWindow()
        : gui::Window(gui::Size(1000, 600))
        , _imgStart(":start"), _imgStop(":stop")
        , _imgSpeed(":speed"), _imgTimer(":timer"), _imgBomb(":dynamite")
        , _imgEasy(":easy"), _imgMedium(":medium"), _imgHard(":hard")
        // Povezivanje callback funkcija
        , _fnUpdateMenuAndTB(std::bind(&MainWindow::updateMenuAndTB, this))
        , _fnUpdateStatusBar(std::bind(&MainWindow::updateStatusBar, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
        , _fnGameEvent(std::bind(&MainWindow::onGameEvent, this, std::placeholders::_1))
        , _mainView(&_fnUpdateMenuAndTB, &_fnUpdateStatusBar, &_fnGameEvent)
    {
        setResizable(false);
        setTitle(tr("Snake Game AI"));

        _mainMenuBar.setAsMain(this);
        setStatusBar(_statusBar);
        setCentralView(&_mainView);
        connectStatusBar();
    }

    ~MainWindow() {}
};

