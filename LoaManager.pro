QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api/api_manager.cpp \
    api/search_option.cpp \
    db/db_manager.cpp \
    db/document_builder.cpp \
    function/auction_calculator/auction_calculator.cpp \
    function/smart_search/smart_search.cpp \
    function/smart_search/smart_search_abilitystone.cpp \
    function/smart_search/smart_search_accessory.cpp \
    function/smart_search/smart_search_engravebook.cpp \
    function/smart_search/smart_search_gem.cpp \
    function/smart_search/smart_search_reforge.cpp \
    function/smart_search/smart_search_tripod.cpp \
    game/engrave/engrave.cpp \
    game/engrave/engrave_manager.cpp \
    game/item/abilitystone.cpp \
    game/item/accessory.cpp \
    game/item/gem.cpp \
    game/item/item.cpp \
    game/skill/skill.cpp \
    game/skill/skill_manager.cpp \
    game/skill/tripod.cpp \
    main.cpp \
    loamanager.cpp \
    resource/resource_manager.cpp \
    ui/admin_login.cpp \
    ui/font_manager.cpp \
    ui/widget_manager.cpp

HEADERS += \
    api/api_manager.h \
    api/lostark_api.h \
    api/search_option.h \
    db/db_enums.h \
    db/db_manager.h \
    db/document_builder.h \
    function/auction_calculator/auction_calculator.h \
    function/function_widget.h \
    function/smart_search/smart_search.h \
    function/smart_search/smart_search_abilitystone.h \
    function/smart_search/smart_search_accessory.h \
    function/smart_search/smart_search_engravebook.h \
    function/smart_search/smart_search_gem.h \
    function/smart_search/smart_search_menu.h \
    function/smart_search/smart_search_reforge.h \
    function/smart_search/smart_search_tripod.h \
    game/character/ability.h \
    game/engrave/engrave.h \
    game/engrave/engrave_manager.h \
    game/item/abilitystone.h \
    game/item/accessory.h \
    game/item/gem.h \
    game/item/item.h \
    game/item/item_grade.h \
    game/item/item_type.h \
    game/skill/skill.h \
    game/skill/skill_manager.h \
    game/skill/tripod.h \
    loamanager.h \
    resource/resource_manager.h \
    ui/admin_login.h \
    ui/font_manager.h \
    ui/widget_manager.h \
    util/util.h

FORMS += \
    function/auction_calculator/auction_calculator.ui \
    function/smart_search/smart_search.ui \
    function/smart_search/smart_search_abilitystone.ui \
    function/smart_search/smart_search_accessory.ui \
    function/smart_search/smart_search_engravebook.ui \
    function/smart_search/smart_search_gem.ui \
    function/smart_search/smart_search_reforge.ui \
    function/smart_search/smart_search_tripod.ui \
    loamanager.ui \
    ui/admin_login.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../../../mongo-cxx-driver/lib/ -llibbsoncxx.dll

INCLUDEPATH += $$PWD/../../../mongo-cxx-driver/include
DEPENDPATH += $$PWD/../../../mongo-cxx-driver/include

win32: LIBS += -L$$PWD/../../../mongo-cxx-driver/lib/ -llibmongocxx.dll

INCLUDEPATH += $$PWD/../../../mongo-cxx-driver/include
DEPENDPATH += $$PWD/../../../mongo-cxx-driver/include

RESOURCES += \
    resource/resource.qrc
