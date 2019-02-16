#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "FileIO.h"
#include "List.h"

int transaction_id = 1;
int airline_count, flight_count;
int day = 1, current_day = 1, finish_day = 0;
int count;

void customer ( void *ptr );
void ticket ( void *ptr );

int main()
{
    // Read customers from input file
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * number;
    
    int first = 0;
    int num = 0;
    int i,j;

    int *first_line;
    first_line = read_first_line();

    // Create a customers array
    int customers[first_line[0]][3];
    int max_id = 0;
    
    first=0;
    fp = fopen("/Users/burak/Documents/testt/testt/input.txt", "r");
    while ((read = getline(&line, &len, fp)) != -1) {
        number = strtok(line, " ");
        while (number != NULL)
        {
            if(first){
                customers[first-1][num] = atoi(number);
                if(customers[first-1][0] > max_id)
                    max_id = customers[first-1][0];
            }
            number = strtok (NULL, " \n");
            num++;
        }
        num = 0;
        first++;
    }
    
    fclose(fp);
    
    // Set all max reservation numbers and max operations to the 3 except given ones
    for(i = first - 1; i < first_line[0]; i++){
        customers[i][0] = max_id + 1;
        customers[i][1] = 3;
        customers[i][2] = 3;
        max_id = customers[i][0];
    }
    count = first_line[1];
    
    // Add all customers to the customers list
    for(i = 0; i < first_line[0]; i++){
        set_customers(customers[i][0], customers[i][1], customers[i][2]);
    }
    
    // Add all ticket officers to the ticket officers list
    for(i = 0; i < first_line[1]; i++){
        set_ticket_officer(i);
    }
    
    
    airline_count = first_line[3];
    flight_count = first_line[4];
    
    // Add all airlines and flights to the their lists
    for(i = 0; i < first_line[3]; i++){
        set_airlines(i);
        for(j = 0; j < first_line[4]; j++){
            set_flights(j,i);
        }
    }
    
    // Create customers and ticket officers threads
    pthread_t customer_threads[first_line[0]];
    pthread_t ticket_officer_threads[first_line[1]];
    
    for (i = 0; i < first_line[0]; i++) {
        pthread_create (&customer_threads[i], NULL, (void *) &customer, (void *) customers[i][0]);
    }
    
    for (i = 0; i < first_line[1]; i++) {
        pthread_create (&ticket_officer_threads[i], NULL, (void *) &ticket, (void *) i);
    }

    
    //Initalize datas for new days
    while (day < first_line[2]){
        sleep(10);
        day++;
        int i=0;
        for(i=0;i < first_line[0] ;i++){
            struct customer * cc = search(customers[i][0]);
            cc->max_operation = customers[i][1];
            cc->max_reserve = customers[i][2];
            cc->reservation = NULL;
        }
        for(i = 0; i < first_line[3]; i++){
            for(j = 0; j < first_line[4]; j++){
                struct flight * ff = search_flight(j, i);
                ff->seat_count = 40;
            }
        }
        current_day++;
        fprintf(stderr, "-----------day end----------\n");

    }
    sleep(10);
    finish_day = 1;

    // Join customers and ticket officers
    for (i = 0; i < first_line[0]; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    for (i = 0; i < first_line[1]; i++) {
        pthread_join(ticket_officer_threads[i], NULL);
    }

    // Print log file according to the project document
    print_log_file();
    return 0;
}

void customer ( void *ptr ){
    
    int id = ((int *) ptr);
    
    struct customer * customer = search(id);
    while(1){
        
        // Until day and current day arenot equal, wait in this loop
        while (day != current_day){
            if (day == current_day)
                break;
        }
        if (finish_day)break;
        // If customer's max operation number is greater than zero, customer should enter the queue again
        while(customer->max_operation > 0){
            pthread_mutex_lock(&(customer->mutex));
        
                int i;
                // Customers try ticket officers, if one of them is busy s/he tries other ones
                for (i = 0; i < count; i++){
                    struct ticket_officer * tt = search_ticket_officer(i);
                    if (pthread_mutex_trylock(&(tt->customer)) == 0){
                        // They choose operation randomly, and according to this, they request to the their ticket officers
                        int operation = (rand() % 4) + 1;
            
                        if (operation == 1){
            
                            int airline_id,flight_id, seat_count;
            
                            airline_id = rand() % airline_count;
                            flight_id = rand() % flight_count;
                            seat_count = (rand() % 40) + 1;
                            set_request(id, 1, airline_id, flight_id, seat_count, -1, tt->id);
                            //fprintf(stderr,"-%d %d %d %d %d------%d\n", id, 1, airline_id, flight_id, seat_count, tt->id);
                            customer->counts_of_buy += 1;
                            customer->r = 1;
                            customer->max_operation--;
            
                        }
                        else if (operation == 2){
                            int airline_id,flight_id, seat_count;
                
                            airline_id = rand() % airline_count;
                            flight_id = rand() % flight_count;
                            if (customer -> max_reserve > 0){
                                seat_count = (rand() % customer -> max_reserve) + 1;
                                customer -> max_reserve -= seat_count;
                            }
                            else
                                seat_count = 0;
                            customer->r = 1;
                            customer->max_operation--;
                            set_request(id, operation, airline_id, flight_id, seat_count, -1, tt->id);
                            customer->counts_of_reserve += 1;

                            //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, airline_id, flight_id, seat_count);
                        }
                        else if (operation == 3){
                            int tt_id = -1;
                            customer->r = 1;
                            customer->max_operation--;
                            customer->counts_of_cancel += 1;
                            if (customer->reservation != NULL){
                                tt_id = customer->reservation->transaction_id;
                                customer->reservation = customer->reservation->next;
                            }
                
                            set_request(id, operation, -1, -1, -1, tt_id, tt->id);
                            //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, 0, 0, 0);
                        }
                        else if (operation == 4){
                            customer->r = 1;
                            customer->max_operation--;
                            customer->counts_of_nothing += 1;
                            set_request(id, operation, -1, -1, -1, -1, tt->id);
                            //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, 0, 0, 0);
                        }
                        pthread_mutex_unlock(&(tt->customer));
                        break;
                    }else{
                        customer->r = 0;
                    }
    
                }
            // If all ticket officers are full, customers wait ticket officer randomly
            if(customer->r == 0){
                int tt_id = rand() % count;
                struct ticket_officer * tt = search_ticket_officer(tt_id);

                pthread_mutex_lock(&(tt->customer));
                    int operation = (rand() % 4) + 1;
                    if (operation == 1){
                        int airline_id,flight_id, seat_count;
        
                        airline_id = rand() % airline_count;
                        flight_id = rand() % flight_count;
                        seat_count = (rand() % 40) + 1;
                        customer->r = 1;
                        customer->max_operation--;
                        customer->counts_of_buy += 1;

                        set_request(id, operation, airline_id, flight_id, seat_count, -1, tt->id);
                        //fprintf(stderr,"-%d %d %d %d %d------%d\n", id, 1, airline_id, flight_id, seat_count, tt->id);
                    }
                    else if (operation == 2){
                        int airline_id,flight_id, seat_count;
            
                        airline_id = rand() % airline_count;
                        flight_id = rand() % flight_count;
                        if (customer -> max_reserve > 0){
                            seat_count = (rand() % customer -> max_reserve) + 1;
                            customer -> max_reserve -= seat_count;
                        }
                        else
                            seat_count = 0;
                        customer->r = 1;
                        customer->max_operation--;
                        customer->counts_of_reserve += 1;

                        set_request(id, operation, airline_id, flight_id, seat_count, -1, tt->id);

                        //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, airline_id, flight_id, seat_count);
                    }
                    else if (operation == 3){
                        int tt_id = -1;
                        customer->r = 1;
                        customer->max_operation--;
                        customer->counts_of_cancel += 1;
                        if (customer->reservation != NULL){
                            tt_id = customer->reservation->transaction_id;
                            customer->reservation = customer->reservation->next;
                        }
            
                        set_request(id, operation, -1, -1, -1, tt_id, tt->id);
                        //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, 0, 0, 0);
                    }
                    else if (operation == 4){
                        customer->r = 1;
                        customer->max_operation--;
                        customer->counts_of_nothing += 1;

                        set_request(id, operation, -1, -1, -1, -1, tt->id);
                        //fprintf(stderr,"-%d %d %d %d %d-\n", id, operation, 0, 0, 0);
                    }
        
                pthread_mutex_unlock(&(tt->customer));
            }
        }
    }
}

// Ticket officers select customers according to the their list
void ticket( void *ptr ){
    while(1){
        int x;
        x = ((int *) ptr);
        struct ticket_officer * ticket_officer = search_ticket_officer(x);
        struct request *temp_list;
        // If their request list is null, officer need to wait, and also if day is not equal current day again officer needs to wait
        while(ticket_officer->request == NULL){
            while (day != current_day){
                if (day == current_day)
                    break;
            }
            if (finish_day)break;

            if (ticket_officer->request != NULL)
                break;
        }
        if (finish_day)break;
        // Officer selects the customer from his/her request list's
        pthread_mutex_lock(&(ticket_officer -> mutex));

            temp_list = ticket_officer->request;
            ticket_officer->request = ticket_officer->request->next;
    
            // According to customer operations, officers work on them
            if(temp_list->operation == 1){
                struct flight * flight = search_flight(temp_list ->flight_id, temp_list->airline_id);
                pthread_mutex_lock(&flight -> mutex);
        
                    int success = 1, seat_count = 0;;

                    if (flight->seat_count > 0 && (flight->seat_count - temp_list->seat_count) >= 0   ){
                        flight->seat_count -= temp_list->seat_count;
                        seat_count = temp_list->seat_count;
                    }else{
                        success = 0;
                    }
        
                    set_transactions(transaction_id, temp_list->customer_id, x, temp_list->operation, day, success, temp_list -> airline_id, temp_list -> flight_id, seat_count );
                    flight->counts_of_solds += seat_count;
                    ticket_officer -> counts_of_buy += 1;
                pthread_mutex_unlock(&flight -> mutex);
            }
            else if (temp_list-> operation == 2){
                struct flight * flight = search_flight(temp_list ->flight_id, temp_list->airline_id);

                pthread_mutex_lock(&flight -> mutex);
                    int success = 1, seat_count = 0;

                    if (flight->seat_count > 0 && (flight->seat_count - temp_list->seat_count) >= 0   ){
                        flight->seat_count -= temp_list->seat_count;
                        seat_count = temp_list->seat_count;
                    }else {
                        success = 0;
                    }
        
                    int tt_id = set_transactions(transaction_id, temp_list->customer_id, x, temp_list->operation, day, success, temp_list -> airline_id, temp_list -> flight_id, seat_count );
                    flight->counts_of_reserved += seat_count;
                    ticket_officer -> counts_of_reserve += 1;
                    set_reservations(tt_id, temp_list->customer_id);
                pthread_mutex_unlock(&flight -> mutex);
            }
            else if (temp_list-> operation == 3){
                int success = 1, seat_count = 0, airline_id = -1, flight_id = -1;
                ticket_officer -> counts_of_cancel += 1;
                if (temp_list->transaction_id != -1){
                    struct transaction * transaction = search_transaction(temp_list->transaction_id);
                    struct flight * flight = search_flight(transaction->flight_id, transaction->airline_id);
                    pthread_mutex_lock(&flight -> mutex);
                        flight->seat_count += transaction->seat_count;
                        flight->counts_of_cancelled += transaction->seat_count;
                        seat_count = transaction->seat_count;
                        airline_id = transaction->airline_id;
                        flight_id = transaction->flight_id;
                    pthread_mutex_unlock(&flight -> mutex);
                }else{
                    success = 0;
                    seat_count = 0;
                }
                set_transactions(transaction_id, temp_list->customer_id, x, temp_list->operation, day, success, airline_id, flight_id, seat_count );
            }
            else if (temp_list-> operation == 4){
                int success = 1;
                ticket_officer -> counts_of_nothing += 1;

                set_transactions(transaction_id, temp_list->customer_id, x, temp_list->operation, day, success, -1, -1, 0 );
            }
        

        pthread_mutex_unlock(&(ticket_officer -> mutex));
        struct customer * cust = search(temp_list->customer_id);
        pthread_mutex_unlock(&(cust->mutex));
    }
}
