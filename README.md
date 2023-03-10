# QT_LoaManager
Lostark OpenAPI를 활용한 편의기능 제공 앱
## 개발 환경
- **Compiler** : MinGW 11.2.0 64-bit for c++
- **Framework** : Qt 6.4.0
- **Database** : mongocxx-driver r3.7.0
## 기능 (지속적으로 추가중입니다.)
### 시세 탭
- **[거래소/경매장 검색](https://github.com/Wseop/QT_LoaManager/tree/main/function/smart_search)** - 옵션 선택을 최소화할 수 있도록 카테고리 별로 거래소/경매장 검색 기능 제공
  - **각인서** : 전체 '전설' 각인서의 가격 정보 제공
  - **강화 재료** : 명예의 파편, 파괴석, 수호석, 돌파석, 융화재료, 숨결, 정수 가격 정보 제공
  - **보석** : 5~10레벨 멸화, 홍염 가격 정보 제공
  - **악세** : 선택한 옵션으로 악세서리 검색 -> 가능한 모든 각인 조합으로 결과 제공
    > 유물 : 33, 34, 35    
    > 고대 : 34, 35, 36
  - **5레벨 트라이포드** : 선택한 직업의 모든 5레벨 트라이포드 가격 정보와 가격 필터링 기능 제공
- **[컨텐츠 보상](https://github.com/Wseop/QT_LoaManager/tree/main/function/content_reward)**
  - 카오스 던전, 가디언 토벌 진행 시 획득 가능한 재화의 평균치를 제공 
  - 획득한 재화를 골드 가치로 환산한 데이터 제공
    > 1490이상 컨텐츠부터 구현
- **[더보기 손익](https://github.com/Wseop/QT_LoaManager/tree/main/function/raid_profit)**
  - 레이드 더보기로 획득 가능한 재화들의 골드 가치를 계산하여 더보기 손익 제공
## 다운로드 및 실행
- 우측의 [Releases 탭](https://github.com/Wseop/QT_LoaManager/releases)에서 zip파일 다운 및 압축해제 후 exe파일 실행
