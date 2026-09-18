#pragma once
#include <gui/View.h>
#include <gui/Label.h>
#include <gui/ComboBox.h>
#include <gui/CheckBox.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include <gui/Application.h>
#include "Difficulty.h"

class ViewSettings : public gui::View
{
protected:
    gui::Label _lblLevel;
    gui::ComboBox _cmbLevel;

    gui::Label _lblAIMode;
    gui::CheckBox _chbAIMode;

    gui::GridLayout _gl;

public:
    ViewSettings()
        : _gl(2, 2)
        , _lblLevel(tr("Difficulty:"))
        , _lblAIMode(tr("AI Mode Enabled:"))
        , _chbAIMode("")
    {
        //components
        _cmbLevel.addItem(tr("Easy"));
        _cmbLevel.addItem(tr("Medium"));
        _cmbLevel.addItem(tr("Hard"));

        auto appProps = getAppProperties();
        _cmbLevel.selectIndex(appProps->getValue("difficulty", 1));
        _chbAIMode.setChecked(appProps->getValue("mode", 0) == 1);

        _gl.insert(0, 0, _lblLevel, td::HAlignment::Left, td::VAlignment::Center);
        _gl.insert(0, 1, _cmbLevel, td::HAlignment::Left, td::VAlignment::Center);

        _gl.insert(1, 0, _lblAIMode, td::HAlignment::Left, td::VAlignment::Center);
        _gl.insert(1, 1, _chbAIMode, td::HAlignment::Left, td::VAlignment::Center);

        _gl.setSpaceBetweenCells(10, 10);
        _gl.setMargins(10, 10);

        setLayout(&_gl);
    }

    //difficulty for dialogsettings
    int getDifficulty() const
    {
        return _cmbLevel.getSelectedIndex();
    }

    //mode for dialogsettings
    int getMode() const
    {
        return _chbAIMode.isChecked() ? 1 : 0;
    }

    td::String getTranslationExt() const { return ""; }
    bool isRestartRequired() const { return false; }
    void setMainTB(gui::ToolBar* pTB) { /* Not needed for snake logic */ }
};
