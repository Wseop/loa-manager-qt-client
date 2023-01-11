QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api/api_manager.cpp \
    db/db_manager.cpp \
    game_data/character/card/card.cpp \
    game_data/character/character.cpp \
    game_data/character/engrave/engrave.cpp \
    game_data/character/engrave/engrave_manager.cpp \
    game_data/character/item/abilitystone.cpp \
    game_data/character/item/accessory.cpp \
    game_data/character/item/bracelet.cpp \
    game_data/character/item/equip.cpp \
    game_data/character/item/gem.cpp \
    game_data/character/item/item.cpp \
    game_data/character/profile/profile.cpp \
    game_data/character/skill/skill.cpp \
    game_data/character/skill/skillrune.cpp \
    main.cpp \
    loamanager.cpp \
    ui/font_manager.cpp \
    ui/widget_manager.cpp

HEADERS += \
    api/api_manager.h \
    api/enum/lostark_api.h \
    db/db_manager.h \
    db/enum/db_enums.h \
    game_data/character/card/card.h \
    game_data/character/character.h \
    game_data/character/engrave/engrave.h \
    game_data/character/engrave/engrave_manager.h \
    game_data/character/item/abilitystone.h \
    game_data/character/item/accessory.h \
    game_data/character/item/bracelet.h \
    game_data/character/item/enum/gem_type.h \
    game_data/character/item/enum/item_grade.h \
    game_data/character/item/enum/item_type.h \
    game_data/character/item/enum/set_effect.h \
    game_data/character/item/equip.h \
    game_data/character/item/gem.h \
    game_data/character/item/item.h \
    game_data/character/profile/enum/ability.h \
    game_data/character/profile/enum/class.h \
    game_data/character/profile/profile.h \
    game_data/character/skill/skill.h \
    game_data/character/skill/skillrune.h \
    loamanager.h \
    ui/font_manager.h \
    ui/widget_manager.h

FORMS += \
    loamanager.ui

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
