#define _CRT_SECURE_NO_WARNINGS
#define ARR_SIZE 21
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <crtdbg.h>

typedef struct player {
	char name[ARR_SIZE]; // player's name, up to 20 chars
	int age; // player's age, 0 - 120
	int defense; // defense power, 1 - 100
	int attack; // attack power, 1 - 100
	struct player *next; // next player in the list
}player;

typedef struct team {
	char name[ARR_SIZE]; // team's name
	player *teamPlayers; // pointer to the team players list
	int points; // the team points in the league
	int wins; // number of wins
	int draws; // number of draws
	int losses; // number of losses
	struct team *next; // Next team in the list
}team;

void free_teams(team* head)	//free memory
{
	team* temp;
	player* temp1;
	while (head != NULL)
	{
		temp = head;
		while (head->teamPlayers != NULL)
		{
			temp1 = temp->teamPlayers;
			temp->teamPlayers = temp->teamPlayers->next;
			free(temp1);
		}

		head = head->next;
		free(temp);
	}
}

team* create_new_team(team* head, char* name)	//create new team
{
	team *std = (team*)malloc(sizeof(team));
	if (std != NULL)
	{
		strcpy(std->name, name);
		std->next = NULL;
		std->points = 0;
		std->wins = 0;
		std->draws = 0;
		std->losses = 0;
		std->teamPlayers = NULL;
	}
	else
	{
		printf("allocation failed");
		free_teams(head);
		exit(1);
	}
	return std;
}

player* create_new_player(team* head, char* player_name, char* player_age_s, char* player_attack_s, char* player_defense_s, char* player_team)
{
	player *std = (player*)malloc(sizeof(player));
	if (std != NULL)
	{
		strcpy(std->name, player_name);
		std->age = char_to_int(player_age_s);
		if (std->age > 120 || std->age < 0)
			std->age = 20;
		std->attack = char_to_int(player_attack_s);
		if (std->attack > 100 || std->attack < 1)
			std->attack = 50;
		std->defense = char_to_int(player_defense_s);
		if (std->defense > 100 || std->defense < 1)
			std->defense = 50;
		std->next = NULL;
	}
	else
	{
		printf("Allocation failed");
		free_teams(head);
		exit(1);
	}
	return std;
}

team* add_team(team* head, char* name)	
{
	team* iter, *prev = NULL, *new_team = NULL;
	new_team = create_new_team(head, name);
	if (head == NULL)
	{
		head = new_team;
		return head;
	}
	if (strcmp(new_team->name, head->name) < 0)
	{
		new_team->next = head;
		head = new_team;
		return head;
	}
	iter = head;
	while (iter != NULL && strcmp(new_team->name, iter->name) >= 0)
	{
		if (strcmp(new_team->name, iter->name) == 0)	//if it's the same name , do nothing
			return head;
		prev = iter;
		iter = iter->next;
	}
	prev->next = new_team;
	new_team->next = iter;
	return head;
}
int NumberofTeams(team* head)
{
	int count = 0;
	while (head != NULL)
	{
		count++;
		head = head->next;
	}
	return count;
}
team* sort_by_pts(team* head)
{
	int num_team = NumberofTeams(head);
	int counter;
	if (head == NULL)
		return head;
	team* curr = head;
	team* next = NULL;
	next = curr->next;
	team* prev = NULL;
	for (counter = 0; counter < num_team; counter++)
	{
		curr = head;
		next = curr->next;
		prev = NULL;
		while (next != NULL)
		{
			if (curr->points < next->points) {
				if (curr == head) {
					head = next;
				}
				else {
					prev->next = next;
				}
				curr->next = next->next;
				next->next = curr;

				prev = next;
				next = curr->next;
			}
			else {
				prev = curr;
				curr = curr->next;
				next = curr->next;
			}
		}
	}
	return head;
}

team* PrintTable(team *head, FILE *fp)
{
	team *ptr,*fptr;
	int rank = 1;
	ptr = sort_by_pts(head);
	fptr = ptr;
	fprintf(fp, "Rank	Team name            	Matches played	Wins	Draws	Losses	Points\n");
	for (ptr; ptr != NULL; ptr = ptr->next)
	{
		fprintf(fp, "%-7d %-23s %-16d%-8d%-8d%-8d%-8d \n",rank,ptr->name,ptr->wins+ptr->draws+ptr->losses,ptr->wins,ptr->draws,ptr->losses,ptr->points);
		rank++;
	}
	return fptr;
}

team* add_player(team* head, char* name, char* age, char* attack, char* defense, char* team_name)
{
	team* ptr = head;
	player* iter, *prev = NULL, *new_player = NULL;
	new_player = create_new_player(head,name,age,attack,defense,team_name);
	while (ptr!=NULL && strcmp(ptr->name,team_name)!=0 )
		ptr = ptr->next;
	if (ptr!=NULL && strcmp(ptr->name, team_name) == 0)
	{
		if (ptr->teamPlayers == NULL)
		{
			ptr->teamPlayers = new_player;
			return head;
		}
		if (strcmp(new_player->name, ptr->teamPlayers->name) < 0)
		{
			new_player->next = ptr->teamPlayers;
			ptr->teamPlayers = new_player;
			return head;
		}
		iter = ptr->teamPlayers;
		while (iter != NULL && strcmp(new_player->name, iter->name) >= 0)
		{
			if (strcmp(new_player->name, iter->name) == 0)
				return head;
			prev = iter;
			iter = iter->next;
		}
		prev->next = new_player;
		new_player->next = iter;
		return head;
	}
	return head;
}
team* read_teams_file(FILE *fp,team *head, char* team_name)
{
	int i = 0;
	char c = 0;
	c = getc(fp);
	while ((c != '\n') && (!feof(fp)))
	{
		for (i; (c != '\n') && (!feof(fp)); i++)
		{
			team_name[i] = c;
			c = getc(fp); //building team name
		}
		team_name[i] = '\0';
		return add_team(head, team_name);
	}
	return head;
}

int ComputeStrength(team A)
{
	int strength = 0;
	for (A.teamPlayers; A.teamPlayers != NULL;) {
		strength += A.teamPlayers->attack + A.teamPlayers->defense;
		A.teamPlayers = A.teamPlayers->next;
	}

	return strength;
}

team* PlayGame(team* head,FILE *results,char* team_name_A,char* team_name_B)
{
	team* teamA=head;
	team* teamB = head;
	int powerA = 0, powerB = 0;
	if (head==NULL)
	{
		fprintf(results, "The game ~%s:%s~ can't play\n", team_name_A, team_name_B);
		return head;
	}
	if (strcmp(team_name_A, team_name_B) == 0)
	{
		fprintf(results, "The game ~%s:%s~ can't play\n", team_name_A, team_name_B);
		return head;
	}
	while (teamA != NULL)
	{
		if (strcmp(teamA->name,team_name_A)==0)
		{
			while (teamB != NULL)
			{
				if (strcmp(teamB->name, team_name_B) == 0)
				{
					powerA = ComputeStrength(*teamA);
					powerB = ComputeStrength(*teamB);
					if (powerA > powerB)
					{
						if (teamA->teamPlayers == NULL || teamB->teamPlayers == NULL)
						{
							fprintf(results, "The game ~%s:%s~ can't play\n", team_name_A, team_name_B);
							return head;
						}
						teamA->points += 3;
						teamA->wins++;
						teamB->losses++;
						fprintf(results, "Team %s WON the game\n",teamA->name);
						return head;
					}
					if (powerA < powerB)
					{
						if (teamA->teamPlayers == NULL || teamB->teamPlayers == NULL)
						{
							fprintf(results, "The game ~%s:%s~ can't play\n", team_name_A, team_name_B);
							return head;
						}
						teamB->points += 3;
						teamB->wins++;
						teamA->losses++;
						fprintf(results, "Team %s WON the game\n", teamB->name);
						return head;
					}
					if (powerA==powerB)
					{
						if (teamA->teamPlayers == NULL || teamB->teamPlayers == NULL)
						{
							fprintf(results, "The game ~%s:%s~ can't play\n", team_name_A, team_name_B);
							return head;
						}
						teamA->points++;
						teamB->points++;
						teamA->draws++;
						teamB->draws++;
						fprintf(results, "Team %s and Team %s finished the match in a draw\n",teamA->name, teamB->name);
						return head;
					}
				}
				teamB = teamB->next;
			}
		}
		teamA = teamA->next;
	}
	fprintf(results,"The game ~%s:%s~can't play\n", team_name_A, team_name_B);
	return head;
}

team* ReadGameFile(FILE* fp,FILE* results, team* head, char* team_name_A,char* team_name_B)
{
	int i = 0,k=0;
	char c = 0;
	c = getc(fp);
	while ((!feof(fp)) && (c!='\n'))
	{
		for (i=0; (c != '\n' && c != ':' && (!feof(fp))); i++)
		{
			team_name_A[i] = c;
			c = getc(fp);
		}
		team_name_A[i] = '\0';
		c = getc(fp);
		for (k = 0; (c != '\n' && c != ':' && (!feof(fp))); k++)
		{
			team_name_B[k] = c;
			c= getc(fp);
		}
		team_name_B[k] = '\0';
		return PlayGame(head,results, team_name_A, team_name_B);
	}
	return head;
}

int char_to_int(char *str)
{
	int x = 0, i;
	for (i = 0; i < strlen(str); i++)
		if (str[i] != 32)
		{
			x = x * 10 + str[i] - '0';
		}
	return x;
}

team* read_players_file(FILE* fp, team *head, char *player_name, char *player_age_s, char *player_attack_s, char *player_defense_s, char *player_team)
{
	int  j, k, s, t, w;
	int player_defense = 0, player_age = 0, player_attack = 0;
	char c = 0;
	c = getc(fp);
	while ((c != '\n') && (!feof(fp)))
	{
		for (k = 0; k < 20; k++)
		{
			player_name[k] = c;
			c = getc(fp);
		}
		player_name[k] = '\0';
		c = getc(fp);
		for (j = 0; j < 4; j++)
		{
			player_age_s[j] = c;
			c = getc(fp);
		}
		player_age_s[j] = '\0';
		c = getc(fp);
		for (s = 0; s < 6; s++)
		{
			player_attack_s[s] = c;
			c = getc(fp);
		}
		player_attack_s[s] = '\0';
		c = getc(fp);
		for (t = 0; t < 6; t++)
		{
			player_defense_s[t] = c;
			c = getc(fp);
		}
		player_defense_s[t] = '\0';
		c = getc(fp);
		c = getc(fp);
		for (w = 0; c != '\n' && (!feof(fp)); w++)
		{
			player_team[w] = c;
			c = getc(fp);
		}
		player_team[w] = '\0';
		return add_player(head, player_name, player_age_s, player_attack_s, player_defense_s, player_team);
	}
	return head;
}

int main(int argc, char* argv[])
{
	FILE *fp_teams = NULL, *fp_players = NULL, *fp_games = NULL,*fp_results=NULL,*fp_table=NULL;
	team* head = NULL;
	char team_name[ARR_SIZE],team_name1[ARR_SIZE], player_name[ARR_SIZE],age_s[5],attack_s[7],defense_s[7];
	
	if (argc != 5)
	{
		printf("Not 5 argc");
		return 1;
	}
	if ((fp_teams = fopen(argv[1], "r")) == NULL)
	{
		printf("File did not open. Exit..\n");
		return 1;
	}
	if ((fp_players = fopen(argv[2], "r")) == NULL)
	{
		printf("File did not open. Exit..\n");
		fclose(fp_teams);
		return 1;
	}
	fseek(fp_players,52,SEEK_SET);
	if ((fp_games = fopen(argv[3], "r")) == NULL)
	{
		printf("File did not open. Exit..\n");
		fclose(fp_teams);
		fclose(fp_players);
		return 1;
	}
	if ((fp_table = fopen(argv[4], "w")) == NULL)
	{
		printf("File did not open. Exit..\n");
		fclose(fp_teams);
		fclose(fp_players);
		fclose(fp_games);
		return 1;
	}
	if ((fp_results = fopen("results.txt", "w+")) == NULL)
	{
		printf("File did not open. Exit..\n");
		fclose(fp_teams);
		fclose(fp_players);
		fclose(fp_games);
		fclose(fp_table);
		return 1;
	}
	while (!feof(fp_teams))
	{
		head = read_teams_file(fp_teams, head, team_name);
	}
	fclose(fp_teams);
	while (!feof(fp_players))
		head = read_players_file(fp_players,head,player_name,age_s,attack_s,defense_s,team_name);
	fclose(fp_players);
	while (!feof(fp_games))
		head = ReadGameFile(fp_games, fp_results, head, team_name, team_name1);
	fclose(fp_games);
	fclose(fp_results);
	head=PrintTable(head, fp_table);
	fclose(fp_table);
	free_teams(head);
	return 0;
}