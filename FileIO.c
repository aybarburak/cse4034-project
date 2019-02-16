//
//  FileIO.c
//
//  Created by Burak
//  Copyright © 2017 Burak. All rights reserved.
//

#include "FileIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

int number_of_customers = 0;

// Read first line for getting information about number of customers, number of ticket officers, simulation day, and number of airlines and flights
int * read_first_line(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * number;
    
    int first = 0;
    int num = 0;
    static int first_line[5];
    
    fp = fopen("/Users/burak/Documents/testt/testt/input.txt", "r");
    
    if (fp == NULL)
        exit(EXIT_FAILURE);
    
    while ((read = getline(&line, &len, fp)) != -1) {
        number = strtok(line, " ");
        while (number != NULL)
        {
            if(!first){
                first_line[num] = atoi(number);
            }
            number = strtok (NULL, " \n");
            num++;
        }
        num = 0;
        first++;
    }
    fclose(fp);
    return first_line;
}

//  Print all information which are required from project document
void print_log_file(){
        int i;
        struct airline *air = airline_list;
        
        printf("TRANSACTION LIST\n");
        
        FILE *f = fopen("/Users/burak/Documents/testt/testt/log_file.txt", "w");
        
        if (f == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
        
        i = 1;
        fprintf(f, "----------FIRST PART----------\n");
        
        while(transaction_list != NULL){
            
            fprintf(f, "<%d     %d     %d     %d>\n", transaction_list->customer_id, transaction_list->officer_id, transaction_list->operation, transaction_list->simulation_day);
            
            fprintf(stderr,"(%d-- %d %d %d o%d %d s%d %d %d %d)\n", i,transaction_list->id, transaction_list->officer_id, transaction_list->customer_id, transaction_list->operation, transaction_list->simulation_day, transaction_list->success, transaction_list->airline_id, transaction_list->flight_id, transaction_list->seat_count);
            
            transaction_list = transaction_list->next;i++;
        }
        fprintf(f, "\n----------SECOND PART----------\n");
        int total_request_buy = 0, total_request_reserve = 0, total_request_cancel = 0, total_request_nothing = 0;
        while(customer_list != NULL){
            fprintf(f, "Customer %d  Buy : %d\n", customer_list->id, customer_list->counts_of_buy );
            fprintf(f, "Customer %d  Reserve : %d\n", customer_list->id, customer_list->counts_of_reserve );
            fprintf(f, "Customer %d  Cancel: %d\n", customer_list->id, customer_list->counts_of_cancel );
            fprintf(f, "Customer %d  Nothing: %d\n", customer_list->id, customer_list->counts_of_nothing );
            total_request_buy += customer_list -> counts_of_buy;
            total_request_reserve += customer_list -> counts_of_reserve;
            total_request_cancel += customer_list -> counts_of_cancel;
            total_request_nothing += customer_list -> counts_of_nothing;
            
            customer_list = customer_list->next;
            fprintf(f, "\n");
        }
        fprintf(f, "Total Buy Request : %d - Reserve Request : %d - Cancel Request : %d - Nothing Request : %d\n\n", total_request_buy, total_request_reserve, total_request_cancel, total_request_nothing );
        
        int total_trans_buy = 0, total_trans_reserve = 0, total_trans_cancel = 0, total_trans_nothing = 0;
        while(ticket_officer_list != NULL){
            fprintf(f, "Officer %d  Buy : %d\n", ticket_officer_list->id, ticket_officer_list->counts_of_buy );
            fprintf(f, "Officer %d  Reserve : %d\n", ticket_officer_list->id, ticket_officer_list->counts_of_reserve );
            fprintf(f, "Officer %d  Cancel : %d\n", ticket_officer_list->id, ticket_officer_list->counts_of_cancel );
            fprintf(f, "Officer %d  Nothing : %d\n", ticket_officer_list->id, ticket_officer_list->counts_of_nothing );
            
            total_trans_buy += ticket_officer_list -> counts_of_buy;
            total_trans_reserve += ticket_officer_list -> counts_of_reserve;
            total_trans_cancel += ticket_officer_list -> counts_of_cancel;
            total_trans_nothing += ticket_officer_list -> counts_of_nothing;
            
            ticket_officer_list = ticket_officer_list->next;
            fprintf(f, "\n");
        }
        fprintf(f, "Total Buy Transaction : %d - Reserve Transaction : %d - Cancel Transaction : %d - Nothing Transaction : %d\n", total_trans_buy, total_trans_reserve, total_trans_cancel, total_trans_nothing );
        fprintf(f, "\n----------THIRD PART----------\n");
        
        
        while(air != NULL){
            struct flight *ff = air -> flight;
            
            while (ff != NULL){
                fprintf(f, "Airline %d Flight %d - %d %d %d\n", air->id, ff-> id,ff -> counts_of_solds, ff->counts_of_reserved, ff->counts_of_cancelled);
                ff = ff->next;
            }
            air = air->next;
        }
        
        
        fclose(f);
}

