QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api/api_manager.cpp \
    db/db_manager.cpp \
    db/document_builder.cpp \
    function/character_search/character_search.cpp \
    game_data/character/card/card.cpp \
    game_data/character/character.cpp \
    game_data/character/character_manager.cpp \
    game_data/character/collectible/collectible.cpp \
    game_data/character/engrave/engrave.cpp \
    game_data/character/engrave/engrave_manager.cpp \
    game_data/character/item/abilitystone.cpp \
    game_data/character/item/accessory.cpp \
    game_data/character/item/bracelet.cpp \
    game_data/character/item/equip.cpp \
    game_data/character/item/gem.cpp \
    game_data/character/item/item.cpp \
    game_data/character/profile/profile.cpp \
    game_data/character/settingcode_builder.cpp \
    game_data/character/skill/skill.cpp \
    game_data/character/skill/skillrune.cpp \
    main.cpp \
    loamanager.cpp \
    ui/character/character_widget.cpp \
    ui/character/engrave/engrave_widget.cpp \
    ui/character/item/abilitystone_widget.cpp \
    ui/character/item/accessory_widget.cpp \
    ui/character/item/bracelet_widget.cpp \
    ui/character/item/equip_widget.cpp \
    ui/character/item/gem_widget.cpp \
    ui/character/sibling/sibling_widget.cpp \
    ui/character/skill/skill_widget.cpp \
    ui/character/skill/skillrune_widget.cpp \
    ui/character/skill/tripod_widget.cpp \
    ui/class_selector.cpp \
    ui/font_manager.cpp \
    ui/widget_manager.cpp

HEADERS += \
    api/api_manager.h \
    api/enum/lostark_api.h \
    db/db_manager.h \
    db/document_builder.h \
    db/enum/db_enums.h \
    function/character_search/character_search.h \
    game_data/character/card/card.h \
    game_data/character/character.h \
    game_data/character/character_manager.h \
    game_data/character/collectible/collectible.h \
    game_data/character/collectible/enum/collectible_type.h \
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
    game_data/character/settingcode_builder.h \
    game_data/character/skill/skill.h \
    game_data/character/skill/skillrune.h \
    loamanager.h \
    ui/character/character_widget.h \
    ui/character/engrave/engrave_widget.h \
    ui/character/item/abilitystone_widget.h \
    ui/character/item/accessory_widget.h \
    ui/character/item/bracelet_widget.h \
    ui/character/item/equip_widget.h \
    ui/character/item/gem_widget.h \
    ui/character/sibling/sibling_widget.h \
    ui/character/skill/skill_widget.h \
    ui/character/skill/skillrune_widget.h \
    ui/character/skill/tripod_widget.h \
    ui/class_selector.h \
    ui/font_manager.h \
    ui/widget_manager.h

FORMS += \
    function/character_search/character_search.ui \
    loamanager.ui \
    ui/character/character_widget.ui \
    ui/character/engrave/engrave_widget.ui \
    ui/character/item/abilitystone_widget.ui \
    ui/character/item/accessory_widget.ui \
    ui/character/item/bracelet_widget.ui \
    ui/character/item/equip_widget.ui \
    ui/character/item/gem_widget.ui \
    ui/character/sibling/sibling_widget.ui \
    ui/character/skill/skill_widget.ui \
    ui/character/skill/skillrune_widget.ui \
    ui/character/skill/tripod_widget.ui \
    ui/class_selector.ui

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
