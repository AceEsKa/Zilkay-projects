#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "rng.h"


int i;

int pripad1(char*argument){
    for(i=0; i<MONSTER_TYPE_COUNT; i++){
        int a=strcmp(monster_types[i].name,argument);
        if(a==0){
            return 0;
        }
    }
    return 1;
}
int dmgcount(int atk_lvl,int deff_lvl, int atk_hp, int deff_hp,int deff_defense,int atk_atk)
{
    int base_dmg, stregth, defense;
    int c1=rnd(1,atk_hp);
    int d1=rnd(1,deff_hp);
    base_dmg=30+atk_lvl-deff_lvl;
    stregth=100+c1+atk_atk;
    defense=100+d1+deff_defense;
    int totaldmg=((base_dmg*stregth)/defense);

    return totaldmg;
}
int pripad2(int argc,char*meno)
{
    if (argc == 6)
    {
        FILE*  subor= fopen(meno,"r");
        if(subor==NULL)
        {
            return 2;
        }
        for(int n=0;n<ENEMY_TYPE_COUNT;n++)
        {
            if (fscanf(subor, "%s %d %d\n", enemy_types[n].name, &enemy_types[n].att, &enemy_types[n].def) != 3)
            {
                fclose(subor);
                return 3;
            }
        }
        fclose(subor);
    }
    return 0;
}

int main(int argc,char *argv[]) {
    int test;
    char *jedna;
    int seed1=strtol(argv[3],& jedna,10);
    int velkostarmady=strtol(argv[2],& jedna,10);
    int alivearmada = velkostarmady;
    Unit nepriatel[velkostarmady];
    int pozicia = 0;
    int total_monster = 0;
    int total_enemy = 0;
    int armada[velkostarmady];
    Unit prisera1[]={{&monster_types[i],MONSTER_INITIAL_HP,0}};
    int meno;



    test=pripad1(argv[1]);
    if(test==1){
        return 1;
    }

    else if (pripad2(argc,argv[5])==2){
        return 2;
    }
    else if(pripad2(argc,argv[5])==3){
        return 3;
    }

    srnd(seed1);

    printf("%s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",monster_types[i].name,monster_types[i].att,monster_types[i].def,prisera1[0].hp,prisera1[0].level);

    for(int a=0; a<velkostarmady; a++){
        meno=rnd(0,ENEMY_TYPE_COUNT-1);
        nepriatel[a].type=&enemy_types[meno];
        nepriatel[a].hp=rnd(ENEMY_MIN_INIT_HP,ENEMY_MAX_INIT_HP);
        nepriatel[a].level=rnd(0,UNIT_MAX_LEVEL);
        armada[a]=meno;
        printf("[%d] %s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",a,enemy_types[meno].name,enemy_types[meno].att,enemy_types[meno].def,nepriatel[a].hp,nepriatel[a].level);
    }
    int lowesthp=nepriatel[0].hp;
    int winner=0;

    while(prisera1[0].hp > 0) {
        if(nepriatel[pozicia].hp<=0)
        {
            for( int j=0;j<strtol(argv[2],&jedna,10);j++)
            {
                if(nepriatel[pozicia].hp<nepriatel[j].hp && nepriatel[j].hp>0)
                {
                    pozicia=j;
                    lowesthp=nepriatel[j].hp;
                    break;
                }
            }
        }

        for(int k=0;k<strtol(argv[2],&jedna,10);k++)
        {
            if(lowesthp>nepriatel[k].hp && nepriatel[k].hp>0)
            {
                lowesthp=nepriatel[k].hp;
                pozicia=k;
            }
        }
        int damage=dmgcount(prisera1[0].level,nepriatel[pozicia].level,prisera1[0].hp,nepriatel[pozicia].hp,enemy_types[armada[pozicia]].def,monster_types[i].att);
        total_monster=total_monster+damage;
        nepriatel[pozicia].hp=nepriatel[pozicia].hp-damage;
        if(nepriatel[pozicia].hp <=0 ){
            alivearmada=alivearmada-1;
        }
        printf("\n%s => %d => [%d] %s\n",monster_types[i].name,damage,pozicia,enemy_types[armada[pozicia]].name);

        for(int l=0;l<strtol(argv[2],&jedna,10);l++)
        {
            if(nepriatel[l].hp>0 && prisera1[0].hp>0)
            {
                damage = dmgcount(nepriatel[l].level,prisera1[0].level,nepriatel[l].hp,prisera1[0].hp,monster_types[i].def,enemy_types[armada[l]].att);
                prisera1[0].hp -= damage;
                total_enemy+=damage;
                printf("[%d] %s => %d => %s\n",l,enemy_types[armada[l]].name,damage,monster_types[i].name);
            }
        }
        if(prisera1[0].hp>0) {
            if(prisera1[0].level!=UNIT_MAX_LEVEL)
            {
                prisera1[0].level=prisera1[0].level+1;
            }
        }
        else{
            winner=1;
        }
        printf("\n%s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",monster_types[i].name,monster_types[i].att,monster_types[i].def,prisera1[0].hp,prisera1[0].level);
        for(int m=0;m<strtol(argv[2],&jedna,10);m++)
        {
            printf("[%d] %s, ATT:%d, DEF:%d, HP:%d, LVL:%d\n",m,enemy_types[armada[m]].name,enemy_types[armada[m]].att,enemy_types[armada[m]].def,nepriatel[m].hp,nepriatel[m].level);
        }
        if(alivearmada==0){
            break;
        }

    }
    if(winner==0)
    {
        printf("\nWinner: %s\nTotal monster DMG: %d\nTotal enemies DMG: %d\n",argv[1],total_monster,total_enemy);
    }
    else
    {
        printf("\nWinner: Enemy\nTotal monster DMG: %d\nTotal enemies DMG: %d\n",total_monster,total_enemy);
    }

    return 0;
}
