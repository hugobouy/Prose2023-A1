/**
 * @file         clock.c
 * @brief        Module used to check if an employee is recognized or not, and if he is allowed to come in.
 * @author         JURET Paul
 * @date         05/06/2023
 *
 * \b License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */


#include "clock.h"
//#include "common.h"
#include <string.h>
char output[100];

static Time currentTime;


Time clock_getCurrentTime()
{
	FILE * fp = popen("date","r"); 						//lance la commande bash dans un processus fils
	if (fp == NULL )
	{
		perror("erreur commande date");						
	}
	fgets(output, 100, fp);									//prend la sortie de la commande sur ce processus fils
	pclose(fp);												//ferme le processus fils à la fin de la commande

	char day[4], month[4], time[9], year[5];
    int dayNum;
	uint8_t RTC_heure;
	uint8_t RTC_minutes;
	uint8_t RTC_seconde;
	uint16_t RTC_year;

    sscanf(output, "%3s %3s %d %8s UTC %4s", day, month, &dayNum, time, year); //découpage de la chaine 
    sscanf(time, "%hhu:%hhu:%hhu", &RTC_heure, &RTC_minutes, &RTC_seconde);


    uint8_t monthNum = 1; // Valeur par défaut pour un mois invalide

    if (strcmp(month, "Jan") == 0) monthNum = 1;
    else if (strcmp(month, "Feb") == 0) monthNum = 2;
    else if (strcmp(month, "Mar") == 0) monthNum = 3;
    else if (strcmp(month, "Apr") == 0) monthNum = 4;
    else if (strcmp(month, "May") == 0) monthNum = 5;
    else if (strcmp(month, "Jun") == 0) monthNum = 6;
    else if (strcmp(month, "Jul") == 0) monthNum = 7;		//comvertisseur du mois en chiffre
    else if (strcmp(month, "Aug") == 0) monthNum = 8;
    else if (strcmp(month, "Sep") == 0) monthNum = 9;
    else if (strcmp(month, "Oct") == 0) monthNum = 10;
    else if (strcmp(month, "Nov") == 0) monthNum = 11;
    else if (strcmp(month, "Dec") == 0) monthNum = 12;

	Day dayTag=MONDAY;

    if (strcmp(day, "Mon") == 0) dayTag = MONDAY;
    else if (strcmp(day, "Tue") == 0) dayTag = TUESDAY;
    else if (strcmp(day, "Wed") == 0) dayTag = WEDNESDAY;
    else if (strcmp(day, "Thu") == 0) dayTag = THURSDAY;		//comvertisseur du jour en enum
    else if (strcmp(day, "Fri") == 0) dayTag = FRIDAY;
    else if (strcmp(day, "Sat") == 0) dayTag = SATURDAY;
    else if (strcmp(day, "Sun") == 0) dayTag = SUNDAY;	

    sscanf(year, "%hu", &RTC_year); 					//convertisseur de l'année en nombre

	currentTime.dayOfWeek = dayTag;
	currentTime.day	      = dayNum;
	currentTime.month     = monthNum;
	currentTime.hour      = RTC_heure;
	currentTime.minute    = RTC_minutes;
	currentTime.second    = RTC_seconde;
	currentTime.year      = RTC_year;

	return currentTime;
}

void clock_setCurrentTime(Time time)
{
	char * month="Jan";   

	if (time.month == 1) month = "Jan";
	else if (time.month == 2) month = "Feb";
	else if (time.month == 3) month = "Mar";
	else if (time.month == 4) month = "Apr";
	else if (time.month == 5) month = "May";
	else if (time.month == 6) month = "Jun";
	else if (time.month == 7) month = "Jul";			//conversion du mois en lettres
	else if (time.month == 8) month = "Aug";
	else if (time.month == 9) month = "Sep";
	else if (time.month == 10) month = "Oct";
	else if (time.month == 11) month = "Nov";
	else if (time.month == 12) month = "Dec";


	char number[2];
	sprintf(number,"%hhu", time.day);
	char newTime[12];
	sprintf(newTime,"%hhu:%hhu:%hhu",time.hour,time.minute, time.second);
	char year[4];
	sprintf(year, "%hu",time.year);

	char commandSET[100];
	char chosenDate[50];
	snprintf(chosenDate, 50, " %s %s %s UTC %s", month, number, newTime, year);			//concaténation de la première partie de la commande à envoyer pour set la date voulue

	snprintf(commandSET, 100, "date --set='%s'",chosenDate);					//concaténation de la seconde partie de la commande

	FILE * fp = popen(commandSET,"r"); 								//lance la commande bash dans un processus fils							
	if (fp == NULL )
	{
		perror("erreur commande date");
	}
	TRACE("\nla date à été modifiée\n");
}

