#include"objects.hpp"
#include <bits/stdc++.h>
#include<ncurses.h>
#include<string>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <utility>

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }

    return 0;
}
using namespace std;

//global_vals
int skip_turn_flag = 0;
int key;
int turn;
int dy[4]={1,0,-1,0};
int dx[4]={0,1,0,-1};
int satiety_level=50;

void intmvprint(int i, int j, int val){
        string s = to_string(val);
        char const *pchar = s.c_str();
        mvprintw(i,j,pchar);
}

void Player::move(std::string key, Player& player){
    if(key=="left")player.x--;
    else if(key=="up")player.y--;
    else if(key=="right")player.x++;
    else if(key=="down")player.y++;
    else cerr<<"illeagl key"<<endl;
}

void Player::skill(string key, Player& player){
    if(key=="q"){
        if(player.state=="none")player.state="flying";
        else player.state="none";
    }

}
void Player::item_process(std::string key){}

inline void initdice(){
    srand((unsigned int)time(NULL));
}

inline int dice(){
    // it's a d100 dice.
    return rand()%100+1;
}

int Field::generate_floor(int i, int j){
    int p=Wall_init_prob;
    for(int k=0;k<4;k++){
        if(field[i+dx[k]][j+dy[k]]==-1)p=Wall_next_prob;
    }
    if(dice()<=p)return -1;
    else{
        return dice()%Num_floor;
    }
}

void Field::renew_field(){
    for(int i=1;i<H+1;i++){
        for(int j=1;j<W+1;j++){
            field[i][j]=generate_floor(i,j); 
        }
    }
}

void Field::field_initialize(){
    // wall:-1,floor:0~4 
    for(int i=0;i<W+2;i++){
        field[0][i]=-1;
        field[H+1][i]=-1;
    }
    for(int i=0;i<H+2;i++){
        field[i][0]=-1;
        field[i][W+1]=-1;
    }
    renew_field();
    // both characters were placed at (H/2,W/4) (H/2,3*W/4);
    // rewrite between them as floor

    for(int i=W/4;i<=3*W/4;i++){
        field[H/2][i]=dice()%Num_floor;
    }
} 


void Field::generate_beans(){
    int p;
    for(int i=0;i<H+1;i++){
        for(int j=0;j<W+1;j++){
            p=dice();
            if(p<=Bean_prob && field[i][j]!=-1){
                beans.push_back({i,j}); 
            }
         }
    }
}

int colorname_to_code(string color){
   if(color=="red")return -1;
   if(color=="green")return 0;
   if(color=="yellow")return 1;
   if(color=="blue")return 2;
   if(color=="violet")return 3;
   if(color=="cyan")return 4;
   return -2;
}

string code_to_colorname(int color){
   if(color==-1)return "red";
   if(color==0)return "green";
   if(color==1)return "yellow";
   if(color==2)return "blue";
   if(color==3)return "violet";
   if(color==4)return "cyan";
   return "error";
}

int color_to_background_code(int color){
   if(color==-1)return 1;
   if(color==0)return 2;
   if(color==1)return 3;
   if(color==2)return 4;
   if(color==3)return 5;
   if(color==4)return 6;
   return -2;
}

bool nextfloor_include(string color,int x, int y, Field field){
    for(int i=0;i<4;i++){
        if(color==code_to_colorname(field.field[y+dy[i]][x+dx[i]])){
            return 1;
        }
    } 
    return 0;
}

bool does_cook_eat_chicken(Player& player,Enemy& enemy){
    if(player.x==enemy.x && player.y==enemy.y)return 1;
    else return 0;
}

bool is_on_beans(int i, int j, Field& field,bool eat){
    for(int k=0;k<field.beans.size();k++){
        if(field.beans[k].first==i && field.beans[k].second==j){
            if(eat)field.beans.erase(field.beans.begin()+k);
            return 1;
        }
    }
    return 0;
}

void add_bean(Field& field){
    int cnt=0;
    while(cnt<10){
        cnt++;
        int w=dice()%W;
        int h=dice()%H;
        if(field.field[h][w]==-1)continue;
        else{
            field.beans.push_back({h,w});
            break;
        }
    }
}

void Player::process(string input,Player& player, Field& field){
    turn++;
    if(player.state=="none")satiety_level--;
    else satiety_level-=5;
    int ix=player.x,iy=player.y;
    if(input=="left" || input=="right" || input=="up"|| input=="down")player.move(input,player);
    else if(input=="q" || input=="e" || input=="r")player.skill(input,player);
    else player.item_process(input);

    if(player.state!="flying")field.player_color=code_to_colorname(field.field[player.y][player.x]);
    //const char *pchar = field.player_color.c_str();
    //mvaddstr(41,0,player.color.c_str());
    
    if(player.state!="flying" && is_on_beans(player.y,player.x,field,1)){
        turn+=Bean_add_score;
        satiety_level+=Bean_add_score;
        add_bean(field);
    }
    if(field.player_color=="red"){
        // don't move
        player.x=ix;
        player.y=iy;
    }
    else if(field.player_color=="blue" && player.color=="green"){
        skip_turn_flag++; 
    }
    else if(field.player_color=="blue" && nextfloor_include("yellow",player.x,player.y,field)){
        skip_turn_flag++; 
    }
    else if(field.player_color=="green"){
        player.color="green";
    }
    else if(field.player_color=="yellow"){
        skip_turn_flag++;

    }
    else if(field.player_color=="violet"){
        player.color="none";
    }
    else{
        
    }
    if(player.state=="none"){
        const char *pchar = player.color.c_str();
        mvaddstr(22,3,"player_state:");
        mvaddstr(22,17,pchar);
    }
    else{
        mvaddstr(22,3,"player_state: flying");
    }
}

void enemy_process(Field& field, Player& player, Enemy& enemy){
    if(does_cook_eat_chicken(player,enemy))return;
    //if cook was surrounded by wall, he will respawn at (10,10)
    int newdist,nx,ny,optx=10,opty=10,mindist=1000000;
    // randamize walks to choose optimals vals for various direction
    vector<int> walk_order={0,1,2,3};
    random_shuffle(walk_order.begin(), walk_order.end());
    for(int i=0;i<4;i++){
        nx=enemy.x+dx[walk_order[i]];
        ny=enemy.y+dy[walk_order[i]];
        newdist=abs(player.x-nx)+abs(player.y-ny);
        if(field.field[ny][nx]==-1)continue;
        if(mindist>newdist){
            mindist=newdist;
            optx=nx;
            opty=ny;
        }
    }
    enemy.x=optx;
    enemy.y=opty;
}

void termination(){
    erase();
    clear();
    endwin();
    cout<<endl;
    cout<<"GAME OVER!!"<<endl;
    cout<<" "<<endl;
    cout<<"SCORE: "<<turn<<endl;
    cout<<" "<<endl;
}

string getKey(){
    string out="none";
    while(1){
        key = getch();
        if(key==-1)continue;
        else if(key=='w')out="up";
        else if(key=='s')out="down";
        else if(key=='a')out="left";
        else if(key=='d')out="right";
        else if(key=='q')out="q";
        else if(key=='e')out="e";
        else if(key=='r')out="r";
        else if(key=='1')out="1";
        else if(key=='2')out="2";
        else if(key=='3')out="3";
        else if(key=='4')out="4";
        break;
    }
   return out; 
}

void draw(Player player, Enemy enemy, Field field){
    //std::system("clear"); 
    int val;
    for(int i=1;i<H+1;i++){
        for(int j=1;j<W+1;j++){
            val = field.field[i][j];
            string s = to_string(val);
            //char const *pchar = s.c_str();
            //mvprintw(i+30,j,pchar);
            attrset(COLOR_PAIR(color_to_background_code(field.field[i][j])));
            if(player.x==j && player.y==i){
                mvaddstr(i,3*j," @ ");
                //cout<<"\033["<<color_to_background_code(field.field[i][j])<<"m @ "<<"\033[m"; //chicken
            }
            else if(enemy.x==j && enemy.y==i){
                mvaddstr(i,3*j," C ");
                //cout<<"\033["<<color_to_background_code(field.field[i][j])<<"m C "<<"\033[m"; //man
            }
            else{
                if(is_on_beans(i,j,field,0))mvaddstr(i,3*j," . ");
                else mvaddstr(i,3*j,"   ");
                //intmvprint(i,3*j,field.field[i][j]);
                //cout<<"\033["<<color_to_background_code(field.field[i][j])<<"m " <<((field.field[i][j]==-1)?"# ":"  ")<<"\033[m";
            }
        }
    }
    attrset(0);
}

void init(){
    initdice();
    initscr();
    erase();
    noecho(); //キーが入力されても表示しない
    curs_set(0);//カーソルを非表示
    start_color();
    use_default_colors();
    mvprintw(12, 30, "Eat the chicken!");
    mvprintw(14, 30, "Press q to start");
     while (true) {
         int ch = getch();
         if (ch == 'q') break;
     }
    init_pair(1, COLOR_WHITE, COLOR_RED); 
    init_pair(2, COLOR_WHITE, COLOR_GREEN); 
    init_pair(3, COLOR_WHITE, COLOR_YELLOW); 
    init_pair(4, COLOR_WHITE, COLOR_BLUE); 
    init_pair(5, COLOR_WHITE, COLOR_MAGENTA); 
    init_pair(6, COLOR_WHITE, COLOR_CYAN); 
}

int main(){
    // init process
    init();
    Player player;
    Field field;
    Enemy enemy;
    string input;
    int out;
    refresh();
    clear();
    //Turn  
    while(!does_cook_eat_chicken(player,enemy) && satiety_level>0){
        refresh();
        draw(player,enemy,field);
        refresh();
        if(skip_turn_flag==0){
            input = getKey();
            erase();
            refresh();
            player.process(input,player,field);
        }
        else{
            clear();
            mvaddstr(22,3,"player_state: stunned");
            skip_turn_flag--;
        }
        mvaddstr(22,36,"score: ");
        intmvprint(22,50,turn);
        mvaddstr(24,3,"satiety_level: ");
        intmvprint(24,20,satiety_level);
        refresh();
        enemy_process(field,player,enemy);
        //std::system("sleep 1"); 
    }
    termination();
    return 0;
}
