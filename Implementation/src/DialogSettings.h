#pragma once
#include <gui/Dialog.h>
#include "ViewSettings.h" 

class DialogSettings : public gui::Dialog
{
protected:
    ViewSettings _view;

    bool onClick(gui::Dialog::Button::ID btnID, gui::Button* pButton) override
    {
        if (btnID == gui::Dialog::Button::ID::OK)
        {
            auto appProps = getAppProperties();
            appProps->setValue("difficulty", _view.getDifficulty());
            appProps->setValue("mode", _view.getMode());
        }
        return true;
    }

public:
    DialogSettings(gui::Frame* pFrame, td::UINT4 wndID = 0)
        : gui::Dialog(pFrame,
            { {gui::Dialog::Button::ID::OK, tr("OK")}, {gui::Dialog::Button::ID::Cancel, tr("Cancel")} },
            gui::Size(300, 250), wndID)
    {
        setTitle("Settings");
        setCentralView(&_view); //GridComposer layout
    }
};