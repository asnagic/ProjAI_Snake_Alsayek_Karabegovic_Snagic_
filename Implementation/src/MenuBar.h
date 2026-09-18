#pragma once
#include <gui/MenuBar.h>
#include <gui/MenuItem.h>

class MenuBar : public gui::MenuBar
{
protected:
    // Koristimo SubMenu klasu jer je tvoj header nudi kao NASLJEDNIKA MenuItem-a
    gui::SubMenu _menuApp;
    gui::SubMenu _menuGame;

public:
    MenuBar()
        : gui::MenuBar(2) // 2 glavne grane
        , _menuApp(10, td::String("App"), 3)   // ID 10, Labela, 3 slota
        , _menuGame(20, td::String("Game"), 7)  // ID 20, Labela, 7 slota
    {
        // 1. POVEZIVANJE NA TRAKU
        setMenu(0, &_menuApp);
        setMenu(1, &_menuGame);

        // 2. POPUNJAVANJE "App" (ID 10)
        auto& appItems = _menuApp.getItems();
        appItems[0].initAsActionItem(td::String("Settings"), 101); // ID 101 za postavke
        appItems[1].initAsSeparator();
        appItems[2].initAsQuitAppActionItem(td::String("Quit"), "q");

        // 3. POPUNJAVANJE "Game" (ID 20)
        auto& gameItems = _menuGame.getItems();

        // Start/Stop Game - ID 10
        gameItems[0].initAsActionItem(td::String("Start/Stop Game"), 10);
        gameItems[0].setAsCheckable(false); // Kvačica će biti na početku isključena

        gameItems[1].initAsSeparator();

        // 4. KREIRANJE "OPTIONS" (Podmeni za težinu)
        // Inicijaliziramo treći item kao SubMenu
        gameItems[2].initAsSubMenu(50, td::String("OPTIONS..."), 3);

        // Dohvatamo listu unutar OPTIONS-a
        auto& diffItems = gameItems[2].getItems();
        // ID-ovi su 20, 21, 22 (isto kao tvoja roze dugmad u StatusBaru)
        diffItems[0].initAsActionItem(td::String("Easy Mode"), 20);
        diffItems[1].initAsActionItem(td::String("Medium Mode"), 21);
        diffItems[2].initAsActionItem(td::String("Hard Mode"), 22);

        // 5. SPEED BOOST (ID 30)
        gameItems[3].initAsActionItem(td::String("Speed Boost"), 30);

        gameItems[4].initAsSeparator();

        // 6. IZBOR MODA - ID-ovi 41 (igrac) i 40 (AI), isti kao u MainWindow::onActionItem
        gameItems[5].initAsActionItem(tr("human"), 41);
        gameItems[5].setAsCheckable(true);   // igrac mod je pocetni

        gameItems[6].initAsActionItem(tr("ai"), 40);
        gameItems[6].setAsCheckable(false);
    }
};