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
    function/auction_calculator/auction_calculator.cpp \
    function/character_ranking/character_ranking.cpp \
    function/character_search/character_search.cpp \
    function/contents_reward/contents_reward.cpp \
    function/contents_reward/reward_adder.cpp \
    function/contents_reward/reward_widget.cpp \
    function/quotation/abilitystone/abilitystone_price.cpp \
    function/quotation/abilitystone/abilitystone_quotation.cpp \
    function/quotation/reforge/reforge_item.cpp \
    function/quotation/reforge/reforge_quotation.cpp \
    function/quotation/tripod/skill_info_widget.cpp \
    function/quotation/tripod/tripod_quotation.cpp \
    function/raid/raidreward.cpp \
    function/raid/raidreward_profit.cpp \
    function/setting_ranking/characterlist_widget.cpp \
    function/setting_ranking/setting_ranking.cpp \
    function/setting_ranking/setting_widget.cpp \
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
    ui/admin_login.cpp \
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
    ui/engrave_selector.cpp \
    ui/font_manager.cpp \
    ui/widget_manager.cpp

HEADERS += \
    api/api_manager.h \
    api/enum/lostark_api.h \
    db/db_manager.h \
    db/document_builder.h \
    db/enum/db_enums.h \
    function/auction_calculator/auction_calculator.h \
    function/character_ranking/character_ranking.h \
    function/character_search/character_search.h \
    function/contents_reward/contents_reward.h \
    function/contents_reward/reward_adder.h \
    function/contents_reward/reward_widget.h \
    function/quotation/abilitystone/abilitystone_price.h \
    function/quotation/abilitystone/abilitystone_quotation.h \
    function/quotation/reforge/reforge_item.h \
    function/quotation/reforge/reforge_quotation.h \
    function/quotation/tripod/skill_info_widget.h \
    function/quotation/tripod/tripod_quotation.h \
    function/raid/raidreward.h \
    function/raid/raidreward_profit.h \
    function/setting_ranking/characterlist_widget.h \
    function/setting_ranking/setting_ranking.h \
    function/setting_ranking/setting_widget.h \
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
    ui/admin_login.h \
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
    ui/engrave_selector.h \
    ui/font_manager.h \
    ui/widget_manager.h

FORMS += \
    function/auction_calculator/auction_calculator.ui \
    function/character_ranking/character_ranking.ui \
    function/character_search/character_search.ui \
    function/contents_reward/contents_reward.ui \
    function/contents_reward/reward_adder.ui \
    function/contents_reward/reward_widget.ui \
    function/quotation/abilitystone/abilitystone_price.ui \
    function/quotation/abilitystone/abilitystone_quotation.ui \
    function/quotation/reforge/reforge_item.ui \
    function/quotation/reforge/reforge_quotation.ui \
    function/quotation/tripod/skill_info_widget.ui \
    function/quotation/tripod/tripod_quotation.ui \
    function/raid/raidreward.ui \
    function/raid/raidreward_profit.ui \
    function/setting_ranking/characterlist_widget.ui \
    function/setting_ranking/setting_ranking.ui \
    function/setting_ranking/setting_widget.ui \
    loamanager.ui \
    ui/admin_login.ui \
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
    ui/class_selector.ui \
    ui/engrave_selector.ui

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
