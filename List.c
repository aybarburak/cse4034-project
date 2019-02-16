//
//  List.c
//
//  Created by Burak
//  Copyright © 2017 Burak. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "List.h"

int t_id = 1;
pthread_mutex_t lock, transaction, request;

struct customer *customer_list, *insert_list = NULL, *temp = NULL;
struct airline *airline_list, *airline_insert_list = NULL, *airline_temp = NULL;
struct ticket_officer *ticket_officer_list, *ticket_officer_insert_list = NULL, *ticket_officer_temp = NULL;
struct transaction *transaction_list;
struct request *request_list, *request_list_back;


//Search customer, return customer with given id
struct customer* search(int id){
    struct customer *temp;
    temp = customer_list;
    
    while(temp != NULL) {
        if(temp->id == id) {
            return temp;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//Search ticket officer, return ticket officer with given id
struct ticket_officer* search_ticket_officer(int id){
    struct ticket_officer *temp;
    temp = ticket_officer_list;
    
    while(temp != NULL) {
        if(temp->id == id) {
            return temp;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//Search airline, return airline with given id
struct airline* search_airline(int id){
    struct airline *temp;
    temp = airline_list;
    
    while(temp != NULL) {
        if(temp->id == id) {
            return temp;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//Search transaction, return transaction with given id
struct transaction* search_transaction(int id){
    struct transaction *temp = transaction_list;
    
    while(temp != NULL) {
        if(temp->id == id) {
            return temp;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//Search flight, return flight with given id, and airline id
struct flight* search_flight(int id, int airline_id){
    struct airline *airline = NULL;
    struct flight *temp = NULL;
    
    airline = search_airline(airline_id);
    temp = airline->flight;
    
    while(temp != NULL) {
        if(temp->id == id) {
            return temp;
        }
        else {
            temp = temp->next;
        }
    }
    return NULL;
}

//Set customers list
void set_customers(int id, int max_operation, int max_reserve){
    insert_list = (struct customer*) malloc(sizeof (struct customer));
    insert_list -> id = id;
    insert_list -> r = 0;
    insert_list -> max_operation = max_operation;
    insert_list -> max_reserve = max_reserve;
    insert_list -> counts_of_buy = 0;
    insert_list -> counts_of_reserve = 0;
    insert_list -> counts_of_cancel = 0;
    insert_list -> counts_of_nothing = 0;
    insert_list -> next = NULL;
    insert_list -> reservation = NULL;
    
    pthread_mutex_init(&(insert_list -> mutex), NULL);
    
    if (customer_list == NULL)
        customer_list = insert_list;
    else{
        temp = customer_list;
        
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = insert_list;
    }
}

//Set ticket officers list
void set_ticket_officer(int id){
    ticket_officer_insert_list = (struct ticket_officer*) malloc(sizeof (struct ticket_officer));
    ticket_officer_insert_list -> id = id;
    ticket_officer_insert_list -> next = NULL;
    ticket_officer_insert_list -> counts_of_buy = 0;
    ticket_officer_insert_list -> counts_of_reserve = 0;
    ticket_officer_insert_list -> counts_of_cancel = 0;
    ticket_officer_insert_list -> counts_of_nothing = 0;
    ticket_officer_insert_list -> request = NULL;
    
    pthread_mutex_init(&ticket_officer_insert_list -> mutex, NULL);
    pthread_mutex_init(&ticket_officer_insert_list -> customer, NULL);
    pthread_mutex_init(&ticket_officer_insert_list -> customer_request, NULL);
    
    if (ticket_officer_list == NULL)
        ticket_officer_list = ticket_officer_insert_list;
    else{
        ticket_officer_temp = ticket_officer_list;
        
        while (ticket_officer_temp -> next != NULL) {
            ticket_officer_temp = ticket_officer_temp -> next;
        }
        ticket_officer_temp -> next = ticket_officer_insert_list;
    }
}

//Set transactions list
int set_transactions(int trans_id, int customer_id, int officer_id, int operation, int simulation_day, int success, int airline_id, int flight_id, int seat_count){
    //Lock following piece of code according to transaction
    pthread_mutex_lock(&transaction);
    
    struct transaction *temp = NULL, *insert = NULL;
    
    insert = (struct transaction*) malloc(sizeof (struct transaction));
    insert -> id = t_id;
    insert -> customer_id = customer_id;
    insert -> officer_id = officer_id;
    insert -> operation = operation;
    insert -> simulation_day = simulation_day;
    insert -> success = success;
    insert -> airline_id = airline_id;
    insert -> flight_id = flight_id;
    insert -> seat_count = seat_count;
    insert -> next = NULL;
    
    if (transaction_list == NULL)
        transaction_list = insert;
    else{
        temp = transaction_list;
        
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        temp -> next = insert;
    }
    trans_id = t_id;
    t_id++;
    pthread_mutex_unlock(&transaction);
    return trans_id;
}

//Set request list
void set_request(int customer_id, int operation, int airline_id, int flight_id, int seat_count, int t_id, int ticket_officer_id){
    struct ticket_officer * tt = search_ticket_officer(ticket_officer_id);

    pthread_mutex_lock(&tt->customer_request);
    
    struct request *temp = NULL, *insert = NULL;
    
    insert = (struct request*) malloc(sizeof (struct request));
    insert -> customer_id = customer_id;
    insert -> operation = operation;
    insert -> airline_id = airline_id;
    insert -> flight_id = flight_id;
    insert -> seat_count = seat_count;
    insert -> transaction_id = t_id;
    insert -> next = NULL;
    
    if (tt -> request == NULL)
        tt -> request = insert;
    else{
        temp = tt -> request;
        
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        
        temp -> next = insert;
    }
    pthread_mutex_unlock(&tt->customer_request);
}

// Set airlines list
void set_airlines(int id){
    airline_insert_list = (struct airline*) malloc(sizeof (struct airline));
    airline_insert_list -> id = id;
    airline_insert_list -> flight = NULL;
    airline_insert_list -> next = NULL;
    
    if (airline_list == NULL)
        airline_list = airline_insert_list;
    else{
        airline_temp = airline_list;
        
        while (airline_temp -> next != NULL) {
            airline_temp = airline_temp -> next;
        }
        airline_temp -> next = airline_insert_list;
    }
}

// Set flights list
void set_flights(int id, int airline_id){
    struct airline *airline = NULL;
    struct flight *temp = NULL, *insert = NULL;
    
    airline = search_airline(airline_id);
    
    insert = (struct flight*) malloc(sizeof (struct flight));
    insert -> id = id;
    insert -> seat_count = 40;
    insert -> counts_of_solds = 0;
    insert -> counts_of_reserved = 0;
    insert -> counts_of_cancelled = 0;
    insert -> next = NULL;
    
    pthread_mutex_init(&insert -> mutex, NULL);

    if (airline -> flight == NULL)
        airline -> flight = insert;
    else{
        temp = airline -> flight;
        
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        
        temp -> next = insert;
    }
}

//Set reservations list
void set_reservations(int t_id, int customer_id){
    pthread_mutex_lock(&lock);
    
    struct customer *customer = NULL;
    struct reservation *temp = NULL, *insert = NULL;
    
    customer = search(customer_id);
    
    insert = (struct reservation*) malloc(sizeof (struct reservation));
    insert -> transaction_id = t_id;
    insert -> next = NULL;
    
    if (customer -> reservation == NULL)
        customer -> reservation = insert;
    else{
        temp = customer -> reservation;
        
        while (temp -> next != NULL) {
            temp = temp -> next;
        }
        
        temp -> next = insert;
    }
    pthread_mutex_unlock(&lock);
    
}
