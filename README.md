# QT_LoaManager
Lostark OpenAPI를 활용한 편의기능 제공 앱
## 개발 환경
- **Compiler** : MinGW 11.2.0 64-bit for c++
- **Framework** : Qt 6.4.0
- **Database** : mongocxx-driver r3.7.0
## 기능
### 캐릭터 탭
- **[캐릭터 조회](https://github.com/Wseop/QT_LoaManager/tree/main/function/character_search)**
- **[캐릭터 순위](https://github.com/Wseop/QT_LoaManager/tree/main/function/character_ranking)** - 전체 혹은 직업별 아이템 레벨 순위
- **[직업별 세팅 순위](https://github.com/Wseop/QT_LoaManager/tree/main/function/setting_ranking)** - 직업별 특성, 각인, 장비 세팅 추이
### 시세 탭
- **[경매 계산기](https://github.com/Wseop/QT_LoaManager/tree/main/function/auction_calculator)** - 레이드 경매 최적 입찰가 제공
- **[트라이포드 가격 조회](https://github.com/Wseop/QT_LoaManager/tree/main/function/quotation/tripod)** - 직업별 전체 트라이포드 가격 조회
- **[강화 재료 가격 조회](https://github.com/Wseop/QT_LoaManager/tree/main/function/quotation/reforge)** - 강화 재료 및 명예의 파편 주머니 효율 제공
- **[어빌리티 스톤 가격 조회](https://github.com/Wseop/QT_LoaManager/tree/main/function/quotation/abilitystone)** - 선택한 각인으로 가능한 모든 조합별 어빌리티 스톤 가격 제공
- **[더보기 손익](https://github.com/Wseop/QT_LoaManager/tree/main/function/raid)** - 모든 레이드 더보기 보상 손익 제공
- **[컨텐츠 보상](https://github.com/Wseop/QT_LoaManager/tree/main/function/contents_reward)** - 카오스 던전, 가디언 토벌 시 획득할 수 있는 보상의 평균량을 제공, 골드 가치로 환산한 데이터 추가 제공
## 다운로드 및 실행
- 우측의 [Releases 탭](https://github.com/Wseop/QT_LoaManager/releases)에서 zip파일 다운 및 압축해제 후 exe파일 실행
