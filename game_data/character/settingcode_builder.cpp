#include "settingcode_builder.h"
#include "game_data/character/engrave/engrave_manager.h"
#include <utility>

QString SettingcodeBuilder::buildSettingCode(QList<Ability> abilities, QList<SetEffect> setEffects, QList<PairEngraveValue> engraves)
{
    return buildAbilityCode(abilities) + buildSetEffectCode(setEffects) + buildEngraveCode(engraves);
}

QList<Ability> SettingcodeBuilder::parseAbility(QString settingCode)
{
    QList<Ability> result;

    int startIndex = settingCode.indexOf("A");
    int endIndex = settingCode.indexOf("S");
    if (startIndex == -1 || endIndex == -1)
        return result;

    startIndex += 1;
    QString abilityCode = settingCode.sliced(startIndex, endIndex - startIndex);
    for (const QChar& ability : abilityCode)
    {
        result.append(static_cast<Ability>(ability.digitValue()));
    }

    return result;
}

QList<SetEffect> SettingcodeBuilder::parseSetEffect(QString settingCode)
{
    QList<SetEffect> result;

    int startIndex = settingCode.indexOf("S");
    int endIndex = settingCode.indexOf("C");
    if (startIndex == -1 || endIndex == -1)
        return result;

    startIndex += 1;
    QString setEffectCode = settingCode.sliced(startIndex, endIndex - startIndex);
    for (const QChar& setEffect : setEffectCode)
    {
        result.append(static_cast<SetEffect>(setEffect.digitValue()));
    }

    return result;
}

QList<QList<PairEngraveValue>> SettingcodeBuilder::parseClassEngrave(QString settingCode)
{
    // index + 1 == level
    QList<QList<PairEngraveValue>> classEngraves(3);

    int startIndex, endIndex;
    QString classEngraveCode, classEngraveLevelCode;

    // slice class engrave code
    startIndex = settingCode.indexOf("C");
    endIndex = settingCode.indexOf("CL");
    if (startIndex == -1 || endIndex == -1)
        return classEngraves;
    startIndex += 1;
    classEngraveCode = settingCode.sliced(startIndex, endIndex - startIndex);
    // slice class engrave level code
    startIndex = settingCode.indexOf("CL");
    endIndex = settingCode.indexOf("N");
    if (startIndex == -1 || endIndex == -1)
        return classEngraves;
    startIndex += 2;
    classEngraveLevelCode = settingCode.sliced(startIndex, endIndex - startIndex);

    // parse
    EngraveManager* pEngraveManager = EngraveManager::getInstance();
    for (int i = 0; i < classEngraveLevelCode.size(); i++)
    {
        int engraveCode = classEngraveCode.sliced(i * 3, 3).toInt();
        int engraveLevel = classEngraveLevelCode[i].digitValue();
        QString engrave = pEngraveManager->getEngraveByCode(engraveCode);

        classEngraves[engraveLevel - 1].append({engrave, engraveLevel});
    }

    return classEngraves;
}

QList<QList<PairEngraveValue>> SettingcodeBuilder::parseNormalEngrave(QString settingCode)
{
    QList<QList<PairEngraveValue>> normalEngraves(3);

    int startIndex, endIndex;
    QString normalEngraveCode, normalEngraveLevelCode;

    // slice normal engrave code
    startIndex = settingCode.indexOf("N");
    endIndex = settingCode.indexOf("NL");
    if (startIndex == -1 || endIndex == -1)
        return normalEngraves;
    startIndex += 1;
    normalEngraveCode = settingCode.sliced(startIndex, endIndex - startIndex);
    // slice normal engrave level code
    startIndex = settingCode.indexOf("NL");
    endIndex = settingCode.size();
    if (startIndex == -1)
        return normalEngraves;
    startIndex += 2;
    normalEngraveLevelCode = settingCode.sliced(startIndex, endIndex - startIndex);

    EngraveManager* pEngraveManager = EngraveManager::getInstance();
    for (int i = 0; i < normalEngraveLevelCode.size(); i++)
    {
        int engraveCode = normalEngraveCode.sliced(i * 3, 3).toInt();
        int engraveLevel = normalEngraveLevelCode[i].digitValue();
        QString engrave = pEngraveManager->getEngraveByCode(engraveCode);

        normalEngraves[engraveLevel - 1].append({engrave, engraveLevel});
    }

    return normalEngraves;
}

QString SettingcodeBuilder::buildAbilityCode(const QList<Ability>& abilities)
{
    QString abilityCode;

    // 목걸이 [0], [1]
    // 귀걸이 [2], [3]
    // 반지 [4], [5]
    for (int i = 0; i < 6; i += 2)
    {
        int value1 = static_cast<int>(abilities[i]);
        int value2 = static_cast<int>(abilities[i + 1]);
        // 부위별 오름차순 정렬
        if (value1 > value2)
            std::swap(value1, value2);
        abilityCode += QString::number(value1);
        abilityCode += QString::number(value2);
    }

    return "A" + abilityCode;
}

QString SettingcodeBuilder::buildSetEffectCode(const QList<SetEffect>& setEffects)
{
    QString setEffectCode;

    QList<int> setEffectNumbers;
    for (const SetEffect& setEffect : setEffects)
    {
        setEffectNumbers.append(static_cast<int>(setEffect));
    }
    // 오름차순정렬
    std::sort(setEffectNumbers.begin(), setEffectNumbers.end());
    for (const int& setEffectNumber : setEffectNumbers)
    {
        setEffectCode += QString::number(setEffectNumber);
    }

    return "S" + setEffectCode;
}

QString SettingcodeBuilder::buildEngraveCode(const QList<PairEngraveValue>& engraves)
{
    EngraveManager* pEngraveManager = EngraveManager::getInstance();

    // 각인명 -> code로 변환하여 추가 후 오름차순 정렬
    QList<int> classEngraveCodes;
    QList<int> normalEngraveCodes;
    for (const PairEngraveValue& pairEngraveValue : engraves)
    {
        if (pEngraveManager->isClassEngrave(pairEngraveValue.first))
            classEngraveCodes.append(pEngraveManager->getEngraveCode(pairEngraveValue.first));
        else
            normalEngraveCodes.append(pEngraveManager->getEngraveCode(pairEngraveValue.first));
    }
    std::sort(classEngraveCodes.begin(), classEngraveCodes.end());
    std::sort(normalEngraveCodes.begin(), normalEngraveCodes.end());

    // 각인명 - 각인레벨 mapping
    QMap<QString, int> mapEngraveLevel;
    for (const PairEngraveValue& pairEngraveValue : engraves)
    {
        mapEngraveLevel[pairEngraveValue.first] = pairEngraveValue.second / 5;
    }

    // 코드생성
    QString classEngraveCode, classEngraveLevelCode;
    QString normalEngraveCode, normalEngraveLevelCode;
    for (const int& engraveCode : classEngraveCodes)
    {
        classEngraveCode += QString::number(engraveCode);
        classEngraveLevelCode += QString::number(mapEngraveLevel[pEngraveManager->getEngraveByCode(engraveCode)]);
    }
    for (const int& engraveCode : normalEngraveCodes)
    {
        normalEngraveCode += QString::number(engraveCode);
        normalEngraveLevelCode += QString::number(mapEngraveLevel[pEngraveManager->getEngraveByCode(engraveCode)]);
    }

    return "C" + classEngraveCode +
           "CL" + classEngraveLevelCode +
           "N" + normalEngraveCode +
           "NL" + normalEngraveLevelCode;
}
