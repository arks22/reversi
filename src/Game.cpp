#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <chrono>
#include <unordered_map>
#include <unistd.h>
#include <ncurses.h>
#include <locale.h>

#include "Game.h"

#define BLACK_STONE 10
#define WHITE_STONE 11
#define LEGAL_POINT 12
#define BOARD       13
#define DEFAULT     1

#define HOVERED_BLACK 20
#define HOVERED_WHITE 21
#define HOVERED_NONE  22

using namespace std;

extern int windowHight, windowWidth; //画面サイズのグローバル変数
extern int centerWidth;

Game::Game(){
  int mode = 1;
  int level = 1;

  setlocale(LC_ALL,""); //UTF-8対応
  initscr();

  noecho();
  start_color();
  keypad(stdscr,TRUE); //十字キー入力など特殊な入力をサポート
  curs_set(0); //カーソルを表示しない

  init_pair(BLACK_STONE, COLOR_BLACK, COLOR_GREEN);
  init_pair(WHITE_STONE, COLOR_WHITE, COLOR_GREEN);
  init_pair(LEGAL_POINT, COLOR_YELLOW, COLOR_GREEN);
  init_pair(BOARD, COLOR_BLACK, COLOR_GREEN);
  init_pair(DEFAULT, COLOR_WHITE, COLOR_BLACK);
  init_pair(HOVERED_BLACK, COLOR_BLACK, COLOR_BLUE);
  init_pair(HOVERED_WHITE, COLOR_WHITE, COLOR_BLUE);
  init_pair(HOVERED_NONE, COLOR_BLACK, COLOR_BLUE);

  getmaxyx(stdscr, windowHight, windowWidth);
  centerWidth = windowWidth / 2;

  int key;
  while(key != '\n'){
    mvprintw(2 * mode + 1, centerWidth - 20, ">>");
    mvprintw(1,centerWidth - 22,"--------------- SELECT A MODE ---------------");
    mvprintw(3,centerWidth - 15,"         YOU  VS  COMPUTER");
    mvprintw(5,centerWidth - 15,"YOU(player1)  VS  YOU(player2)");
    mvprintw(7,centerWidth - 15,"    COMPUTER  VS  COMPUTER");
    key = getch();
    mvprintw(2 * mode + 1, centerWidth - 20, "  "); //">>"があったところを消す
    refresh();
    switch(key){
      case KEY_UP:
        if(mode > 1) mode--;
        break;
      case KEY_DOWN:
        if(mode < 3) mode++;
        break;
    }
  }
  mvprintw(2 * mode + 1, centerWidth - 20, ">>");

  char str[10];
  echo();
  curs_set(1); //カーソルを表示
    mvprintw(9,centerWidth - 22,"------------- PUT ALLOTTED TIME -------------");
  while(true){
    mvprintw(11,centerWidth - 5,"TIME: ");
    refresh();
    getstr(str);
    allottedTime = atoi(str);
    if(30 <= allottedTime && allottedTime < 100000){
      break;
    }else{
      mvprintw(13,centerWidth - 27,"WARNING: Invalid input. Input a number between 30 and 99999.");
      mvprintw(11,centerWidth,"                                              ");
    }
  }
  noecho();
  curs_set(0);

  switch(mode){
    case 1:
      mvprintw(17, centerWidth - 8, ">>");
      key = 0;
      isBlackHuman = true; //初期値
      isWhiteHuman = false;
      while(key != '\n'){
        mvprintw(15,centerWidth - 22,"------------- SELECT YOUR COLOR -------------");
        mvprintw(17 ,centerWidth - 4, "BLACK");
        mvprintw(19 ,centerWidth - 4, "WHITE");
        key = getch();
        refresh();
        switch(key){
          case KEY_UP:
            mvprintw(17, centerWidth - 8, ">>");
            mvprintw(19, centerWidth - 8, "  ");
            isBlackHuman = true;
            isWhiteHuman = false;
            break;
          case KEY_DOWN:
            mvprintw(17, centerWidth - 8, "  ");
            mvprintw(19, centerWidth - 8, ">>");
            isBlackHuman = false;
            isWhiteHuman = true;
            break;
        }
      }

      key = 0;
      while(key != '\n'){
        mvprintw(level + 24, centerWidth - 10, ">>");
        mvprintw(23, centerWidth - 22,"----------- SELECT COMPUTER LEVEL -----------");
        mvprintw(25, centerWidth - 5, "Level 1");
        mvprintw(26, centerWidth - 5, "Level 2");
        mvprintw(27, centerWidth - 5, "Level 3");
        mvprintw(28, centerWidth - 5, "Level 4");
        mvprintw(29, centerWidth - 5, "Level 5");
        key = getch();
        mvprintw(level + 24, centerWidth - 10, "  ");
        refresh();
        if(key == KEY_UP){
          if(level > 1) level--;
        }else if(key == KEY_DOWN){
            if(level < 5) level++;
        }
      }

      if(!isBlackHuman) blackLevel = level;
      if(!isWhiteHuman) whiteLevel = level;
      break;

    case 2:
      isBlackHuman = true;
      isWhiteHuman = true;
      break;

    case 3:
      isBlackHuman = false;
      isWhiteHuman = false;
      for(int i = 0; i<2; i++){
        level = 1;
        key = 0;
        while(key != '\n'){
          if(i==0){
            mvprintw(16, centerWidth - 22,"------- SELECT COMPUTER LEVEL (Black) -------");
          }else{
            mvprintw(25, centerWidth - 22,"------- SELECT COMPUTER LEVEL (White) -------");
          }
          mvprintw(level + 17 + i * 9, centerWidth - 10, ">>");
          mvprintw(18 + i * 9, centerWidth - 5, "Level 1");
          mvprintw(19 + i * 9, centerWidth - 5, "Level 2");
          mvprintw(20 + i * 9, centerWidth - 5, "Level 3");
          mvprintw(21 + i * 9, centerWidth - 5, "Level 4");
          mvprintw(22 + i * 9, centerWidth - 5, "Level 5");
          key = getch();
          mvprintw(level + 17 + i * 9, centerWidth - 10, "  ");
          refresh();
          switch(key){
            case KEY_UP:
              if(level > 1) level--;
              break;
            case KEY_DOWN:
              if(level < 5) level++;
              break;
          }
        }
        mvprintw(level + 17 + i * 9, centerWidth - 10, ">>");
        if(i==0){
          blackLevel = level;
        }else{
          whiteLevel = level;
        }
      }
      break;
  }

  whiteTime = allottedTime;
  blackTime = allottedTime;
  currentColor = -1;
  turnCount = 1;
  boardsNumber = 1;
  Game::startClock();
}



void Game::nextTurn(Board board){
  Game::finishClock();//計測終了
  Game::startClock();//計測開始
  currentColor *= -1;
  if(turnCount < boardsNumber){
    Game::forget();
  }
  turnCount++;
}


bool Game::isOver(Board board){
  if(board.isPass(currentColor) && board.isPass(currentColor* -1)){
    return true;
  }else if(blackTime <= 0){
    blackTime = 0;
    return true;
  }else if(whiteTime <= 0){
    whiteTime = 0;
    return true;
  }else{
    return false;
  }
}


int Game::getTurnCount(){
  return turnCount;
}

int Game::getComputerLevel(int color){
  if(color == -1){
    return blackLevel;
  }else{
    return whiteLevel;
  }
}

int Game::getCurrentColor(){
  return currentColor;
}

int Game::getBoardsNumber(){
  return boardsNumber;
}

double Game::getBlackTime(){
  return blackTime;
}

double Game::getWhiteTime(){
  return whiteTime;
}

void Game::startClock(){
  startTime = system_clock::now(); //時間計測開始
}


void Game::finishClock(){
  finishTime = system_clock::now(); //時間計測開始
  turnTime = duration_cast<seconds>( finishTime - startTime ).count();
  if(currentColor == -1){
    blackTime -= turnTime;
  }else{
    whiteTime -= turnTime;
  }
}

bool Game::isTurnOver(Board board, int x, int y){
  if(Game::isOver(board)){
    Game::gameOver(board);
    return true;
  }else if(y == 0){
    return false;
  }else if(board.isLegal(x,y,currentColor)){
    return true;
  }else{
    mvprintw(17, centerWidth - 19, "WARNING: You can't put it there.");
    return false;
  }
}


bool Game::isPlayerHuman(int color){
  if(color == -1){
    return isBlackHuman;
  }else{
    return isWhiteHuman;
  }
}

void Game::gameOver(Board board){
  string winner;
  if(allottedTime < whiteTime){
    winner = "black";
  }else if(allottedTime < blackTime){
    winner = "white";
  }else{
    winner = board.getVictory() == -1 ? "BLACK" : "WHITE";
  }
  mvprintw(17,centerWidth - 22,"---------------- GAME OVER ----------------");
  mvprintw(19,centerWidth - 7,"%s WINS", winner.c_str());
  mvprintw(21,centerWidth - 11,"Press any key to exit");
  mvprintw(13, centerWidth + 8, "          ");
  mvprintw(4,centerWidth - 15,"                   ");
  int key = getch();
  endwin();
  exit(0);
}


Board Game::undo(){
  turnCount -= 2;
  return boards.at(turnCount - 1);
}

Board Game::redo(){
  turnCount += 2;
  return boards.at(turnCount - 1);
}

void Game::remember(Board board){
  boardsNumber++;
  boards.push_back(board);
}

void Game::forget(){
  boards.erase(boards.begin() + turnCount, boards.end());
}


