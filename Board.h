#ifndef ___Class_Board
#define ___Class_Board

#include <vector>
#include <array>

using namespace std;


class Board{
  vector<vector<int>> board;

public:
  Board();

  bool isLegal(const int x, const int y, const int color);
  bool isPass(const int color);
  bool isGameOver(const int color);

  int  countStone(const int value);
  int  getVictory(); //勝者を返す

  vector<array<int,2>> getLegalMove(const int color);//合法な手を全て返す
  vector<vector<int>>  getBoardVector(); //全てのboardを返す
  vector<array<int,2>> setShouldReverseStone(const int x, const int y, const int color);

  void putStone(const int x, const int y, const int color); //ある場所に石を置き、それに伴って周りの石をひっくり返す一連の動作を行う

private:
  void reverse(const int x, const int y); //与えられた位置の石を一つだけひっくり返す

};

#endif
