//
//  List.h
//  testt
//
//  Created by Burak on 15/04/2017.
//  Copyright © 2017 Burak. All rights reserved.
//

#ifndef List_h
#define List_h

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct flight{
    int id;
    int seat_count;
    struct flight * next;
    pthread_mutex_t mutex;
    int counts_of_solds;
    int counts_of_reserved;
    int counts_of_cancelled;
};

struct airline{
    int id;
    struct airline * next;
    struct flight* flight;
};

struct reservation{
    int transaction_id;
    struct reservation *next;
};

struct customer{
    int id;
    int max_operation;
    int max_reserve;
    int counts_of_buy;
    int counts_of_reserve;
    int counts_of_cancel;
    int counts_of_nothing;
    int r;
    pthread_mutex_t mutex;
    struct customer * next;
    struct reservation * reservation;
};

struct ticket_officer{
    int id;
    int counts_of_buy;
    int counts_of_reserve;
    int counts_of_cancel;
    int counts_of_nothing;
    pthread_mutex_t mutex;
    pthread_mutex_t customer;
    pthread_mutex_t customer_request;
    struct ticket_officer * next;
    struct request * request;
};

struct transaction{
    int id;
    int customer_id;
    int officer_id;
    int operation;
    int simulation_day;
    int success;
    int airline_id;
    int flight_id;
    int seat_count;
    struct transaction * next;
};

struct request{
    int customer_id;
    int operation;
    int airline_id;
    int flight_id;
    int seat_count;
    int transaction_id;
    struct request * next;
};

extern struct customer *customer_list, *insert_list, *temp;
extern struct airline *airline_list, *airline_insert_list, *airline_temp;
extern struct ticket_officer *ticket_officer_list, *ticket_officer_insert_list, *ticket_officer_temp;
extern struct transaction *transaction_list;
extern struct request *request_list, *request_list_back;

struct customer* search(int id);
struct ticket_officer* search_ticket_officer(int id);
struct airline* search_airline(int id);
struct transaction* search_transaction(int id);
struct flight* search_flight(int id, int airline_id);
void set_customers(int id, int max_operation, int max_reserve);
void set_ticket_officer(int id);
int set_transactions(int trans_id, int customer_id, int officer_id, int operation, int simulation_day, int success, int airline_id, int flight_id, int seat_count);
void set_request_tt(int customer_id, int operation, int airline_id, int flight_id, int seat_count, int transaction_id, int ticket_officer_id);
void set_airlines(int id);
void set_flights(int id, int airline_id);
void set_reservations(int transaction_id, int customer_id);


#endif /* List_h */
