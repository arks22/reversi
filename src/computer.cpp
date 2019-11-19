#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <chrono>
#include <unordered_map>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>

#include "Board.h"

using namespace std;

extern int centerWidth;

void computerMove(int strength, Board board, int computerColor, int turnCount, int& x, int& y);
int minimax(Board board, int limitDepth, int color, bool isMyTurn);
int fullSearch(Board board, int color, bool isMyTurn);
int eval(Board board, int color);



void computerMove(int strength, Board board, int computerColor, int turnCount, int& x, int& y){ 
  vector<array<int,2>> moveVector = board.getLegalMove(computerColor);
  size_t moveVectorSize = moveVector.size();
  array<int,2> move;

  mvprintw(2,centerWidth - 19,"Computer is thinking...");
  refresh();

  int limitDepth = strength; //強さ = 読みの深さ
  int score;
  int maxScore = -1000; 
  for(int i=0; i<moveVectorSize; i++){
    Board hypoBoard = board;
    hypoBoard.putStone(moveVector.at(i).at(0), moveVector.at(i).at(1), computerColor); //仮に手を打つ
    if(turnCount < 52){
      score = minimax(hypoBoard, limitDepth, computerColor * -1, false );//minimaxで探索
    }else{
      score = fullSearch(hypoBoard, computerColor * -1, false); //全探索
    }

    //cout << "eval:" << score << " ,";
    if(score > maxScore){
      maxScore = score;
      move = moveVector.at(i);
      //cout << "更新！(" << moveVector.at(i).at(0) << "," <<  moveVector.at(i).at(1) << ")" << endl << endl;
    }
  }
  x = move.at(0);
  y = move.at(1);
}



int minimax(Board board, int limitDepth, int color, bool isMyTurn){
  vector<array<int,2>> moveVector = board.getLegalMove(color);
  size_t moveVectorSize = moveVector.size();

  if(moveVectorSize == 0){//passだったら色を反転して次
    color *= -1;
    isMyTurn = !isMyTurn;
    limitDepth--;
    moveVector = board.getLegalMove(color);
    moveVectorSize = moveVector.size();

    if(moveVectorSize == 0){//もう一回パス(game over)だったら
      //cout << "o";
      int winner = board.getVictory();
      if(isMyTurn){
        return (winner == color) ? 10000 : -10000;//自分のターンの場合、勝者=ターンプレイヤーなら勝利
      }else{
        return (winner == color) ? -10000 : 10000;
      }
    }
  }

  if(limitDepth == 0){
    return isMyTurn ? eval(board, color) : eval(board, color * -1); //自分の盤面として評価させる(自分のターンんでなければ反転)　
  }

  int score;
  int returnScore = isMyTurn ? -1000 : 1000; //returnScore -> 最小値か最大値
  for(int i=0; i<moveVectorSize; i++){
    Board hypoBoard = board;//hypo -> 仮定
    hypoBoard.putStone(moveVector.at(i).at(0), moveVector.at(i).at(1), color);
    score = minimax(hypoBoard, limitDepth - 1, color * -1, !isMyTurn );

    if(isMyTurn){
      if(score > returnScore) returnScore = score;//自分のターンの場合、最大値を返す(max)
    }else{
      if(score < returnScore) returnScore = score;//相手のターンの場合、最小値を返す(mini)
    }
  }
  return returnScore;
}



int fullSearch(Board board, int color, bool isMyTurn){ //全探索
  vector<array<int,2>> moveVector = board.getLegalMove(color);
  size_t moveVectorSize = moveVector.size();

  if(moveVectorSize == 0){//passだったら色を反転して次
    color *= -1;
    isMyTurn = !isMyTurn;
    moveVector = board.getLegalMove(color);
    moveVectorSize = moveVector.size();

    if(moveVectorSize == 0){//もう一回パス(game over)だったら石の数をreturn
      //cout << "count:" << board.countStone(color); 
      return board.countStone(color);
    }

  }

  int score;
  int returnScore = isMyTurn ? -1000 : 1000; //returnScore -> 最小値か最大値
  for(int i=0; i<moveVectorSize; i++){
    Board hypoBoard = board;//hypo -> 仮定
    hypoBoard.putStone(moveVector.at(i).at(0), moveVector.at(i).at(1), color);
    score = fullSearch(hypoBoard, color * -1, !isMyTurn);

    if(isMyTurn){
      if(score > returnScore) returnScore = score;//自分のターンの場合、最大値を返す(max)
    }else{
      if(score < returnScore) returnScore = score;//相手のターンの場合、最小値を返す(mini)
    }
  }
  return returnScore;
}



int eval(Board board, int color){ //盤面を評価する関数
  vector<vector<int>> scoreVector = {
    { 50,-30, 4,-3,-3, 4,-30, 50},
    {-30,-25,-1,-5,-5,-1,-25,-30},
    { 4, -1,  2,-1,-1, 2,-1,  4},
    {-3, -5, -1, 0, 0,-1,-5, -3},
    {-3, -5, -1, 0, 0,-1,-5, -3},
    { 4, -1,  2,-1,-1, 2,-1,  4},
    {-30,-25,-1,-5,-5,-1,-25,-30},
    { 50,-30, 4,-3,-3, 4,-30, 50},
  };
  vector<vector<int>> boardVector = board.getBoardVector();
  int score = 0;

  for(int i=1; i<=8; i++){
    for(int j=1; j<=8; j++){
      score += scoreVector.at(i-1).at(j-1) * boardVector.at(i).at(j) * color;
      //なにも置いてなければ0
      //白の評価値を出す場合はscore*1*1 = score, 黒の場合はscore*(-1)*(-1) = score
    }
  }
  //cout << ".";
  return score;
}
