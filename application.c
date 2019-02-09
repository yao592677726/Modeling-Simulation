#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sim.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Discrete Event Simulation
// Author: Chengxi Yao
//
/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
//
// State variables  and other global information
//
/////////////////////////////////////////////////////////////////////////////////////////////


int	At_Line = 0;        // number of customers waiting in line to get connected
int SE_Avil = 0;      // number of service agents available
int In_Answered = 0;    // number of incoming guests answered immediately
int In_Total = 0;       // total number of incoming guests
int In_Hangup = 0;		// number of incoming guests where the caller hangs up
int In_Wait = 0;		// number of incoming guests where the caller chooses to wait
int In_Selfserve = 0;   // number of incoming guests who choose to self-serve
int Wait_Solved = 0;	// number of customers in line that have been answered
double* wait_start;				// array used to store the starting time for each guest in the line
double* wait_Time;			// array used to store the total waiting time for each guest in the line
int chat_started = 0;   // count number of conversations with guests started
int chat_finished = 0;  // count number of conversations with guests finished
int* idle_SE;			// number of idle SE every 60s
double hourly_costs;
double hourly_income;
double poisson_A;


// Event types
#define	GUEST_ARRIVAL          1
#define	INCALL_FINISHEDBYSE     2

#define MAX_RAND                9999    // for generating a random number PoIsson Distribution
#define AGENT_A_CAPACITY        5
#define AGENT_B_CAPACITY        2
#define AGENT_C_CAPACITY        1
#define AGENT_A_SALARY          32
#define AGENT_B_SALARY          25
#define AGENT_C_SALARY          16
#define FIXED_COSTS             6


// #define OUTCALL_NUMBER          5       // number of outgoing guests every 60 seconds
// #define S_TRUE                  75      // percentage of outgoing guests that are successful



/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data structures for event data
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Events only have one parameter, the event type
struct EventData {
	int EventType;
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////////////////////////////////

// prototypes for event handlers
void Incall_Arrival (struct EventData *e);		// incoming guests
void Incall_FinishedbySE (struct EventData *e);	// incoming guests finished by service agents
void Initial_global (void);
double Count_Profit (int starthour, double lamda[], int a, int b, int c);
void Print_Hourly_Stats(int starthour, double lamda[], int a, int b, int c);



/////////////////////////////////////////////////////////////////////////////////////////////
//
// Event Handlers
// Parameter is a pointer to the data portion of the event
//
/////////////////////////////////////////////////////////////////////////////////////////////

// General Event Handler Procedure define in simulation engine interface
void EventHandler (void *data)
{
    struct EventData *d;
    
    // coerce type
    d = (struct EventData *) data;
    // call an event handler based on the type of event
    do{
    if (d->EventType == GUEST_ARRIVAL) Incall_Arrival (d);
    if (d->EventType == INCALL_FINISHEDBYSE) Incall_FinishedbySE (d);
    } while(0);
    // else {fprintf (stderr, "Illegal event found\n"); exit(1); }
}

// event handler for incoming guests
void Incall_Arrival (struct EventData *e) {
	struct EventData *d;
	double ts;
    double order_value;

    In_Total++;
    // printf("%d\n", SE_Avil);


    // printf ("Incoming guest at time %f, At_Line=%d, SE_Avil=%d\n", CurrentTime(), At_Line, SE_Avil);
	if (e->EventType != GUEST_ARRIVAL) {fprintf (stderr, "Unexpected event type\n"); exit(1);}

	// schedule next arrival event, here
    if((d=malloc(sizeof(struct EventData)))==NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GUEST_ARRIVAL;
    ts = CurrentTime() - log(1-rand()%(MAX_RAND+1)/(double)(MAX_RAND+1)) / poisson_A;
    Schedule (ts, d);
    // PrintList();


    // the visitor has 7.85% of probability to talk to an agent
    if(rand()%10000 < 785){
    	if (SE_Avil > 0) {
            In_Answered++;
    		if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    		d->EventType = INCALL_FINISHEDBYSE;
    		ts = CurrentTime() + (rand()%(MAX_RAND+1)/(double)(MAX_RAND+1) * 400 + 500);
    		Schedule (ts, d);
            SE_Avil--;
            wait_Time[chat_started] = 0;
            chat_started++;

    	}
        //  if no agents are available, the guest chooses to either wait in line or hang out
        else{
            if(rand()%100 < 75){
            At_Line++;   // update state variable, the guest will remain on the line with probability 0.75
            wait_start[In_Wait] = CurrentTime();	
            In_Wait++;
            }
            else{
            	In_Hangup++;
                // printf("Guest leaves the line.\n");
            }
        }
    } else {
        In_Selfserve++;
        // printf("Guest choose to self-serve\n");
        if(rand()%100 < 1) {
            order_value = (rand()%(MAX_RAND+1)/(double)(MAX_RAND+1) * 100 + 50);
            hourly_income += order_value * 0.3;
            hourly_costs += FIXED_COSTS + order_value * 0.05;
            // printf("guest place order of %lf\n", order_value);
        }
    }    

	free (e);	// don't forget to free storage for event!
}

// event handler for incoming guests finishedy by service agents
void Incall_FinishedbySE (struct EventData *e)
{
    struct EventData *d;
    double ts;
    double p;   //  willingness to place the ordeer
    double order_value;
    
    // printf ("Incoming guest answered by SE at time %f, At_Line=%d, SE_Avil=%d\n", CurrentTime(), At_Line, SE_Avil);
    // printf ("Wait time of current guest is %lf\n", wait_Time[chat_finished]);
	if (e->EventType != INCALL_FINISHEDBYSE) {fprintf (stderr, "Unexpected event type\n"); exit(1);}

	SE_Avil++;   // one fewer vehicle at pump
    p = 1 -0.0000625 * wait_Time[chat_finished] * wait_Time[chat_finished] -0.00375 * wait_Time[chat_finished];
    if(p > 1) p = 1;
    // printf("waiting time %lf, %lf\n", wait_Time[chat_finished], p);
    if(p > 0 && rand()%100 < 40 * p){
        order_value = (rand()%(MAX_RAND+1)/(double)(MAX_RAND+1) * 100 + 50);
        hourly_income += order_value * 0.3;
        hourly_costs += FIXED_COSTS + order_value * 0.05 + order_value * 0.03;
        // printf("guest place order of %lf\n", order_value);
    }

    chat_finished++;
    // if()

    // If another guest is waiting in line, schedule its finish event
    if (At_Line>0) {
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = INCALL_FINISHEDBYSE;
        ts = CurrentTime() + (rand()%(MAX_RAND+1)/(double)(MAX_RAND+1) * 400 + 500);
        Schedule (ts, d);
        wait_Time[chat_started] = CurrentTime() - wait_start[Wait_Solved];
        chat_started++;
        Wait_Solved++;
        At_Line--;
        SE_Avil--;
    }
	free (e);	// release memory for data part of event
}

// initialize all global variables
void Initial_global (void) {
    At_Line = 0;        // number of customers waiting in line to get connected
    In_Answered = 0;    // number of incoming guests answered immediately
    In_Total = 0;       // total number of incoming guests
    In_Hangup = 0;      // number of incoming guests where the caller hangs up
    In_Wait = 0;        // number of incoming guests where the caller chooses to wait
    In_Selfserve = 0;
    Wait_Solved = 0;    // number of customers in line that have been answered
    chat_started = 0;   // count number of conversations with guests started
    chat_finished = 0;  // count number of conversations with guests finished
}

// Calculate the total profit in current time period with a A-level agents, b B-level agents and c C-level agents
double Count_Profit (int starthour, double lamda[], int a, int b, int c) {
    struct EventData *d, *e;
    double ts;
    double temp = 0;
    double total_cost = 0;
    double total_income = 0;
    double hourly_salary = 0;
    for(int i = 0; i < 100; i++){
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = GUEST_ARRIVAL;
        ts = 0.0;
        Schedule (ts, d);
        SE_Avil = a * AGENT_A_CAPACITY + b * AGENT_B_CAPACITY + c * AGENT_C_CAPACITY;
        total_income = 0;
        total_cost = 0;
        Initial_global();
        for (int hour = 0; hour < 4; hour++){
            poisson_A = lamda[starthour+hour];
            hourly_costs = 0;
            hourly_income = 0;
            RunSim((hour+1) * 3600.0);
            // printf("here");
            hourly_salary = a * AGENT_A_SALARY + b * AGENT_B_SALARY + c * AGENT_C_SALARY;
            total_cost += hourly_costs;
            total_cost += hourly_salary;
            total_income += hourly_income;
            // printf("number of total guest: %d\n", In_Total);
            // printf("From %d:00 to %d:00, In_Answered: %d, In_Wait: %d, In_Hangup: %d, Income: %lf, Cost: %lf, Profit: %lf\n", hour+9, hour+10, In_Answered, In_Wait, In_Hangup, hourly_income, hourly_costs, hourly_income - hourly_costs);
        }
        temp += total_income - total_cost;
        RemoveAll();
    }
    return temp / 100;
}

void Print_Hourly_Stats(int starthour, double lamda[], int a, int b, int c) {
    struct EventData *d, *e;
    double ts;
    double count = 0;
    double hourly_salary = 0;
    double avg_income[4];
    double avg_cost[4];


    for (int m = 0; m < 4; m++) {
        avg_income[m] = 0;
        avg_cost[m] = 0;
    }

    for (int m = 0; m < 10; m++) {
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = GUEST_ARRIVAL;
        ts = 0.0;
        Schedule (ts, d);
        SE_Avil = a * AGENT_A_CAPACITY + b * AGENT_B_CAPACITY + c * AGENT_C_CAPACITY;
        Initial_global();
        for (int hour = 0; hour < 4; hour++){
            poisson_A = lamda[starthour+hour];
            hourly_costs = 0;
            hourly_income = 0;
            RunSim((hour+1) * 3600.0);
            // printf("here");
            hourly_salary = a * AGENT_A_SALARY + b * AGENT_B_SALARY + c * AGENT_C_SALARY;

            avg_income[hour] += hourly_income;
            avg_cost[hour] += (hourly_costs + hourly_salary);

            // printf("From %d:00 to %d:00:\n In_Answered: %lf, In_Wait: %lf, In_Hangup: %lf, In_Selfserve: %lf,\n Income: %lf, Cost: %lf, Profit: %lf\n", starthour+9, starthour+10, ((double)In_Answered/In_Total), ((double)In_Wait/In_Total), ((double)In_Hangup/In_Total), ((double)In_Selfserve/In_Total), hourly_income, hourly_costs, hourly_income - hourly_costs);
            // printf ("Statistics of Average Wait_Time: %lf of %d Guests\n", count/chat_finished, chat_finished);
            // printf("number of total guest: %d\n", In_Total);
            // printf("From %d:00 to %d:00, In_Answered: %d, In_Wait: %d, In_Hangup: %d, Income: %lf, Cost: %lf, Profit: %lf\n", hour+9, hour+10, In_Answered, In_Wait, In_Hangup, hourly_income, hourly_costs, hourly_income - hourly_costs);
        }
        RemoveAll();
    }

    for(int i = 0; i < chat_finished; i++){
        count += wait_Time[i];
    }

    for (int m = 0; m < 4; m++) {
        avg_income[m] = avg_income[m]/10;
        avg_cost[m] = avg_cost[m]/10;
        printf("From %d:00 to %d:00:\n Income:%lf, Cost: %lf, Profit: %lf\n", m+starthour+9, m+starthour+10, avg_income[m], avg_cost[m], avg_income[m]-avg_cost[m]);
    }
    printf ("\n\n\nStatistics of Average Wait_Time: %lf of %d Guests\n", count/chat_finished, chat_finished);
    printf ("\n\n\nStatistics of incoming guests:\n In_Answered=%lf\n In_Wait=%lf\n In_Hangup=%lf\n In_Selfserve=%lf\n\n\n", ((double)In_Answered/In_Total), ((double)In_Wait/In_Total), ((double)In_Hangup/In_Total), ((double)In_Selfserve/In_Total));
    // printf("number of total guest: %d\n", In_Total);
}


///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////

int main (void)
{
	wait_start = NULL;
	wait_Time = NULL;
	idle_SE = NULL;
    double total_profit = 0;    // total number of profits
    double daily_profit = 0;

    srand(time(NULL));
    wait_start = (double*) malloc(sizeof(int)*10000);
    wait_Time = (double*) malloc(sizeof(int)*10000);
    idle_SE = (int*) malloc(sizeof(int)*1000);

    // printf("%d\n", SE_Avil);

	// initialize event list with first arrival
	// if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
	// d->EventType = INCALL_ARRIVAL;
	// ts = 0.0;
	// Schedule (ts, d);

    int hour;
    double temp;
    double maxprofit = -1;
    int arr[3];
    double lamda[12] = {4.6905, 6.5952, 8.4762, 10.1810, 9.5524, 8.9976, 8.8357, 9.2857, 10.0500, 9.9119, 11.5571, 13.8690};
    for(int i = 0; i < 12; i++) {
        lamda[i] = lamda[i]/60;
    }

    // for morning arrangement
    for(int a = 1; a <= 2; a++) {
        for(int b = 1; b <= 3; b++) {
            for(int c = 1; c <= 6; c++) {
                // take average of 3 tries
                total_profit = Count_Profit(0, lamda, a, b, c);
                printf("Average total profit: %lf, a = %d, b = %d, c = %d\n", total_profit, a, b, c);
                if(maxprofit == -1 || total_profit > maxprofit) {
                    maxprofit = total_profit;
                    arr[0] = a;
                    arr[1] = b;
                    arr[2] = c;
                }
            }
        }
    }
    daily_profit += maxprofit;
    printf("The best morning arrangement is %d A-level agent, %d B-level agent, %d C-level agent, maxprofit is %lf\n", arr[0], arr[1], arr[2], maxprofit);
    Print_Hourly_Stats(0, lamda, arr[0], arr[1], arr[2]);
     
    // for afternoon arrangement
    for(int a = 1; a <= 2; a++) {
        for(int b = 1; b <= 3; b++) {
            for(int c = 1; c <= 6; c++) {
                // take average of 3 tries
                total_profit = Count_Profit(4, lamda, a, b, c);
                printf("Average total profit: %lf, a = %d, b = %d, c = %d\n", total_profit, a, b, c);
                if(maxprofit == -1 || total_profit > maxprofit) {
                    maxprofit = total_profit;
                    arr[0] = a;
                    arr[1] = b;
                    arr[2] = c;
                }
            }
        }
    }
    daily_profit += maxprofit;
    printf("The best afternoon arrangement is %d A-level agent, %d B-level agent, %d C-level agent, maxprofit is %lf\n", arr[0], arr[1], arr[2], maxprofit);
    
    // print wait_Time
    Print_Hourly_Stats(4, lamda, arr[0], arr[1], arr[2]);

    // printf ("\n\n\nStatistics of incoming guests:\n In_Answered=%lf\n In_Wait=%lf\n In_Hangup=%lf\n In_Selfserve=%lf\n\n\n", ((double)In_Answered/In_Total), ((double)In_Wait/In_Total), ((double)In_Hangup/In_Total), ((double)In_Selfserve/In_Total));
   
    // for evening arrangement
    for(int a = 1; a <= 2; a++) {
        for(int b = 1; b <= 3; b++) {
            for(int c = 1; c <= 6; c++) {
                // take average of 3 tries
                total_profit = Count_Profit(8, lamda, a, b, c);
                printf("Average total profit: %lf, a = %d, b = %d, c = %d\n", total_profit, a, b, c);
                if(maxprofit == -1 || total_profit > maxprofit) {
                    maxprofit = total_profit;
                    arr[0] = a;
                    arr[1] = b;
                    arr[2] = c;
                }
            }
        }
    }

    daily_profit += maxprofit;
    printf("The best evening arrangement is %d A-level agent, %d B-level agent, %d C-level agent, total_profit is %lf\n", arr[0], arr[1], arr[2], total_profit);

    Print_Hourly_Stats(8, lamda, arr[0], arr[1], arr[2]);
   
    printf ("\n\n\nOverall Profit:\n%lf", daily_profit);
    // printf ("\n\n\nStatistics of Average Wait_Time: %lf of %d Guests\n", count/(i+1), chat_finished);

    // double count_idleSE = 0;
   	// for(i = 0; i < time_60s; i++){
    // 	count_idleSE += idle_SE[i];
   	// }
    // printf ("\n\n\nStatistics of idle agents: \n Average number of idle SE agents in 60s: %lf\n", count_idleSE/(time_60s+1));


    // for(i = 0; i < time_60s; i++){
    // 	printf("%d %lf\n", i+1, S_history[i]);
   	// }


}
