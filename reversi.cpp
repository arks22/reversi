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

void humanMove(Board board, int currentColor, int& x, int& y);

void drowTable(Game game, Board board, int currentColor); //盤面を描画する関数

void printBlueCursor(Board board, int currentColor, int x, int y);
void printPoint(Board board, int currentColor, int x, int y);
void printUndoRedo(int x);


void computerMove(int strength, Board board, int computerColor, int turnCount, int& x, int& y);
int minimax(Board board, int limitDepth, int color, bool isMyTurn);
int fullSearch(Board board, int color, bool isMyTurn);
int eval(Board board, int color);


int windowHight, windowWidth; //画面サイズのグローバル変数
int centerWidth;

char alphabet[9] = "ABCDEFG";

int main(){
  Board board;
  Game game;
  int x,y;
  
  while(!game.isOver(board)){
    drowTable(game, board, game.getCurrentColor());

    if(board.isPass(game.getCurrentColor())){
      mvprintw(1, centerWidth - 19, "********* PASS *********");
      mvprintw(2, centerWidth - 19, "Press any key to continue");
      getch();
      refresh();
      game.nextTurn(board);
      continue;
    }

    if(game.getTurnCount() > 1) mvprintw(16, centerWidth - 19, "Previous player putted %c%d",alphabet[x-1],y);

    do{
      game.remember(board);

      if(game.isCurrentPlayerHuman()){
        humanMove(board, game.getCurrentColor(), x,y);

        //UNDO,REDO機能
        if(y == 0){
          if(x == 0){
            if(game.getTurnCount() > 2){
              board = game.undo();
            }else{
              mvprintw(16,centerWidth - 19, "WARNING: Cannot UNDO");
              mvprintw(17,centerWidth - 19, "Press any key to continue");
              refresh();
              getch();
            }
          }else{
            if(game.getBoardsNumber() - game.getTurnCount() >= 2){
              board = game.redo();
            }else{
              mvprintw(16,centerWidth - 19, "WARNING: Cannot REDO");
              mvprintw(17,centerWidth - 19, "Press any key to continue");
              refresh();
              getch();
            }
          }
          drowTable(game, board, game.getCurrentColor());
        }
      }else{
        computerMove(game.getCurrentPlayerLevel(), board, game.getCurrentColor(), game.getTurnCount(), x, y);
      }
    }while(!game.isTurnOver(board,x,y));

    board.putStone(x,y,game.getCurrentColor());
    game.nextTurn(board);
  }
  drowTable(game, board,game.getCurrentColor());
  game.gameOver(board);
}



void humanMove(Board board, int currentColor, int& x, int& y){
  int key = 0;
  x = 1;
  y = 1;
  //curs_set(1); //カーソルを表示

  mvprintw(2,centerWidth - 19,"Choose the place to put");
  printBlueCursor(board,currentColor,x,y);

  while(key != '\n'){ 
    key = getch();

    if(y == 0){
      mvprintw(4,centerWidth - 15,"UNDO");
      mvprintw(4,centerWidth - 7,"REDO");
    }else{
      printPoint(board,currentColor,x,y);
    }

    switch(key){
      case 'q':
        curs_set(1);
        endwin();
        exit(0); //quit

      case KEY_UP:
        if(y > 1){
          y--;
        }else if(y == 1){
          y = 0;
          x = 0;
        }
        break;

      case KEY_DOWN:
        if(y == 0){
          x = 1;
          y = 1;
        }else if(y < 8){
          y++;
        }
        break;

      case KEY_RIGHT:
        if(y == 0 && x == 0){
          x = 1; 
        }else if(x < 8){
          x++;
        }
        break;

      case KEY_LEFT:
        if(x == 1 && y == 0){
          x = 0;
        }else if(x > 1){
          x--;
        }
        break;
    }

    if(y == 0){
      printUndoRedo(x);
    }else{
      printBlueCursor(board,currentColor,x,y);
    }
  }

  curs_set(0); //カーソルを非表示
}


void drowTable(Game game, Board board, int currentColor){ //盤面を描画する関数
  clear();
  vector<vector<int>> boardVector = board.getBoardVector();

  mvprintw(4,centerWidth - 15,"UNDO");
  mvprintw(4,centerWidth - 7,"REDO");
 
  for(int i=1; i<=8; i++){
    attron(COLOR_PAIR(BOARD));
    mvprintw(i+6, centerWidth - 17, "|");
    for(int j=1; j<=8; j++){
      switch (boardVector.at(j).at(i)){
        case -1:
          attron(COLOR_PAIR(BLACK_STONE));
          mvprintw(i+6,centerWidth + j * 2 - 18, "●");//black
          break;
        case 1:
          attron(COLOR_PAIR(WHITE_STONE));
          mvprintw(i+6,centerWidth + j * 2 - 18, "●");//white
          break;
        case 0:
          attron(COLOR_PAIR(BOARD));
          if(board.isLegal(j,i,currentColor)){
            mvprintw(i+6,centerWidth + j * 2 - 18, "*");
          }else{
            mvprintw(i+6,centerWidth + j * 2 - 18, " ");
          }
          break;
      }
      attron(COLOR_PAIR(BOARD));
      mvprintw(i+6, centerWidth + j * 2 - 17, "|");
    }
  }
  attron(COLOR_PAIR(DEFAULT));

  for(int i=4; i<15; i++){
    mvprintw(i,centerWidth + 3, "│"); //tatesenn
  }

  mvprintw(6, centerWidth + 8, "○: %d", board.countStone(-1)); 
  mvprintw(7, centerWidth + 8, "●: %d", board.countStone(1));

  mvprintw(9, centerWidth + 8, "Time:");
  mvprintw(10, centerWidth + 9, "Black:%5.0lf(s)", game.getBlackTime()); 
  mvprintw(11, centerWidth + 9, "White:%5.0lf(s)", game.getWhiteTime());

  mvprintw(13, centerWidth + 8, "Turn: %s", (currentColor == -1 ? "○" : "●"));

  refresh();
}


void printBlueCursor(Board board, int currentColor, int x, int y){
  vector<vector<int>> boardVector = board.getBoardVector();
  switch (boardVector.at(x).at(y)){
    case -1:
      attron(COLOR_PAIR(HOVERED_BLACK));
      mvprintw(y+6,centerWidth + x * 2 - 18, "●");//black
      break;
    case 1:
      attron(COLOR_PAIR(HOVERED_WHITE));
      mvprintw(y+6,centerWidth + x * 2 - 18, "●");//white
      break;
    case 0:
      attron(COLOR_PAIR(HOVERED_NONE));
      if(board.isLegal(x,y,currentColor)){
        mvprintw(y+6,centerWidth + x * 2 - 18, "*");
      }else{
        mvprintw(y+6,centerWidth + x * 2 - 18, " ");
      }
      break;
  }
  attron(COLOR_PAIR(DEFAULT));
}


void printPoint(Board board, int currentColor, int x, int y){
  vector<vector<int>> boardVector = board.getBoardVector();
  switch (boardVector.at(x).at(y)){
    case -1:
      attron(COLOR_PAIR(BLACK_STONE));
      mvprintw(y+6,centerWidth + x * 2 - 18, "●");//black
      break;
    case 1:
      attron(COLOR_PAIR(WHITE_STONE));
      mvprintw(y+6,centerWidth + x * 2 - 18, "●");//white
      break;
    case 0:
      attron(COLOR_PAIR(BOARD));
      if(board.isLegal(x,y,currentColor)){
        mvprintw(y+6,centerWidth + x * 2 - 18, "*");
      }else{
        mvprintw(y+6,centerWidth + x * 2 - 18, " ");
      }
      break;
  }
  attron(COLOR_PAIR(DEFAULT));
}


void printUndoRedo(int x){
  if(x == 0){
    attron(COLOR_PAIR(HOVERED_BLACK));
  }else{
    attron(COLOR_PAIR(DEFAULT));
  }
  mvprintw(4,centerWidth - 15,"UNDO");
  if(x == 0){
    attron(COLOR_PAIR(DEFAULT));
  }else{
    attron(COLOR_PAIR(HOVERED_BLACK));
  }
  mvprintw(4,centerWidth - 7,"REDO");
  attron(COLOR_PAIR(DEFAULT));
}
