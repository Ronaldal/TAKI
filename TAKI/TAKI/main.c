#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // in order to use the "rand" and "srand" functions
#include <time.h>	// in order to use "time" function

#define PLAYER_NAME_MAX_LEN 20
#define MAX_FERQNCY_LEN 14
#define MIN_NUM_OF_PLAYERS 2
#define FIRST_CARD_LEN 4
#define FIRST_PLAYER 0
#define INCORRECT_SELCTION 0
#define MAX_COLOR 5
#define MAX_CARD 10
#define TAKE_CARD 0
#define NOT_FOUND -1
#define RED_TEXT 'R'
#define YELLOW_TEXT 'Y'
#define BLUE_TEXT 'B'
#define GREEN_TEXT 'G'
#define PLUSE_TEXT "+"
#define PLUSE_CARD_ID 10
#define STOP_TEXT "STOP"
#define STOP_CARD_ID 11
#define WAY_CHANGE_TEXT "<->"
#define WAY_CHANGE_CARD_ID 12
#define TAKI_TEXT "TAKI"
#define TAKI_CARD_ID 13
#define COLOR_TEXT "COLOR"
#define COLOR_CARD_ID 14

struct Card_info // Struct that saves the color and num of the card
{
	char color;
	int num;
};
typedef struct Card_info CARD_INFO;
struct Player// Struct that saves any player in the gme information
{
	char name[PLAYER_NAME_MAX_LEN]; //Name of the player
	CARD_INFO* cardInfo;//Array of card info
	int nofcard; // Number of card and logical size of card array
	int phySize;// Physical size of card array 
};
typedef struct Player PLAYER;
struct Game_Direction // Struct that saves dirction of the game 
{
	int direction;// The number will be only 1 or -1 if the direction of the game is reversed
	int NumOfPlayersToJump;
};
typedef struct Game_Direction GAMEDIRECTION;
struct Freqnc // Struct that saves the statistics of the game 
{
	int value;
	int count;
};
typedef struct Freqnc FREQNC;

PLAYER* GetNumOfPlayers(int* NumOfPlayer);
CARD_INFO* GetName(PLAYER Players[], int size);
void PrintTurnDetails(PLAYER* players, CARD_INFO* upperCard, int CurrentPlyr);
void TakeCard(PLAYER* players, int CurrentPlyr, int* NofCard, int* PhySize, FREQNC CardFreq[]);
void ResetFreqArry(FREQNC CardFreq[]);
void GetUpperCard(CARD_INFO* cards, FREQNC CardFreq[]);
void PrintUpperCard(CARD_INFO cards);
char GetColor(int c);
void GetCards(PLAYER players[], int size, FREQNC CardFreq[]);
void PrntPlyrCards(PLAYER* players, int p);
int PutCards(PLAYER* players, CARD_INFO* card, int p, FREQNC CardFreq[]);
void SwapUpperCard(CARD_INFO* PlayerCard, CARD_INFO* Uppercard, int curntCard);
int CheckCard(CARD_INFO* PlayerCard, CARD_INFO* card, int curntCard, int* NofCard);
void SwapCards(CARD_INFO* PlayerCard, int curntCard, int* NofCard);
void PrintSpeCard(CARD_INFO CARD);
void CheckDirection(GAMEDIRECTION* GameDirection, int NumOfCard, int NumOfPlayer);
void HandleColorCard(CARD_INFO* upperCard);
void CheckCurrntPlayerRang(int* currentplyr, int NumOfPlayer);
CARD_INFO* alloc(CARD_INFO* CurrentArr, int size, int newSize);
void mergeSort(FREQNC arr[], int size);
FREQNC* merge(FREQNC a1[], int size1, FREQNC a2[], int size2);
void copyArr(FREQNC dest[], FREQNC src[], int size);
void PrintFREQNCDetails(FREQNC arr[], int size);
void main()
{
	//The number of players, the player whose turn to play, the number of players the player chooses to put
	int NumOfPlayers, currPlayer = 0, CardId;
	int end = NOT_FOUND;// A variable that we use to check whether the game is over or not
	PLAYER* Players;// Array of players info 
	char Winner[PLAYER_NAME_MAX_LEN]; // String that saves the name of the winner
	CARD_INFO UpperCard;// Struct of the upper card 
	FREQNC CardFreq[MAX_FERQNCY_LEN];//Array of game statistics
	GAMEDIRECTION GameDirection = { 1,1 };//Struct of the game diercation 

	ResetFreqArry(CardFreq);// Function that rest the statistics array
	printf("************  Welcome to TAKI game !!! ***********\n");
	srand(time(NULL));
	//A function that returns the address of an array of players and updates the number of players
	Players = GetNumOfPlayers(&NumOfPlayers);
	//Go through the array of players and get the address of a array of cards for each player
	for (int i = 0; i < NumOfPlayers; i++)
	{
		Players[i].cardInfo = GetName(Players, i);
		(Players + i)->nofcard = FIRST_CARD_LEN;//Update logical size of cards arry
		(Players + i)->phySize = FIRST_CARD_LEN;//Update physical size of cards arry
	}
	GetUpperCard(&UpperCard, CardFreq);// Draw the upper card
	GetCards(Players, NumOfPlayers, CardFreq);// Draw the of players cards

	while (end == NOT_FOUND)
	{
		PrintTurnDetails(Players, &UpperCard, currPlayer);// Prints the Upper card and the current player cards
		CardId = PutCards(Players, &UpperCard, currPlayer, CardFreq);
		CheckDirection(&GameDirection, CardId, NumOfPlayers);
		//Check's if game over , if the player's last card was a plus then he should pull a card
		if (Players[currPlayer].nofcard == 0 && CardId != PLUSE_CARD_ID)
		{
			end = 1;
			strcpy(Winner, Players[currPlayer].name);// Updates the winner name
		}
		currPlayer += GameDirection.direction * GameDirection.NumOfPlayersToJump;// Calculates the player's turn
		CheckCurrntPlayerRang(&currPlayer, NumOfPlayers);//Check's if the current Player is un range
	}
	printf("\nThe Winner is...%s! Congratulations! \n", Winner);
	mergeSort(CardFreq, MAX_FERQNCY_LEN);//Sort bigger to lower card frequency
	PrintFREQNCDetails(CardFreq, MAX_FERQNCY_LEN);// Prints the statistics of the game
	for (int i = 0; i < NumOfPlayers; i++)// Free the arrays 
		free((Players + i)->cardInfo);
	free(Players);
}
/*
//The function gets the array of players, the main card and the number of players in turn to play
//The function prints the main card and the cards of the player in turn to play
*/
void PrintTurnDetails(PLAYER* players, CARD_INFO* upperCard, int CurrentPlyr)
{
	PrintUpperCard(*upperCard);
	printf("\n%s's turn:\n", (players + CurrentPlyr)->name);
	PrntPlyrCards(players, CurrentPlyr);//Calls to function that prints the cards of the player
}
/*
//The function gets the players array and the num of the current player
//The function prints cards
{*/
void PrntPlyrCards(PLAYER* players, int CurrentPlyr)
{
	int i;

	for (i = 0; i < (players + CurrentPlyr)->nofcard; i++)
	{
		printf("\nCard #%d:", i + 1);
		printf("\n*********");
		printf("\n*       *");
		if ((players + CurrentPlyr)->cardInfo[i].num <= 9)// if not special card 
			printf("\n*   %d   *", (players + CurrentPlyr)->cardInfo[i].num);
		else
			PrintSpeCard((players + CurrentPlyr)->cardInfo[i]);
		printf("\n*   %c   *", (players + CurrentPlyr)->cardInfo[i].color);
		printf("\n*       *");
		printf("\n*********\n");
	}
}
/*
//The function gets the plyaers array and the num of the current player, and the place of the card
//The function prints special cards
{*/
void PrintSpeCard(CARD_INFO CARD)
{
	switch (CARD.num)
	{
	case PLUSE_CARD_ID: // case 10 print the card PLUSE
		printf("\n*   %s   *", PLUSE_TEXT);
		break;
	case STOP_CARD_ID: // case 11 print the card STOP
		printf("\n*  %s *", STOP_TEXT);
		break;
	case WAY_CHANGE_CARD_ID:// case 12 print the card <->
		printf("\n*  %s  *", WAY_CHANGE_TEXT);
		break;
	case TAKI_CARD_ID:// case 13 print the card TAKI
		printf("\n*  %s *", TAKI_TEXT);
		break;
	case COLOR_CARD_ID:// case 14 print the card COLOR
		printf("\n* %s *", COLOR_TEXT);
		break;
	}
}
/*
// The function get int by reference
//The function returns the address of the array and update the number of player in main
*/
PLAYER* GetNumOfPlayers(int* pNumOfPlayer)
{
	PLAYER* data = NULL;
	printf("Please enter the number of players :\n");
	scanf("%d", pNumOfPlayer);
	data = (PLAYER*)malloc(*pNumOfPlayer * (sizeof(PLAYER)));
	if (data == NULL)
	{
		printf("Memory allocation failure!!!\n");
		exit(1);
	}
	return data;

}
/*
//The function gets players array and the current player
//The function updates the player's name, and assigns a array of cards to each player
*/
CARD_INFO* GetName(PLAYER Players[], int CurrentPlayer)
{
	CARD_INFO* card = NULL;
	printf("Please enter the first name of player #%d:\n", (CurrentPlayer + 1));
	scanf("%s", &Players[CurrentPlayer].name);
	card = (CARD_INFO*)malloc((sizeof(CARD_INFO)) * FIRST_CARD_LEN);
	if (card == NULL)
		exit(1);
	return card;
}
/*
//The function gets the array of the statistics of the game
*/
void ResetFreqArry(FREQNC CardFreq[])
{
	int i;
	for (i = 0; i < MAX_FERQNCY_LEN; i++)
	{
		CardFreq[i].value = i + 1;// Taype of card
		CardFreq[i].count = 0;// Reset the counter of any card
	}
}
/*
//The function deal random card for the upper card
//The function get the upper card struct by reference and the statistics array for update
*/
void GetUpperCard(CARD_INFO* cards, FREQNC CardFreq[])
{
	int num, color;
	num = 1 + rand() % 9;
	color = 1 + rand() % 4;
	cards->color = GetColor(color);
	cards->num = num;
	CardFreq[num - 1].count++;//Update the stastic of the card 
	//for example : if the card is 6 so is place in the array is 5 and then we need to update the count by one
}
/*
//The function get an int And returns a char according to the number
*/
char GetColor(int c)
{
	switch (c)
	{
	case 1:
		return YELLOW_TEXT;
	case 2:
		return RED_TEXT;
	case 3:
		return BLUE_TEXT;
	case 4:
		return GREEN_TEXT;
	}
}
/*
//The function gets the upper card and prints it
*/
void PrintUpperCard(CARD_INFO card)
{
	printf("\nUpper card:");
	printf("\n*********");
	printf("\n*       *");
	if (card.num <= 9)//If not special card 
		printf("\n*   %d   *", card.num);
	else
		PrintSpeCard(card);//If special card we send the card to function that prints it 
	printf("\n*   %c   *", (card.color));
	printf("\n*       *");
	printf("\n*********\n");
}
/*
//The function gets the players array and his size
//The function deals cards to each player
*/
void GetCards(PLAYER players[], int size, FREQNC CardFreq[])
{
	int num, color, i, j;
	for (j = 0; j < size; j++)
	{
		for (i = 0; i < players[j].nofcard; i++)
		{
			num = 1 + rand() % 14;// Get random num betwwen 1 to 14 
			color = 1 + rand() % 4;
			CardFreq[num - 1].count++;
			players[j].cardInfo[i].num = num;
			if (num == 14) // If 14 so the card is color and we dont need to put value in color  struct
				players[j].cardInfo[i].color = ' ';
			else
				players[j].cardInfo[i].color = GetColor(color);

		}
	}
}
/*
//The function get the Players array , the upper card by by reference, number of the current player and the statistics array for update
//The function is responsible for selecting the card that a player wants to put on the table
*/
int PutCards(PLAYER* players, CARD_INFO* upperCard, int CurrentPlyr, FREQNC CardFreq[])
{
	int found = NOT_FOUND, choice;
	while (found == NOT_FOUND)
	{
		printf("\nPlease enter 0 if you want to take a card from the deck");
		printf("\nor 1-%d if you want to put one of your cards in the middle:\n", (players + CurrentPlyr)->nofcard);
		scanf("%d", &choice);
		if (choice == TAKE_CARD)//If the player chose 0 
		{
			//Send to a function that handles that the player wanted to draw a card from the deck
			TakeCard(players, CurrentPlyr, &((players + CurrentPlyr)->nofcard), &((players + CurrentPlyr)->phySize), CardFreq);
			return TAKE_CARD;
		}
		// If the number is on range of number of cards that he have
		else if (choice <= (players + CurrentPlyr)->nofcard && choice > 0)
			found = CheckCard((players + CurrentPlyr)->cardInfo, upperCard, choice - 1, &((players + CurrentPlyr)->nofcard));
		//If not good choice of card so the found variable has not changed
		if (found == NOT_FOUND || choice < 0)
			printf("Invalid card! Try again.\n");
		else
		{
			if (found == TAKI_CARD_ID)// If the player choose to put taki
				found = HandleTakiCard(players, upperCard, CurrentPlyr);
			if (found == COLOR_CARD_ID)
				HandleColorCard(upperCard); //If the card is a color then the player has to choose the color he wants to have at the upper card
			return found; //Return the num of the card
		}
	}
}



/*
//The function adds a card to the user at the end of the array
//The function gets the players array ,current player number,
number of card that the player have now, the current players physical number of the cards array
*/
void TakeCard(PLAYER* players, int CurrentPlyr, int* NofCard, int* PhySize, FREQNC CardFreq[])
{
	int num;
	char color;
	if (*NofCard == *PhySize)//Check if there is space left on the card array 
	{
		//If the number of cards is equal to the physical size then we will increase the array
		players[CurrentPlyr].cardInfo = alloc(players[CurrentPlyr].cardInfo, *NofCard, (*PhySize * 2));
		*PhySize = *PhySize * 2;
	}
	*NofCard = *NofCard + 1;//Update the num of cards that the player have
	num = 1 + rand() % 14;// Get random num betwwen 1 to 14 
	color = 1 + rand() % 4;
	CardFreq[num - 1].count++;
	players[CurrentPlyr].cardInfo[*NofCard - 1].num = num;
	if (num == 14) // If 14 so the card is color and we dont need to put value in color  struct
		players[CurrentPlyr].cardInfo[*NofCard - 1].color = ' ';
	else
		players[CurrentPlyr].cardInfo[*NofCard - 1].color = GetColor(color);
	players[CurrentPlyr].cardInfo[*NofCard - 1].color;
}
/*
//The function checks if the card that the player wants to put if it is proper according to the rules
//The function gets array card of the player, upper card ,
the number of the card he selected from his cards and the number of his cards
*/
int CheckCard(CARD_INFO* PlayerCard, CARD_INFO* Uppercard, int curntCard, int* NofCard)
{
	int found;
	//Checks if the card selected by the player matches the color or number of the upper card
	//or if it is a color card
	if ((PlayerCard + curntCard)->color == Uppercard->color || (PlayerCard + curntCard)->num == Uppercard->num
		|| (PlayerCard + curntCard)->num == COLOR_CARD_ID)
	{
		//If the selection is correct, the upper card should be replaced with the selected card
		found = (PlayerCard + curntCard)->num;
		SwapUpperCard(PlayerCard, Uppercard, curntCard);
		SwapCards(PlayerCard, curntCard, NofCard);//"Remove" the selected card from the players array of cards
		return (found);// Return the number of the card 
	}
	else// If not return -1 
		return NOT_FOUND;
}
/*
//The function replace the upper card with other card
//The function gets array card of the player, upper card ,
the number of the card he selected from his cards
*/
void SwapUpperCard(CARD_INFO* PlayerCard, CARD_INFO* Uppercard, int curntCard)
{
	Uppercard->num = (PlayerCard + curntCard)->num;
	Uppercard->color = (PlayerCard + curntCard)->color;
}
/*
//The function "deletes" the selected card from the array of cards
//The function gets array card of the player,
the number of the card he selected from his cards and the number of his cards
*/
void SwapCards(CARD_INFO* PlayerCard, int curntCard, int* NofCard)
{
	//If the selected card is the last card in the array then subtract the amount of cards in one
	if ((curntCard + 1) == *NofCard)
		*NofCard = *NofCard - 1;
	else//else copy the last card data to the location of the selected card
		*NofCard = *NofCard - 1;
	(PlayerCard + curntCard)->num = (PlayerCard + (*NofCard))->num;
	(PlayerCard + curntCard)->color = (PlayerCard + (*NofCard))->color;
}
/*
//The function changes the direction of the game depending on the card the player puts
//The function gets the struct of game direction, thetaype (number) of the card,upper card, number of player in the game
*/
void CheckDirection(GAMEDIRECTION* GameDirection, int CardId, int PlayerId)
{
	switch (CardId)
	{
	case PLUSE_CARD_ID://If the player puts the PLUSE card then he has to choose another card to put
		GameDirection->NumOfPlayersToJump = 0;
		break;
	case STOP_CARD_ID://If the player chooses a stop card then one player should be promoted
		GameDirection->NumOfPlayersToJump = 2;
		break;
	case WAY_CHANGE_CARD_ID://If the player has chosen a way change card then the direction of the game should be negative
	{
		if (PlayerId == MIN_NUM_OF_PLAYERS)// If only 2 players in the game so it is the turn of the current player to play again
		{
			GameDirection->NumOfPlayersToJump = 0;
			GameDirection->direction *= -1;
		}
		else
		{
			GameDirection->NumOfPlayersToJump = 1;
			GameDirection->direction *= -1;
		}
		break;
	}
	default:// If it taki card or rgeular card so the players progress is normal
		GameDirection->NumOfPlayersToJump = 1;
		break;
	}
}
/*
//The function checks what color the user has chosen to put in the upper card and update it
*/
void HandleColorCard(CARD_INFO* upperCard)
{
	int Color = NOT_FOUND;
	while (Color == NOT_FOUND)
	{
		printf("\nPlease enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
		scanf("%d", &Color);
		if (Color >= MAX_COLOR || Color <= INCORRECT_SELCTION)// If more than 4 or less then 1
		{
			Color = NOT_FOUND;
			printf("Invalid color! Try again.\n");
		}
	}
	upperCard->color = GetColor(Color);
}
/*
//The function handles in case the player chooses to put taki card
//The function gets array card of the player, upper card and current player number
*/
int HandleTakiCard(PLAYER* players, CARD_INFO* upperCard, int CurrentPlyr)
{
	int choice = NOT_FOUND, found = TAKI_CARD_ID;
	PrintTurnDetails(players, upperCard, CurrentPlyr);//Print the upper card and the player cards
	while (1 > 0)//Enter the loop all the time, inside the loop there is an option to exit a function
	{
		printf("\nPlease enter 0 if you want to finish your turn");
		printf("\nor 1-%d if you want to put one of your cards in the middle:\n", (players + CurrentPlyr)->nofcard);
		scanf("%d", &choice);
		if (choice != 0)//If the Player wants to finish so he need to choose 0
		{
			found = NOT_FOUND;

			if (choice <= (players + CurrentPlyr)->nofcard && choice > 0)// If the choice is on range
			{
				//Checks if the card selected by the player matches the color of the upper card
				if ((players + CurrentPlyr)->cardInfo[choice - 1].color == upperCard->color)// If 
				{
					found = (players + CurrentPlyr)->cardInfo[choice - 1].num;// Update found
					SwapUpperCard((players + CurrentPlyr)->cardInfo, upperCard, choice - 1);
					SwapCards((players + CurrentPlyr)->cardInfo, choice - 1, &((players + CurrentPlyr)->nofcard));
				}
			}
			if ((players + CurrentPlyr)->nofcard == 0)// If the player and the cards that he have
				return(found);
			else if (found == NOT_FOUND)//If the player has not selected a suitable card then the variable has not changed
				printf("Invalid card! Try again.\n");
			else
				PrintTurnDetails(players, upperCard, CurrentPlyr);

		}
		else
		{
			return found;
		}
	}

}
/*
The function get the number of the current player and num of player in the game
And change the num of the current player depending on the direction
*/
void CheckCurrntPlayerRang(int* currentplyr, int NumOfPlayer)
{
	if (*currentplyr >= NumOfPlayer)// If the number of the current player is bigger then num of player
		*currentplyr -= NumOfPlayer;// So the direction is positive and we need to change to the first player
	else if (*currentplyr < FIRST_PLAYER)//Else if the number of the current player is smaller then 0
		*currentplyr += NumOfPlayer;// So the direction is negetive and we need to change to the last player
}
/*
//The function increases the array of the cards
//The function gets array card of the player,the current size and the new size
*/
CARD_INFO* alloc(CARD_INFO* CurrentArr, int size, int newSize)
{
	CARD_INFO* newArr = NULL;
	int i;
	newArr = (CARD_INFO*)malloc((sizeof(CARD_INFO)) * newSize);
	//Check if memory allocation succeeded
	if (newArr == NULL)
		exit(1);
	else
	{
		//Copy arr into newArr
		for (i = 0; i < size; i++)
		{
			(newArr + i)->color = (CurrentArr + i)->color;
			(newArr + i)->num = (CurrentArr + i)->num;
		}
		//Free allocated memory of arr

		free(CurrentArr);
	}
	return newArr;
}
/*
//The function sorts the array of statistics by the number of times each card appeared
//The function gets the statistics array and size of the array
*/
void mergeSort(FREQNC arr[], int size)
{
	FREQNC* tmpArr = NULL;
	if (size <= 1)
		return;

	mergeSort(arr, size / 2);
	mergeSort(arr + size / 2, size - size / 2);
	tmpArr = merge(arr, size / 2, arr + size / 2, size - size / 2);
	if (tmpArr)
	{
		copyArr(arr, tmpArr, size); // copy values from tmpArr to arr
		free(tmpArr);
	}
	else
	{
		printf("Memory allocation failure!!!\n");
		exit(1);	// end program immediately with code 1 (indicating an error)
	}
}
/*
//The function takes two halves of arrays and merges between them
*/
FREQNC* merge(FREQNC a1[], int size1, FREQNC a2[], int size2)
{
	int ind1, ind2, resInd;
	FREQNC* res = (FREQNC*)malloc((size1 + size2) * sizeof(FREQNC));

	if (res)
	{
		ind1 = ind2 = resInd = 0;

		while ((ind1 < size1) && (ind2 < size2)) {
			if (a1[ind1].count > a2[ind2].count)
			{
				res[resInd].count = a1[ind1].count;
				res[resInd].value = a1[ind1].value;
				ind1++;
			}
			else
			{
				res[resInd].count = a2[ind2].count;
				res[resInd].value = a2[ind2].value;
				ind2++;
			}
			resInd++;
		}

		while (ind1 < size1)
		{
			res[resInd].count = a1[ind1].count;
			res[resInd].value = a1[ind1].value;
			ind1++;
			resInd++;
		}
		while (ind2 < size2)
		{
			res[resInd].count = a2[ind2].count;
			res[resInd].value = a2[ind2].value;
			ind2++;
			resInd++;
		}
	}
	return res;
}
/*
//The function copies the src array to dest array
*/
void copyArr(FREQNC dest[], FREQNC src[], int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		dest[i].count = src[i].count;
		dest[i].value = src[i].value;
	}
}
/*
//The function prints the frequency with which each card appeared
//The function gets the statistics array and size of the array
*/
void PrintFREQNCDetails(FREQNC arr[], int size)
{
	int i = 0;
	printf("\n************ Game Statistics ************\n");
	printf("Card #   | Frequency\n__________________\n");
	for (i = 0; i < size; i++)
	{
		if (arr[i].value > 9)
		{
			switch (arr[i].value)
			{
			case PLUSE_CARD_ID: // case 10 print the card PLUSE
				printf("  %s   |    ", PLUSE_TEXT);
				break;
			case STOP_CARD_ID: // case 11 print the card STOP
				printf(" %s |    ", STOP_TEXT);
				break;
			case WAY_CHANGE_CARD_ID:// case 12 print the card <->
				printf("  %s |    ", WAY_CHANGE_TEXT);
				break;
			case TAKI_CARD_ID:// case 13 print the card TAKI
				printf(" %s |    ", TAKI_TEXT);
				break;
			case COLOR_CARD_ID:// case 14 print the card COLOR
				printf(" %s|    ", COLOR_TEXT);
				break;
			}
		}
		else
			printf("  %d   |    ", arr[i].value);
		printf("%d\n", arr[i].count);

	}
}