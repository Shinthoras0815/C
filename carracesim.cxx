	//  g++ carracesim.cxx -o carracesim.exe -I D:/msys64/ucrt64/include -L D:/msys64/ucrt64/lib  -lncursesw -DNCURSES_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses/ncurses.h>
#include <time.h>

int buycolour(int, char[], int);
void print (int);
void shop ();
int increasestat(int);
int changemon(int, int);
//int chosenextrace ();
void initrace();
int difchos = 0;
int track = 0;


int colprice = 300;

#define COLOUR_CASE 1
#define SPEED_CASE 2
#define HANDLING_CASE 3
#define ACCELERATION_CASE 4
#define BRAKE_CASE 5
#define NITRO_CASE 6

	
	struct properties{
	char colour[1024];
	int speed;
	int handling;
	int acceleration;
	int brake;
	int nitro;
	};
	struct properties car1;
	int playermon = 1000;
	
	struct scenario{
	int NUM;
	int *data;
	};

	int main(int argc, char *argv[])
{
	initscr();
	cbreak();
	noecho();
	
	srand((unsigned int)time(NULL));
	
	strcpy(car1.colour, "grey");
	car1.speed = 1;
	car1.handling = 1;
	car1.acceleration = 1;
	car1.brake = 1;
	car1.nitro = 1;
	

//	int playerstatchose = 0;
	
	
	
	struct car2{
	int car2colour;
	int car2speed;
	int car2handling;
	int car2acceleration;
	int car2brake;
	int car2nitro;
	};
	
	struct car3{
	int car3colour;
	int car3speed;
	int car3handling;
	int car3acceleration;
	int car3brake;
	int car3nitro;
	};
	
	struct car4{
	int car4colour;
	int car4speed;
	int car4handling;
	int car4acceleration;
	int car4brake;
	int car4nitro;
	};

shop();
//chosenextrace();
//initrace(&difchos, &track);
initrace();
endwin();
}
	
	void shop (){
int error = 0;
int negmon = 0;
int playerstat = 0;
while(1){
	int buything;
	int price = 100;
	clear();
	print(playermon);
	if(negmon == 1){
		printw("!!! NOT ENOUGH MONEY !!! \n\n");
		negmon = 0;
	}
	printw("buy improvements:\n\tcosts: %i $\n\t\t1 = colour\n\tcosts: %i $\n\t\t2 = speed\n\t\t3 = handling\n\t\t4 = acceleration\n\t\t5 = brake\n\t\t6 = nitro\n\n\n\t\t7 = check car stat\n\n\t\t\t8 = continue\n\n", colprice, price);
	if (error > 0){
		printw("ERROR wrong input\n");
		error = 0;
	}
	refresh();

//	scanf("%i", &buything);
buything = (getchar()) -'0';
printw("%i", buything);
	if(buything < 8 && ((playermon - price) <0 || (playermon - colprice) <0)){
		negmon++;
		buything = 7;
	}


	switch(buything) {
		case COLOUR_CASE:
		 playermon = buycolour (playermon, car1.colour, playerstat);
		 break;
		case SPEED_CASE: playerstat = car1.speed;
		playerstat = increasestat(playerstat);
		playermon = changemon(playermon, price);
		car1.speed = playerstat;
		break;
		case HANDLING_CASE: playerstat = car1.handling;
		playerstat = increasestat(playerstat);
		playermon = changemon(playermon, price);
		car1.handling = playerstat;
		break;
		case  ACCELERATION_CASE: playerstat = car1.acceleration;
		playerstat = increasestat(playerstat);
		playermon = changemon(playermon, price);
		car1.acceleration = playerstat;
		break;
		case BRAKE_CASE: playerstat = car1.brake;
		playerstat = increasestat(playerstat);
		playermon = changemon(playermon, price);
		car1.brake = playerstat;
		break;
		case NITRO_CASE: playerstat = car1.nitro;
		playerstat = increasestat(playerstat);
		playermon = changemon(playermon, price);
		car1.nitro = playerstat;
		break;
		case 7: 
		print(playermon);
		break;
		case 8:
		clear();
		return ;
		break;
		default: 
		error++;
		break;
	};
	refresh();
//	printf("amount?\n");
//	scanf("%d", &amount);
continue;
	}
}

	
	int buycolour (int playermon, char colour[],int playerstat){
		
		int choscol= 0;
		clear();
		printw("which colour?:\n\t1 = red\n\t2 = blue\n\t3 = black \n\t4 = green \n\t5 = gray\n");
		refresh();
		choscol = (getchar()) - '0';
	//		scanf("%i", &amount);

		playermon = changemon(playermon, colprice);
		switch (choscol) {
			case 1: 
            strcpy(colour, "red");
            break;
        case 2: 
            strcpy(colour, "blue");
            break;
        case 3: 
            strcpy(colour, "black");
            break;
        case 4:
            strcpy(colour, "green");
            break;
        default: 
            strcpy(colour, "grey");
            break;
		};
		return playermon;
	}
	
	int increasestat (int playerstat){
	playerstat++;
	return playerstat;
	}
	
	int changemon( int playermon, int price){
		playermon -= price;
		return(playermon);
	}
	
void print(int playermon){
		printw("\n\n\t\tMONEY = %i $ \n\nCar Colour = %s \nCar Speed = %i\nCar Handling = %i\nCar Acceleration= %i\nCar Brake = %i\nCar Nitro = %i\n\n\n",playermon, &car1.colour, car1.speed, car1. handling, car1.acceleration, car1.brake, car1.nitro);
		refresh();
		
	}

void initrace(){
	while(1){
	printw("\n\n\n\t\tChose your next race:\n\n\t\t\tEASY = 1\n\t\t\tMODERATE = 2\n\t\t\tHARD = 3\n\n");
	refresh();
	difchos = (getchar()) - '0';
	clear();
	int trackchecksdif= 0;
	switch (difchos){
			//easy
			case 1:
			trackchecksdif -= 2;
			break;
			// moderate 
			case 2:
			break;
			//hard
			case 3:
			trackchecksdif += 3;
			break;
			default:
			clear();
			printw("ERROR wrong input\n");
			refresh();
			getchar();
			clear();
			continue;
		}
        break;
	}
	struct checks;
		int speedchecks;
		int handlingchecks;
		int accelerationchecks;
		int brakeschecks;
		int nitrochecks;
		int otherchecks;
	while(1){
	printw("\n\n\n\t\tTRACK:\n\n\t\t\tHills = 1\n\t\t\tDesert = 2\n\t\t\tCity = 3\n\n");
	refresh();
	track = (getchar()) - '0';
	clear();
		switch (track) {
			//hills
			case 1:
			/*1 speed, 4 handling, 3 acceleration,4 brakes,1 nitro,2 other*/
			speedchecks = 1;
			handlingchecks = 4;
			accelerationchecks = 3;
			brakeschecks = 4;
			nitrochecks = 1;
			otherchecks = 2;
			break;
			//desert
			case 2:
			break;
			//city
			case 3:
			break;
			default:
			clear();
			printw("ERROR wrong input\n");
			refresh();
			getchar();
			clear();
			continue;
		}
        break;
	}
		struct scenario speed;
		speed.NUM = speedchecks;
		int speedscenario[speed.NUM];	
		for(int i = 0; i < speedchecks; i++){
			speedscenario[i] = rand() % 5 + 1;
		}

		int handlingNUM = handlingchecks;
		int handlingscenario[handlingNUM];	
		for(int i = 0; i < handlingchecks; i++){
			handlingscenario[i] = rand() % 5 + 1;
		}

		int accelerationNUM = handlingchecks;
		int handlingscenario[handlingNUM];	
		for(int i = 0; i < handlingchecks; i++){
			handlingscenario[i] = rand() % 5 + 1;
		}

		int handlingNUM = handlingchecks;
		int handlingscenario[handlingNUM];	
		for(int i = 0; i < handlingchecks; i++){
			handlingscenario[i] = rand() % 5 + 1;
		}

		int handlingNUM = handlingchecks;
		int handlingscenario[handlingNUM];	
		for(int i = 0; i < handlingchecks; i++){
			handlingscenario[i] = rand() % 5 + 1;
		}

		int handlingNUM = handlingchecks;
		int handlingscenario[handlingNUM];	
		for(int i = 0; i < handlingchecks; i++){
			handlingscenario[i] = rand() % 5 + 1;
		}		
for(int i = 0; i < handlingchecks; i++){
			switch (handlingscenario[i]) {
				case 1:
			//	clear();
				printw("handling test1");
				break;
				case 2:
			//	clear();
				printw("handling test2");
				break;
				case 3:
			//	clear();
				printw("handling test3");
				break;
				case 4:
			//	clear();
				printw("handling test4");
				break;
				case 5:
			//	clear();
				printw("handling test5");
				break;
				default:
			//	clear();
				printw("handlingcheck Error");
				break;
		}
		refresh();
	};

		for(int i = 0; i < speedchecks; i++){
			switch (speedscenario[i]) {
				case 1:
			//	clear();
				printw("speed test1");
				break;
				case 2:
			//	clear();
				printw("speed test2");
				break;
				case 3:
			//	clear();
				printw("speed test3");
				break;
				case 4:
			//	clear();
				printw("speed test4");
				break;
				case 5:
			//	clear();
				printw("speed test5");
				break;
				default:
			//	clear();
				printw("Speedcheck Error");
				break;
		}
		refresh();
	};
	getchar();
	return;
}
	