#include "raylib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
namespace cattoEngine{
  enum dir {
    up,
    right,
    down,
    left,
    up_right,
    up_left,
    down_right,
    down_left,
  };
  enum entitytype {
    AIR,
    PLAYER,
    ENEMY,
    STATICENEMY,
    ATTACK,
  };
  using namespace std;
  const int ARRAYSIZE = 100;
  Color ENEMYCOLOR = RED;
  Color PLAYERCOLOR = BLACK;
  Color BACKGROUND = WHITE;
  int screenWidth = 1080;
  int screenHeight = 1080;
  struct returner{
    bool yes = false;
    int arrx = -1;
    int arry = -1;
  };
  struct point{
    int x = -1;
    int y = -1;
  };
  struct doublepoint{
    double x = -1;
    double y = -1;
  };
  class attack{
    public :
      int size;
      doublepoint pos;
      doublepoint bottomright;
      int facing;
      double speed;
      double diagspeed;
      Color attackcolour = BLUE;
      bool friendly;
      Color GetColour(){
        return attackcolour;
      }
      void SetupAttack(point position,int size, int direction, bool isfriendly, double movespeed = 0.5, double diagmovespeed = 0.25){
        pos = {position.x, position.y};
        bottomright = {pos.x + size, pos.y + size};
        facing = direction;
        friendly = isfriendly;
        speed = movespeed;
        diagspeed = diagmovespeed;
      }
  };
  class player{
    public :
    int size = 5;
    doublepoint pos = {ARRAYSIZE / 2 , ARRAYSIZE / 2};
    doublepoint bottomright = {ARRAYSIZE / 2 + size, ARRAYSIZE / 2 + size};
    double speed = 0.5;
    double diagspeed = 0.25;
    int direction = 0;
    //0 - facing up
    //1 - facing right
    //2 - facing down
    //3 - facing left
    int points = 0;
    int hp = 1;
    //the idea here is that its a fraction instead of actual hitpoints
      bool MoveTo(bool barrier[ARRAYSIZE][ARRAYSIZE],double x,double y){
        double xbottom = x + size;
        double ybottom;
        if(x >= ARRAYSIZE || y >= ARRAYSIZE) return false;
        if(y <= 0 || x <= 0) return false;
        if(barrier[int(x)][int(y)]) return false;
        if(xbottom >= ARRAYSIZE || ybottom >= ARRAYSIZE) return false;
        if(ybottom <= 0 || xbottom <= 0) return false;
        if(barrier[int(xbottom)][int(ybottom)]) return false;
        //if its not invalid space
        else {
          return true;
        };
      }
  };
  struct pointbool{
    point pos;
    bool isInArr;
  };
  class game_array{
    public :
      bool barrier[ARRAYSIZE][ARRAYSIZE]; //if there is a barrier at coordinates
      int entity[ARRAYSIZE][ARRAYSIZE];
      bool oci = true;
      bool fullscreened = false;
      player character;
      returner EntityInBorder(){
          returner returned;
          for(int i = 0;i < ARRAYSIZE;i++){
            for(int k = 0;k < ARRAYSIZE;k++){
                if(entity[i][k] && barrier[i][k]){
                  returned.yes = true;
                  returned.arrx = i;
                  returned.arry = k;
                }
            }
          }
          returned.yes = false;
          return returned;
      }
      string StringRange(string in,int start,int end,bool wantOut = false){
        string out;
        int k = 0;
        for(int i = start; i < end; i++){
          out.push_back(in[i]);
          if(wantOut){
            cout<<"Output["<<k<<"] = "<<in[i]<<endl;
          }
          k++;
        }
        return out;
      }
      void ClearArray(){
        for(int i = 0; i < ARRAYSIZE; i++){
          for(int k = 0; k < ARRAYSIZE; k++){
            entity[i][k] = AIR;
            barrier[i][k] = false;
          }
        }
      }
      pointbool FindPlayer(){
        pointbool player;
        player.pos.x = -1;
        player.pos.y = -1;
        player.isInArr = false;
        for(int i = 0; i < ARRAYSIZE; i++){
          for(int k = 0; k < ARRAYSIZE; k++){
              if(entity[k][i] == PLAYER){
                player.pos.x = k;
                player.pos.y = i;
                player.isInArr = true;
              }
          }
        }
        return player;
      }
      void CheckArray(){
        while(EntityInBorder().yes){
          MoveEntity(EntityInBorder().arrx,EntityInBorder().arry + 1,EntityInBorder().arrx, EntityInBorder().arry);
          //moves entity down, probably will cause problems at some point
        }
      }
      void MoveEntity(int FromX, int FromY, int ToX, int ToY){
        if(barrier[ToX][ToY]) return;
        //destroys entity already there, so you can move air to an entity and kill it
        int Stored = entity[FromX][FromY];
        entity[FromX][FromY] = AIR;
        entity[ToX][ToY] = Stored;
        CheckArray();
      }
      void GetData(string fname){
        ifstream in (fname);
        string line;
        string LineArray[4];
        int i = 0;
        if (in.is_open())
        {
          while ( getline (in,line) ){
            cout<<"putting "<<line<<" into LineArray["<<i<<']'<<endl;
            LineArray[i] = line;
            i++;
          }
          in.close();
        }
        else{
          return;
        }
        //layer 0 = amount of stuff in the format borders,enemys
        //layer 1 = coordinates of borders
        //layer 2 = coordinates of enemys
        //layer 3 = player coords

        //layer 0 proccessing

        string BorderStr;
        string EnemyStr = LineArray[0];
        int Borders;
        int Enemys;
        point player;
        vector<point> BorderArr;
        vector<point> EnemyArr;
        int division = LineArray[0].find(',');
        cout<<"file division = "<<division;
        cout<<endl<<LineArray[0]<<endl;
        cout<<LineArray[1]<<endl;
        cout<<LineArray[2]<<endl;
        cout<<LineArray[3]<<endl;
        point tempoint;
        cout<<"Trying to set BorderStr to "<<StringRange(LineArray[0],0,division)<<endl;
        BorderStr.append(StringRange(LineArray[0],0,division,true));
        cout<<"BorderStr = "<<BorderStr<<endl;
        LineArray[0].erase(0,division + 1);
        cout<<"LineArray[0] = "<<LineArray[0]<<endl;
        Borders = stoi(BorderStr);
        cout<<"Border count = "<<Borders<<endl;
        EnemyStr = LineArray[0];
        Enemys = stoi(LineArray[0]);
        cout<<"Enemy count = "<<Enemys<<endl;
        //end layer 0 proccessing
        //start layer 1 proccessing
        cout<<endl<<"LAYER ONE PROCCESSING"<<endl<<endl;
        string Temp1, Temp2;
        for(int i = 0; i < Borders; i++){
          cout<<"LineArray[1] = "<<LineArray[1]<<endl;
          cout<<"Getting LineArray[1] from "<<i * 4<<" to "<<(i * 4) + 2<<endl;
          Temp1 = StringRange(LineArray[1],(i * 4),(i * 4) + 2,true);
          cout<<"Getting LineArray[1] from "<<(i * 4) + 2<<" to "<<(i * 4) + 4<<endl;
          Temp2 = StringRange(LineArray[1],(i * 4) + 2,(i * 4 ) + 4,true);
          // the idea here is that LineArray[1] will look a little something like this
          // 1003
          // ^^^^
          // x  y
          // with no space or anything seperating x and y, to make it easier to code
          tempoint.x = stoi(Temp1);
          cout<<"tempoint.x = "<<tempoint.x<<endl;
          tempoint.y = stoi(Temp2);
          cout<<"tempoint.y = "<<tempoint.y<<endl;
          BorderArr.push_back(tempoint);
        }

        //end layer 1 proccessing
        //start layer 2 proccessing
        cout<<endl<<"LAYER TWO PROCCESSING"<<endl<<endl;
        for(int i = 0; i < Enemys; i++){
          cout<<"LineArray[2] = "<<LineArray[2]<<endl;
          cout<<"Getting LineArray[2] from "<<i * 4<<" to "<<(i * 4) + 2<<endl;
          Temp1 = StringRange(LineArray[2],(i * 4),(i * 4) + 2,true);
          cout<<"Getting LineArray[2] from "<<(i * 4) + 2<<" to "<<(i * 4) + 4<<endl;
          Temp2 = StringRange(LineArray[2],(i * 4) + 2,(i * 4 ) + 4,true);
          // the idea here is that LineArray[2] will look a little something like this
          // 1003
          // ^^^^
          // x  y
          // with no space or anything seperating x and y, to make it easier to code
          tempoint.x = stoi(Temp1);
          cout<<"tempoint.x = "<<tempoint.x<<endl;
          tempoint.y = stoi(Temp2);
          cout<<"tempoint.y = "<<tempoint.y<<endl;
          EnemyArr.push_back(tempoint);
        }
        //end layer 2 proccessing
        //start layer 3 proccessing
        cout<<endl<<"LAYER THREE PROCCESSING"<<endl<<endl;
        cout<<"LineArray[3] = "<<LineArray[3]<<endl;
        Temp1.clear();
        Temp1.append(StringRange(LineArray[3],0,2));
        Temp2.clear();
        Temp2.append(StringRange(LineArray[3],2,4));
        player.x = stoi(Temp1);
        cout<<"player.x = "<<player.x<<endl;
        player.y = stoi(Temp2);
        cout<<"player.y = "<<player.y<<endl;
        character.pos.x = player.x;
        character.pos.y = player.y;
        //end layer 3 proccessing
        //clear game array

        for(int i = 0; i < ARRAYSIZE; i++){
          for(int k = 0; k < ARRAYSIZE; k++){
              barrier[k][i] = false;
              entity[k][i] = AIR;
          }
        }

        //put all this into the level

        for(int i = 0; i < Enemys; i++){
          entity[EnemyArr[i].x][EnemyArr[i].y] = ENEMY;
          cout<<"EnemyArr ["<<i<<"] = "<<EnemyArr[i].x<<" "<<EnemyArr[i].y<<endl;
        }
        for(int i = 0; i < Borders; i++){
          barrier[BorderArr[i].x][BorderArr[i].y] = true;
        }
        entity[player.x][player.y] = PLAYER;
      }
      void RenderArr(bool debug = false){
        float Temp1;
        float Temp2;
        float CellSize = screenHeight / ARRAYSIZE;
        Color colour;
        for(int x = 0; x < ARRAYSIZE; x++){
            Temp1 = CellSize * x;
            for(int y = 0; y < ARRAYSIZE; y++){
              Temp2 = CellSize * y;
              if(entity[x][y] == ENEMY){
                colour = ENEMYCOLOR;
              }
              else{
                colour = BACKGROUND;
              }
              DrawRectangle(Temp1,Temp2,CellSize,CellSize,colour);
              if(debug){
                string debugout = to_string(x) + "," + to_string(y);
                DrawText(debugout.c_str(),Temp1,Temp2, 20, LIGHTGRAY);
              }
            }
            DrawRectangle(character.pos.x * CellSize,character.pos.y * CellSize,character.size * CellSize,character.size * CellSize,PLAYERCOLOR);
        }
      }
      int ParseIn(){
        //0 - facing up
        //1 - facing right
        //2 - facing down
        //3 - facing left
        //4 - up and right
        //5 - up and left
        //6 - down and right
        //7 - down and left
        if(IsKeyDown('W')){
          if(IsKeyDown('D')){
            return 4;
          }
          else if(IsKeyDown('A')){
            return 5;
          }
          return 0;
        }
        else if(IsKeyDown('D')){
          if(IsKeyDown('S')){
            return 6;
          }
          return 1;
        }
        else if(IsKeyDown('S')){
          if(IsKeyDown('A')){
            return 7;
          }
          return 2;
        }
        else if(IsKeyDown('A')){
          return 3;
        }
        return -1;
      }
      void UpdateLogic(){
        if(oci){
          oci = false;
        }
        else{
          oci = true;
        }
        entity[int(character.pos.x)][int(character.pos.y)] = PLAYER;
        for(int y = 0; y < ARRAYSIZE; y++){
          for(int x = 0; x < ARRAYSIZE; x++){
              if(y > character.pos.y && y < character.bottomright.y && x > character.pos.x && x < character.bottomright.x){
                  entity[x][y] = PLAYER;
              }
          }
      }
        switch (ParseIn()) {
          //0 - facing up
          //1 - facing right
          //2 - facing down
          //3 - facing left
          //4 - up and right
          //5 - up and left
          //6 - down and right
          //7 - down and left
          case up :
            if(character.MoveTo(barrier,character.pos.x,character.pos.y - 0.5)){
              character.pos.y -= character.speed;
              character.bottomright.y -= character.speed;
            }
            break;
          case right :
            if(character.MoveTo(barrier,character.pos.x + 0.5,character.pos.y)){
              character.pos.x += character.speed;
              character.bottomright.y += character.speed;
            }
            break;
          case down :
            if(character.MoveTo(barrier,character.pos.x,character.pos.y + 0.5)){
              character.pos.y += character.speed;
              character.bottomright.y += character.speed;
            }
            break;
          case left :
            if(character.MoveTo(barrier,character.pos.x - 0.5,character.pos.y)){
              character.pos.x -= character.speed;
              character.bottomright.x -= character.speed;
            }
            break;
          case up_right :
            if(character.MoveTo(barrier,character.pos.x + 0.25,character.pos.y - 0.25)){
              character.pos.x += character.diagspeed;
              character.pos.y -= character.diagspeed;
              character.bottomright.x += character.diagspeed;
              character.bottomright.y -= character.diagspeed;
            }
            break;
          case up_left :
            if(character.MoveTo(barrier,character.pos.x - 0.25,character.pos.y - 0.25)){
              character.pos.x -= character.diagspeed;
              character.pos.y -= character.diagspeed;
              character.bottomright.x -= character.diagspeed;
              character.bottomright.y -= character.diagspeed;
            }
            break;
          case down_right :
            if(character.MoveTo(barrier,character.pos.x + 0.25,character.pos.y + 0.25)){
              character.pos.x += character.diagspeed;
              character.pos.y += character.diagspeed;
              character.bottomright.x += character.diagspeed;
              character.bottomright.y += character.diagspeed;
            }
            break;
          case down_left :
            if(character.MoveTo(barrier,character.pos.x - 0.25,character.pos.y + 0.25)){
              character.pos.x -= 0.25;
              character.pos.y += 0.25;
              character.bottomright.x -= character.diagspeed;
              character.bottomright.y += character.diagspeed;
            }
            break;
            entity[int(character.pos.x)][int(character.pos.y)] = PLAYER;
            for(int y = 0; y < ARRAYSIZE; y++){
              for(int x = 0; x < ARRAYSIZE; x++){
                  if(y > character.pos.y && y < character.bottomright.y && x > character.pos.x && x < character.bottomright.x){
                      entity[x][y] = PLAYER;
                  }


              }
          }
  //        default :
  //          break;
        }
        entity[int(character.pos.x)][int(character.pos.y)] = PLAYER;
        character.bottomright = {character.pos.x + character.size, character.pos.y + character.size};
        CheckArray();
        if(IsKeyPressed('F')){
  //        ToggleFullscreen(); doesnt work for some reason, warps screen
          if(!fullscreened) {
            MaximizeWindow();
            fullscreened = true;
          }
          else if(fullscreened) {
  //          SetWindowSize(screenWidth,screenHeight);
  //          SetWindowPosition(0,0);
            RestoreWindow();
            fullscreened = false;
          }
        }

      }
      void Init(){
        ClearArray();
        for(int i = 0; i < ARRAYSIZE; i++){
          barrier[i][0] = true;
          barrier[i][ARRAYSIZE - 1] = true;
          barrier[0][i] = true;
          barrier[ARRAYSIZE - 1][i] = true;
        }
      }
  };
}
int main(){
    cattoEngine::game_array test;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(cattoEngine::screenWidth, cattoEngine::screenHeight, "game");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
    test.Init();
    test.GetData("test.txt");
    while (!WindowShouldClose()){
    //    test.character.speed = 20;
    //    test.character.diagspeed = 10;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        test.UpdateLogic();
        test.RenderArr();
        EndDrawing();
      }
}