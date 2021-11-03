# Tetris

2020-1 컴퓨팅설계및실험 수업에서 제공된 baseline 코드를 기반으로 주요 함수들을 구현했습니다.

주요 구현 함수

int CheckToMove()
  : 블록이 움직일 좌표와 회전 횟수를 이용해 현재 블록이 필드의 해당 위치로 이동 가능한지 여부를 체크.
  
void DrawChange()
  : 블록이 이동할 좌표와 회전 횟수, command를 이용해 현재 위치의 블록을 지우고, 필드의 새 위치에 블록을 그림.

void BlockDown()
  : 현재 블록을 매초마다 한 칸씩 아래로 떨어뜨림.
    블록이 더 이상 내려갈 수 없을 경우 블록을 필드에 쌓고 완전히 채워진 line을 지움.
    
void AddBlockToField()
  : 현재 위치의 현재 블록을 필드에 쌓음.
  
int DeleteLine()
  : 완전히 채워진 line을 찾아 지우고 지운 line의 개수에 대한 점수를 return

void createRankList()
  : 테트리스 프로그램 시작 시 rank.txt로부터 랭킹 정보를 입력받아 연결 리스트의 형태로 자료를 구축.
  
void rank()
  : 정수 2개, x, y(x<=y)를 입력 받고 랭킹 정보(x위 ~ y위)를 화면에 출력.
  
void writeRankFile()
  : 추가된 랭킹 정보가 있으면 새 정보를 rank.txt에 저장하고, 추가된 정보가 없으면 그대로 프로그램을 종료.
  
void newRank()
  : 게임 종료(game over)시 사용자의 이름을 입력 받고, 사용자의 이름과 점수를 자료 구조에 추가 및 저장.

void DrawShadow()
  : 현재 위치에 대해 그림자를 필드에 그림.

int recommand()
  : 사용자에게 좋은 점수를 얻을 수 있는 블록의 위치를 계산.
    현재와 다음 블록의 모양, 회전 수, 필드의 상태, 얻을 수 있는 점수 등을 고려.
