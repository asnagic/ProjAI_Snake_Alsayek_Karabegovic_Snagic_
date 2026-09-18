#pragma once
#include <gui/StatusBar.h>
#include <gui/Button.h>
#include <gui/HorizontalLayout.h>
#include <gui/Label.h>

class StatusBar : public gui::StatusBar
{
protected:
    // Use the layout owned and measured by gui::StatusBar.

    // Slike moraju biti deklarisane PRIJE kontrola koje ih koriste
    gui::Image _imgHeartFull;
    gui::Image _imgHeartEmpty;
    gui::Image _imgSettings;

    gui::Button _btnEasy, _btnMedium, _btnHard;
    gui::Button _vHearts[3];

    gui::Label _lblScore;
    gui::Button _btnSettings;

public:
    StatusBar()
        : gui::StatusBar(15)

        , _imgHeartFull(":heartFull")
        , _imgHeartEmpty(":heartEmpty")
        , _imgSettings(":settings")
        , _btnEasy(td::String(" EASY "))
        , _btnMedium(td::String(" MEDIUM "))
        , _btnHard(td::String(" HARD "))
        , _lblScore(td::String("SCORE: 000000"))
        , _btnSettings(&_imgSettings)
    {
        // 1. BOJE I STIL (Pink gradacija)
        _btnEasy.setBackgroundColor(td::ColorID::LightPink);
        _btnMedium.setBackgroundColor(td::ColorID::DeepPink);
        _btnHard.setBackgroundColor(td::ColorID::Maroon);

        // Sve postavljamo na FLAT za moderan izgled
        _btnEasy.setFlat();
        _btnMedium.setFlat();
        _btnHard.setFlat();
        _btnSettings.setFlat();

        for (int i = 0; i < 3; i++) {
            _vHearts[i].setImage(&_imgHeartFull); // Postavljamo početnu sliku
            _vHearts[i].setFlat();
            // Fiksna veličina je ključna da Layout ne "zgnječi" dugme na 0 piksela
            _vHearts[i].setSizeLimits(35, gui::Control::Limit::Fixed, 35, gui::Control::Limit::Fixed);
        }
        _layout.appendSpace(10);   // margina da EASY ne bude odsjecen na ivici prozora
        _layout << _btnEasy << _btnMedium << _btnHard;
        _layout.appendSpacer(1);

        _layout << _vHearts[0] << _vHearts[1] << _vHearts[2];
        _layout.appendSpace(20);
        _layout << _lblScore;

        _layout.appendSpacer(1);
        _layout << _btnSettings;

        setLayout(&_layout);
    }

    // --- PROFESIONALNE METODE ---

    void updateScore(int score) {
        td::String str;
        str.format("SCORE: %06d", score); // Profesionalni format sa nulama (npr. 000120)
        _lblScore.setTitle(str);
    }

    void highlightDifficulty(int level) {
        // Resetuj sve (ovdje bi bilo dobro imati metodu za skidanje bolda, 
        // ali ako nema, postavi font na normal)
        _btnEasy.setFont(gui::Font::ID::SystemNormal);
        _btnMedium.setFont(gui::Font::ID::SystemNormal);
        _btnHard.setFont(gui::Font::ID::SystemNormal);

        // Istakni selektovano koristeći setBold() iz Control.h
        if (level == 0) _btnEasy.setBold();
        else if (level == 1) _btnMedium.setBold();
        else if (level == 2) _btnHard.setBold();
    }

    void setLives(int livesCount) {
        for (int i = 0; i < 3; i++) {
            if (i < livesCount) {
                _vHearts[i].setImage(&_imgHeartFull);
            }
            else {
                _vHearts[i].setImage(&_imgHeartEmpty);
            }

            // Umjesto reDraw, koristi tvoj postojeći sistem za update
            // Ako setImage ne osvježi dugme samo, koristi ovo:
            _vHearts[i].enable(true);
        }
     
    }

    // Getteri za MainWindow
    gui::Button& getEasyBtn() { return _btnEasy; }
    gui::Button& getMediumBtn() { return _btnMedium; }
    gui::Button& getHardBtn() { return _btnHard; }
    gui::Button& getSettingsBtn() { return _btnSettings; }
};
