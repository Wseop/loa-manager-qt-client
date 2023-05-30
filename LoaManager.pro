QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api/api_manager.cpp \
    api/lostark/search_option.cpp \
    api/statistics/reward_chaos.cpp \
    api/statistics/reward_guardian.cpp \
    function/auction_calculator/auction_calculator.cpp \
    function/character_search/abilitystone_info.cpp \
    function/character_search/accessory_info.cpp \
    function/character_search/bracelet_info.cpp \
    function/character_search/character_info.cpp \
    function/character_search/character_search.cpp \
    function/character_search/equip_info.cpp \
    function/character_search/gem_info.cpp \
    function/character_search/skill_info.cpp \
    function/statistic_armory/statistic_armory.cpp \
    function/statistic_skill/statistic_skill.cpp \
    game/character/character.cpp \
    game/engrave/engrave_manager.cpp \
    game/skill/skill_manager.cpp \
    main.cpp \
    loamanager.cpp \
    resource/resource_manager.cpp \
    ui/font_manager.cpp \
    ui/widget_manager.cpp \
    user/login.cpp

HEADERS += \
    api/api_manager.h \
    api/lostark/search_category.h \
    api/lostark/search_option.h \
    api/resources/resources.h \
    api/statistics/reward_chaos.h \
    api/statistics/reward_guardian.h \
    api/statistics/settings_armory.h \
    api/statistics/settings_skill.h \
    api/statistics/statistics.h \
    function/auction_calculator/auction_calculator.h \
    function/character_search/abilitystone_info.h \
    function/character_search/accessory_info.h \
    function/character_search/bracelet_info.h \
    function/character_search/character_info.h \
    function/character_search/character_search.h \
    function/character_search/equip_info.h \
    function/character_search/gem_info.h \
    function/character_search/skill_info.h \
    function/function_widget.h \
    function/smart_search/smart_search_menu.h \
    function/statistic_armory/statistic_armory.h \
    function/statistic_skill/statistic_skill.h \
    game/character/character.h \
    game/character/collectible.h \
    game/character/profile.h \
    game/engrave/engrave_manager.h \
    game/item/equipment.h \
    game/item/gem.h \
    game/item/item_grade.h \
    game/item/rune.h \
    game/skill/skill.h \
    game/skill/skill_manager.h \
    game/skill/tripod.h \
    loamanager.h \
    resource/resource_manager.h \
    ui/font_manager.h \
    ui/widget_manager.h \
    user/login.h \
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
    function/statistic_armory/statistic_armory.ui \
    function/statistic_skill/statistic_skill.ui \
    loamanager.ui \
    user/login.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc
