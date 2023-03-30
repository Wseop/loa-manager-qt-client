QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api/api_manager.cpp \
    api/lostark/response_parser.cpp \
    api/lostark/search_option.cpp \
    db/db_manager.cpp \
    db/document_builder.cpp \
    function/auction_calculator/auction_calculator.cpp \
    function/character_search/abilitystone_info.cpp \
    function/character_search/accessory_info.cpp \
    function/character_search/bracelet_info.cpp \
    function/character_search/character_info.cpp \
    function/character_search/character_search.cpp \
    function/character_search/equip_info.cpp \
    function/character_search/gem_info.cpp \
    function/character_search/skill_info.cpp \
    function/content_reward/content_reward.cpp \
    function/content_reward/content_reward_adder.cpp \
    function/content_reward/content_reward_table.cpp \
    function/raid_profit/raid_profit.cpp \
    function/raid_profit/raid_profit_table.cpp \
    function/smart_search/smart_search.cpp \
    function/smart_search/smart_search_abilitystone.cpp \
    function/smart_search/smart_search_accessory.cpp \
    function/smart_search/smart_search_engravebook.cpp \
    function/smart_search/smart_search_gem.cpp \
    function/smart_search/smart_search_reforge.cpp \
    function/smart_search/smart_search_tripod.cpp \
    game/character/card.cpp \
    game/character/character.cpp \
    game/character/profile.cpp \
    game/engrave/engrave.cpp \
    game/engrave/engrave_manager.cpp \
    game/item/abilitystone.cpp \
    game/item/accessory.cpp \
    game/item/armor.cpp \
    game/item/bracelet.cpp \
    game/item/gem.cpp \
    game/item/item.cpp \
    game/item/rune.cpp \
    game/item/weapon.cpp \
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
    api/loamanager/loamanager_api.h \
    api/lostark/lostark_api.h \
    api/lostark/response_auction.h \
    api/lostark/response_market.h \
    api/lostark/response_parser.h \
    api/lostark/search_option.h \
    db/db_enums.h \
    db/db_manager.h \
    db/document_builder.h \
    function/auction_calculator/auction_calculator.h \
    function/character_search/abilitystone_info.h \
    function/character_search/accessory_info.h \
    function/character_search/bracelet_info.h \
    function/character_search/character_info.h \
    function/character_search/character_search.h \
    function/character_search/equip_info.h \
    function/character_search/gem_info.h \
    function/character_search/skill_info.h \
    function/content_reward/content_reward.h \
    function/content_reward/content_reward_adder.h \
    function/content_reward/content_reward_table.h \
    function/function_widget.h \
    function/raid_profit/raid_profit.h \
    function/raid_profit/raid_profit_table.h \
    function/smart_search/smart_search.h \
    function/smart_search/smart_search_abilitystone.h \
    function/smart_search/smart_search_accessory.h \
    function/smart_search/smart_search_engravebook.h \
    function/smart_search/smart_search_gem.h \
    function/smart_search/smart_search_menu.h \
    function/smart_search/smart_search_reforge.h \
    function/smart_search/smart_search_tripod.h \
    game/character/ability.h \
    game/character/card.h \
    game/character/character.h \
    game/character/profile.h \
    game/engrave/engrave.h \
    game/engrave/engrave_manager.h \
    game/item/abilitystone.h \
    game/item/accessory.h \
    game/item/armor.h \
    game/item/bracelet.h \
    game/item/gem.h \
    game/item/item.h \
    game/item/item_grade.h \
    game/item/item_set.h \
    game/item/item_type.h \
    game/item/rune.h \
    game/item/weapon.h \
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
    function/character_search/abilitystone_info.ui \
    function/character_search/accessory_info.ui \
    function/character_search/bracelet_info.ui \
    function/character_search/character_info.ui \
    function/character_search/character_search.ui \
    function/character_search/equip_info.ui \
    function/character_search/gem_info.ui \
    function/character_search/skill_info.ui \
    function/content_reward/content_reward.ui \
    function/content_reward/content_reward_adder.ui \
    function/content_reward/content_reward_table.ui \
    function/raid_profit/raid_profit.ui \
    function/raid_profit/raid_profit_table.ui \
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
