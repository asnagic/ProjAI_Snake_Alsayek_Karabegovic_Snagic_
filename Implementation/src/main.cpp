//
//  Created by Izudin Dzafic on 10 Nov 2022.
//  Copyright © 2022 IDz. All rights reserved.
//
#include "Application.h"
#include <td/StringConverter.h>
#include <gui/WinMain.h>

//Napomena: korigovati ResPaths.txt

int main(int argc, const char * argv[])
{
    Application app(argc, argv);
    const td::String defaultLang("BA");
    app.init(defaultLang);

    auto appProperties = app.getProperties();
    td::String trLang = defaultLang;
    if (appProperties)
        trLang = appProperties->getValue("translation", defaultLang);

    if (trLang != defaultLang)
        app.init(trLang);
    return app.run();
}
