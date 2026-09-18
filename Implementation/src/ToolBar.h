#pragma once
#include <gui/ToolBar.h>
#include <gui/Image.h>

class ToolBar : public gui::ToolBar
{
protected:
    // Slike moraju biti članovi klase da bi ostale u memoriji
    gui::Image _imgStart;
    gui::Image _imgAI;
    gui::Image _imgPlayer;

public:
    // Konstruktor: proslijeđuje ID, broj dozvoljenih stavki (nAllowedItems) i broj podrazumijevanih (nDefaultItems)
    ToolBar()
        : gui::ToolBar("mainTB", 5, 5, false)
        , _imgStart(":start")
        , _imgAI(":ai")
        , _imgPlayer(":human")
    {
        // 1. PLAYER (Manual) - ID: 41
        // Koristimo td::String() za labele i tooltipe
        addItem(td::String("PLAYER"), &_imgPlayer, td::String("Manual Control"), 20, 0, 0, 41);

        // 2. AI MODE - ID: 40
        addItem(td::String("AI MODE"), &_imgAI, td::String("AI Control"), 20, 0, 0, 40);

        // 3. Razmak (Feder) koji gura Start u sredinu
        addSpaceItem();

        // 4. START - ID: 10
        addItem(td::String("START"), &_imgStart, td::String("Start Game"), 20, 0, 0, 10);

        // 5. Još jedan razmak da bi Start ostao centriran
        addSpaceItem();

        // Postavke prikaza
        showLabels(true);
        setIconSize(IconSize::Regular);
    }
};