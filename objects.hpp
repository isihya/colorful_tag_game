#ifndef   OBJECTS_HPP
#define   OBJECTS_HPP

#include <bits/stdc++.h>
#define W 20
#define H 20
#define Wall_init_prob 15
#define Wall_next_prob 35
#define Bean_prob 10
#define Bean_add_score 5
#define Num_floor 5

class Enemy{
    public:
        int x;
        int y;
        std::string color;
        Enemy(){
            color="none";
            x=3*W/4;
            y=H/2;
        }
};

class Field{
    public:
        int field[H+2][W+2];
        int dx[4]={1,0,-1,0};
        int dy[4]={0,1,0,-1};
        int i,j;
        std::vector<std::pair<int,int>> beans;
        std::string player_color;
        Field(){
            field_initialize();
            generate_beans();
        }
        int generate_floor(int i,int j);
        void field_initialize();
        void renew_field();
        void generate_beans();
        void add_bean();
};


class Player{
    public:
        bool are_you_chicken;
        int x;
        int y;
        std::string color;
        std::string state;
        std::string key;
        Player(){
            color="none";
            state="none";
            x=W/4;
            y=H/2;
        }
        void move(std::string key, Player& player);
        void skill(std::string key, Player& player);
        void item_process(std::string key);
        void process(std::string input,Player& player, Field& field);
};



#endif
