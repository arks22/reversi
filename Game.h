#ifndef ___Class_Game
#define ___Class_Game

#include <vector>

#include "Board.h"

using namespace std;
using namespace chrono;


class Game{
  bool isBlackHuman, isWhiteHuman;
  int blackLevel, whiteLevel;

  int whiteStone,blackStone; //石の数
  int currentColor;
  int turnCount;
  int boardsNumber;

  double allottedTime; //持ち時間
  double whiteTime, blackTime; //それぞれの残り時間
  system_clock::time_point startTime, finishTime; //計測開始時間、終了時間
  double turnTime; //このターン消費した時間

  vector<Board> boards;

public:
  Game();

  void putStone(int x, int y);
  void nextTurn(Board board);

  bool isPass();
  bool isTurnOver(Board board, int x, int y);
  bool isOver(Board board);
  bool isPlayerHuman(int color);
  bool isBlackHuman();
  bool isWhiteHuman();

  int getComputerLevel(int color);
  int getTurnCount();
  int getCurrentColor();
  int getBoardsNumber();

  void gameOver(Board board);

  Board undo();
  Board redo();

  void startClock();
  void finishClock();

  double getBlackTime();
  double getWhiteTime();

  void remember(Board board);

private:
  void forget();

};

#endif
