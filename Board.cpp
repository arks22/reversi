#include <iostream>
#include <vector>
#include <array>
#include "Board.h"

using namespace std;

Board::Board(){
  board = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,-1,0,0,0,0},
    {0,0,0,0,-1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
  };
  // 1 -> white
  //-1 -> black
 
}


int Board::countStone(const int color){
  int num = 0;

  for(int i=1; i<9; i++){
    for(int j=1; j<9; j++){
      if(board.at(j).at(i) == color){
        num++;
      }
    }
  }

  return num;
}




bool Board::isLegal(const int x, const int y, const int color){
  if(x<0 || x>8 || y<0 || y>8){
    return false;
  }

  if(board.at(x).at(y) != 0){
    return false;
  }

  vector<array<int,2>> reverseStone = setShouldReverseStone(x,y,color);
  size_t reverseStoneSize = reverseStone.size();

  if(reverseStoneSize == 0){
    return false;
  }else{
    return true;
  }
}



vector<array<int,2>> Board::getLegalMove(const int color){//合法な手を全て返す
  vector<array<int,2>> legalMove;
  for(int i=1; i<=8; i++){
    for(int j=1; j<=8; j++){
      if(board.at(i).at(j) == 0 && isLegal(i,j,color) == true){
        legalMove.push_back({i,j});
      }
    }
  }
  return legalMove;
}



bool Board::isPass(const int color){//パスかどうかpassならtrue
  for(int i=1; i<=8; i++){
    for(int j=1; j<=8; j++){
      if(board.at(i).at(j) == 0 && isLegal(i,j,color) == true){
        return false; //何も置いてない、かつ合法な場所が一つでもあったらfalseを返す
      }
    }
  }
  return true;
}


bool Board::isGameOver(const int color){
  if(Board::isPass(color)){
    if(Board::isPass(color * -1)) return true;
  }
  return false;
}


void Board::putStone(const int x, const int y, const int color){

  vector<array<int,2>> reverseStone = setShouldReverseStone(x,y,color);

  size_t reverseStoneSize = reverseStone.size();

  int reverse_x, reverse_y;
  for(int i=0; i<reverseStoneSize; i++){ //shouldReverseに格納された値に当たるboardをreverse
    reverse_x = reverseStone.at(i).at(0);
    reverse_y = reverseStone.at(i).at(1);
    reverse(reverse_x, reverse_y);
  }

  board.at(x).at(y) = color;
}



vector<array<int,2>> Board::setShouldReverseStone(const int x, const int y, const int color){//ある位置(x,y)にcolorの色の石を置くとき、ひっくり返るべき石の組み合わせを返す
  vector<array<int,2>> reverseStone;
  vector<array<int,2>> vec = {};
  int currentGridColor;

  for(int i=x-1;; i--){ //左 vecを必ず初期化するためにforループの終了条件を設定しない
    currentGridColor = board.at(i).at(y);
    if(currentGridColor != color * -1){//何も置いていない、または今の色の石に当たったらbreak
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end()); //今の色の石に当たったらvectorを連結
      }; 
      vec = {}; //vecを初期化
      break;
    }
    vec.push_back({i,y});
  }

  for(int i=x-1, j=y-1;; i--,j--){ //左上
    currentGridColor = board.at(i).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({i,j});
  }

  for(int j=y-1;;j--){ //上
    currentGridColor = board.at(x).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({x,j});
  }

  for(int i=x+1, j=y-1;;i++, j--){ //右上
    currentGridColor = board.at(i).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({i,j});
  }

  for(int i=x+1;;i++){ //右
    currentGridColor = board.at(i).at(y);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({i,y});
  }

  for(int i=x+1, j=y+1;;i++,j++){ //右下
    currentGridColor = board.at(i).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({i,j});
  }

  for(int j=y+1;;j++){ //下
    currentGridColor = board.at(x).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({x,j});
  }

  for(int i=x-1, j=y+1;; i--,j++){ //左下
    currentGridColor = board.at(i).at(j);
    if(currentGridColor != color * -1){
      if (currentGridColor == color){
        reverseStone.insert(reverseStone.end(), vec.begin(), vec.end());
      }; 
      vec = {};
      break;
    }
    vec.push_back({i,j});
  }

  return reverseStone;
}



int Board:: getVictory(){
  int white = countStone(1);
  int black = countStone(-1);

  if(white > black){
    return 1;
  }else if(white < black){
    return -1;
  }else{
    return 0;
  }
}



void Board::reverse(const int x, const int y){
  board.at(x).at(y) *= -1;
}



vector<vector<int>> Board::getBoardVector(){
  return board;
}
