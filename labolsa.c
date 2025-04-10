/**
    <labolsa> Montecarlo model of market stocks
    Copyright (C) 2024  Victor De la Luz 
                        <vdelaluz@enesmorelia.unam.mx>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

VERSION Beta (10/22/2024)
**/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "help.h"
#include "user.h"
#include "stock.h"
#include "market.h"
#include "engine.h"
#include "order.h"
#include "common.h"
#include "labolsaMPI.h"

// NOTE: The user not update the price of the order after the first execution. We need to create a new function to ask to the user if wants to update price after each execution ends.

int main(int argn, char **argv){
  User *user;
  Stock *stock;
  Market *market;
  char code[8];
  int i,j,k,n,N,M,P;
  float memory_used;
  int numproc, miproc;
  MPI_Status status;
  double utime0, stime0, wtime0, utime1, stime1, wtime1;
  
  //MPI montecarlo
  float price;
  int r;
  int n_actions,actual_stock;
  float ask,bid;
  int n_buy, n_sell,n_exe;
  Order order;
  OrderMPI orderMPI;

  MPI_Datatype MPI_ORDER;

  
  MPI_Init(&argn, &argv); /* Inicializar MPI */
  MPI_Comm_rank(MPI_COMM_WORLD,&miproc); /* Determinar el rango del proceso invocado*/
  MPI_Comm_size(MPI_COMM_WORLD,&numproc); /* Determinar el numero de procesos */
  MPI_Barrier (MPI_COMM_WORLD);

  MPI_ORDER = create_OrderMPI_type();

  
	if (miproc == 0){ //master 
    	//printf("%i\n",argn);
    	if (argn == 5){
		sscanf(argv[2],"%i", &M);
		sscanf(argv[3],"%i", &N);
		sscanf(argv[4],"%i", &P);
		market = newMarket(argv[1],M,N,P);
		
		//user = malloc(sizeof(User)*N);
		//stock = malloc(sizeof(Stock)*M);
		printf("%i\n",M);    
		printf("#LabolsaMPI simulator ver 20250409_1125\n");
		printf("# GNU/GPL License\n");
		printf("# By: Victor De la Luz <vdelaluz@enesmorelia.unam.mx>\n");
		}else{
		print_help();
		exit(0);
		}

			// Creating stocks
			printf("#Generating %i stock... ",M);
			for(i=0; i < M; i++){
				sprintf(code,"MEX%i",i);
				addStock(market,newStock(code,10.0,1000));      
				//stock[i] = newStock(code,100.0);
			}
			printf("#Ready!\n");
		
			printf("#Generating %i users... ",N);
			for(i=0; i < N; i++){
				addUser(market,newUser(i,2000.00));
			}
			printf("#Ready!\n");
			//printf("%s:%f\n",stock[0].code,stock[0].price);
			memory_used = (float)(sizeof(User)*N+sizeof(Stock)*M)/1e6; 
			printf("#Memory used: %f Mb \n",memory_used);
			//print_divergence(market);
			// create the OPIs of our model. We create a random asignator of OPIS for all the users.
			srand(time(NULL));
			k=0;
			printf("#Computing IOPs...\n");
			do{
				for(int i=0; i < market->index_user;i++){
					j = (int)randomValue(0.0, (float)market->index_stock);
					n = (int)((market->users[i].money/market->stocks[j].price)*randomValue(0.0, 1.0));
					//printf("INFO: n= %i\n",n);
					if (n < 1) n = 1;
					//printf("INFO1:%s\n",market->stocks[j].code);
					buy_OPI(&market->stocks[j],&market->users[i],n,market->stocks[j].price);
				}
				k++;
		
				//printMarket(market);
		
			}while(remain_stocks(*market) > 0);
		
			printf("#IOPs iterations: %i\n",k);
			//print_divergence(market);
			//printMarket(market);


	}
    
    MPI_Barrier (MPI_COMM_WORLD);
    
		if (miproc != 0) { // slaves
			//int flag_start = 1;
			//double F = 0.0; //result
			//data = 1;
			//range.F = 0.0;
			int flag_start = 1;
			//MPI_Send(&mp, sizeof(mp), MPI_CHARACTER, n_proc, 98, MPI_COMM_WORLD);
			//printf("Preparing slave %i\n",miproc);

			//BUG?
			////orderMPI.flag_start=flag_start; //first time flag_star==1		
		
			while(1){
			  
				//MPI_Send(&data, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD); 
				//MPI_Recv(&data, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
			  //printf("%i: Sending result...\n",miproc);
				MPI_Send(&orderMPI, 1, MPI_ORDER, 0, 0, MPI_COMM_WORLD);
				//printf("%i: OK...\n",miproc);
				//printf("%i: Receiving new dataset...\n",miproc);
				MPI_Recv(&orderMPI, 1, MPI_ORDER, 0, 0, MPI_COMM_WORLD, &status);
				//printf("%i: OK_R...\n",miproc);
				//integral de Riemman                                   
				//printf("%i\t%llu\t%i\n",miproc,data, f(data));
				//printf("%i: Computing order...\n",miproc);
				if (orderMPI.flag_start){
				  orderMPI = createOrderMPI_buy(orderMPI.i,orderMPI.j, orderMPI.index_order_buy, orderMPI.norders_buy,orderMPI.price,orderMPI.money);
				}
				//orderMPI = createOrderMPI_buy(i,j, market->index_order_buy, market->norders_buy,market->stocks[i].price,market->users[j].money );
				//printf("%i: OK_C...\n",miproc);
		
				//integral de Riemman
				//printf("%i\t%llu\t%i\n",miproc,data, f(data));
				//range.F = F;
				//printf("%i:[%lf,%lf] dx=%lf F=%lf\n",miproc,range.a,range.b,range.dx,range.F);
				//Parallel processing
			}
		
		}else{  //Master
		       int flag = -1;
		       MPI_Request request;
		       double sum = 0;
		       int n=0;
		       int i,j;
		       int no_deal;
		       
		       printf("%i: Running Montecarlo...\n",miproc);
			for(int i_monte=0; i_monte < 1000; i_monte++){
			  printf("%i:%i iteration\n",miproc,i_monte);
				//montecarlo(market);
		
				//***************************** BEGIN ************************************
				n_buy=0;
				n_sell=0;
				n_exe=0;
				//CREATING BUY/SELL ORDERS
				printf("#Creating NEW orders (old %i/%i):\n",market->index_order_buy,market->index_order_sell);  
				//printf("INFO: index_stock=%i\n",market->index_stock);		
				////      for(int i=0; i < market->index_stock; i++){
				////	price = market->stocks[i].price;
				////
				//Master                                                                                                                       
				i = 0;
				j = 0;
				no_deal=0;
				srand(1);
		
				price = market->stocks[0].price;
				orderMPI.flag_start = 0;		
				while (1) {
					if(flag != 0){
					  //printf("0: receiving...\n");
						MPI_Irecv(&orderMPI, 1, MPI_ORDER, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
						//printf("0: ok...\n");
						flag = 0;
						n++;
					}
					MPI_Test(&request, &flag, &status);
					if (flag != 0) {
						if (status.MPI_SOURCE != -1){
					  ////if (orderMPI.flag_start == 1){
					  ////		orderMPI.flag_start = 0;
					  ////	}else{
						  //printf("0: Adding new order buy to market...\n");
							//output processing
							//Fixing memory bug
							//order.stock = orderMPI.stock_reference; // &market->stocks[i];
							//order.user = orderMPI.market_reference; //&market->users[j];
						  if(orderMPI.flag_start){
							order.stock = &market->stocks[orderMPI.i]; // &market->stocks[i];
							order.user = &market->users[orderMPI.j]; //&market->users[j];
			
							//WORKS!!!
							//order.stock =  &market->stocks[i];
							//order.user = &market->users[j];

			
							order.typeOrder = orderMPI.typeOrder; //1
							order.bid = orderMPI.bid;
							order.n_actions = orderMPI.n_actions;
							order.stock->begin_flag = orderMPI.stock_begin_flag;
			
							market->users[orderMPI.j].money -= orderMPI.money;
							market->users[orderMPI.j].money_in_orders += orderMPI.money_in_orders;
			
							market->orders_buy[market->index_order_buy] = order;
							market->index_order_buy++;
			
							n_buy++;
							//printf("0: Adding OK...\n");
						  }
							// printf("%i\t%i\t%i\t%i\t%lf\t%lf\n",sum ,mp.miproc,mp.x,mp.y,mp.nu/1e9,C_light*C_light*mp.I_nu/(2.0*K*mp.nu*mp.nu));			
							//}
			

							// sending information                                                                                                        
							// segmentar la informacion para enviarla al nodo disponible                                                                  
			
							//printf("%i: %lf\n",status.MPI_SOURCE,range.F);                                                                              
							//data = rand64();
							//orderMPI = createOrderMPI_buy(i,j, market->index_order_buy, market->norders_buy,market->stocks[i].price,market->users[j].money );

						  //printf("0: Asking for buy...\n");
							if (askOrderBuy(market->users[j], market->stocks[i])){
								orderMPI.i = i;
								orderMPI.j = j;
								orderMPI.index_order_buy = market->index_order_buy;
								orderMPI.norders_buy = market->norders_buy;
								orderMPI.price = market->stocks[i].price; 
								orderMPI.money = market->users[j].money;
								orderMPI.flag_start = 1;
								//printf("0: User wants buy, creating order...\n");
								MPI_Send(&orderMPI, 1, MPI_ORDER, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
								//printf("0: Create new order sent to slave...\n");
							}else{
							  //printf("0: No deal\n");
								no_deal++;
							       	orderMPI.flag_start = 0;
								MPI_Send(&orderMPI, 1, MPI_ORDER, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
							}
			
							j++;
							
							if (j == market->index_user){
								j = 0;
								i++;
								//OJO Chekar!!!
								price = market->stocks[i].price;
							}
			
							if (i == market->index_stock){
								break;
							}	    
						}

						flag = -1;
					}
		
					//stop condition                                                                                                                  
					//if ((a+n*dx) >= b){                                                                                                             
					if (n == ((market->index_stock*market->index_user) -no_deal)){
					  //printf("Ready!\n");
						break;
					}
				
			 	} //while(1)   
		
				//createrOrder_buy(market, &market->stocks[i], &market->users[j]);
				//orderMPI = createOrderMPI_buy(&market->stocks[i], market, &market->users[j], market->index_order_buy, market->norders_buy,market->stocks[i].price,market->users[j].money );
				// i == stocks
				// j == users
				//data_send.i = i;
				//data_send.j = j;
				//data_send.index_order_buy = market->index_order_buy;
				//data_send.norders_buy = market->norders_buy;
				//data_send.price =  
				//MPI_Send(&data_send, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
				//MPI_Recv(&orderMPI, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);
				//integral de Riemman                                   
				//printf("%i\t%llu\t%i\n",miproc,data, f(data));
				//orderMPI = createOrderMPI_buy(i,j, market->index_order_buy, market->norders_buy,market->stocks[i].price,market->users[j].money );
				////	    //Fixing memory bug
				////	    //order.stock = orderMPI.stock_reference; // &market->stocks[i];
				////	    //order.user = orderMPI.market_reference; //&market->users[j];
				////
				////	    order.stock = &market->stocks[orderMPI.i]; // &market->stocks[i];
				////	    order.user = &market->users[orderMPI.j]; //&market->users[j];
				////
				////	    //WORKS!!!
				////	    //order.stock =  &market->stocks[i];
				////	    //order.user = &market->users[j];
				////
				////
				////	    order.typeOrder = orderMPI.typeOrder; //1
				////	    order.bid = orderMPI.bid;
				////	    order.n_actions = orderMPI.n_actions;
				////	    order.stock->begin_flag = orderMPI.stock_begin_flag;
				////
				////	    market->users[orderMPI.j].money -= orderMPI.money;
				////	    market->users[orderMPI.j].money_in_orders += orderMPI.money_in_orders;
				////
				////	    market->orders_buy[market->index_order_buy] = order;
				////	    market->index_order_buy++;
				////	    
				////	    n_buy++;
				//}
				////	} //j
				////  } //i
				
				for(int i=0; i < market->index_stock; i++){
					price = market->stocks[i].price;
					for(int j=0; j < market->index_user;j++){				
						if (askOrderSell(market->users[j], market->stocks[i])){
							//printf("INFO: User=%i code=%s\n",j,market->stocks[i].code);      
							//printf("INFO: User %i wants to sell!\n",j);
							createrOrder_sell(market, &market->stocks[i], &market->users[j]);
							n_sell++;
						}
					}
				}


				printf("#Order created!");
				//  printOrders(market);
				printf("#Buy Orders:%i\tSell Orders:%i.\n",n_buy,n_sell);
		
				// CLEAR BEGIN_FLAGS
		
				// EXECUTINGS ORDERS
				printf("#Executing orders...\n");
				for (int i=0; i < market->index_order_buy; i++){
					bid = market->orders_buy[i].bid;	
					////////printf("Looking for seller [%i/%s/%i/%f]...",market->orders_buy[i].user->index ,market->orders_buy[i].stock->code,market->orders_buy[i].n_actions,bid);
					for (int j=0; j < market->index_order_sell; j++){
						ask = market->orders_sell[j].ask;
						//printf("[%f],",ask);
						//BUG5: check if stock > 0
						//if (bid >= ask){
						//BUG6: check that code from buyer and seller are equals!
						//if ( (bid >= ask) && (market->orders_sell[j].n_actions > 0) ){
						if ( (bid >= ask) && (market->orders_sell[j].n_actions > 0) && (strcmp(market->orders_sell[j].stock->code,market->orders_buy[i].stock->code) == 0)){
							n_exe++;
							n_actions = market->orders_buy[i].n_actions;			
							if (market->orders_buy[i].n_actions > market->orders_sell[j].n_actions){
								n_actions = market->orders_sell[j].n_actions;
							}
							////////printf("<%i/%s/%i|%i:%f>\n",market->orders_sell[j].user->index,market->orders_sell[j].stock->code, market->orders_sell[j].n_actions,n_actions,bid);
							//update the n_actions in the order
							market->orders_buy[i].n_actions -= n_actions;
							market->orders_sell[j].n_actions -= n_actions;
							//printf("INFO: buy=%i\tsell=%i\n",market->orders_buy[i].n_actions,market->orders_sell[j].n_actions);
							//Update the money in order from the user i
							market->orders_buy[i].user->money_in_orders -= n_actions*bid;
							//transfering money to seller
							market->orders_sell[j].user->money += n_actions*bid;
							//quit the stock from the seller
							//printf("A:%s\n",market->orders_buy[i].stock->code);
							//BUG1 actual_stock = get(*market->orders_sell[j].user, market->orders_buy[i].stock[i].code);
							//BUG2 ask to the buyer not the seller!
							//actual_stock = get(*market->orders_sell[j].user, market->orders_buy[i].stock->code);
							actual_stock = get(*market->orders_buy[i].user, market->orders_buy[i].stock->code);
							//BUG5 new stock changes get from -1 to 0
							if(actual_stock <0){
								actual_stock=0; //new stock!!
							//	  printf("Warning: actual_stock < 0. [%i/%s/%i]\n",market->orders_buy[i].user->index,market->orders_buy[i].stock->code, actual_stock);
							}
							//printf("B\n");
							//BUG1 insert(market->orders_sell[j].user,market->orders_buy[i].stock[i].code,actual_stock-n_actions);
							//BUG2 the stocks are frozen in the order sell!!!
							// we need fix in the garbage collector return to the original seller the stocks
							// that not rise the price and stay in the order of sell!
							//insert(market->orders_sell[j].user,market->orders_buy[i].stock->code,actual_stock-n_actions);
						
							//BUG3 printf mistake!!
							//if ((actual_stock-n_actions)< 0 ){
							//  printf("ERROR [%i/%s/%i]\n",market->orders_sell[j].user->index,market->orders_sell[j].stock->code,actual_stock-n_actions);
							//}
						
							//BUG3: the stock are frozen in create sell order!
							//insert(market->orders_sell[j].user,market->orders_buy[i].stock->code,actual_stock-n_actions);
						
							// transfer the stock to the buyer
							//BUG1 actual_stock = get(*market->orders_buy[i].user, market->orders_buy[i].stock[i].code);
							//BUG3 get duplicated!
							//actual_stock = get(*market->orders_buy[i].user, market->orders_buy[i].stock->code);
							//BUG1 insert(market->orders_buy[i].user,market->orders_buy[i].stock[i].code,actual_stock+n_actions);
							insert(market->orders_buy[i].user,market->orders_buy[i].stock->code,actual_stock+n_actions);
						
							// update the price of the stock
							if (market->orders_sell[j].stock->begin_flag == 1){
								market->orders_sell[j].stock->begin = bid;
								market->orders_buy[i].stock->min = bid;
								market->orders_buy[i].stock->max = bid;
								market->orders_sell[j].stock->begin_flag = 0;
							}
							//BUG7 fixing average computations
							//market->orders_buy[i].stock->price = (market->orders_buy[i].stock->price + bid)/2.0;
							market->orders_buy[i].stock->avg_price[market->orders_buy[i].stock->index_avg_price] = bid;
							market->orders_buy[i].stock->index_avg_price++;
							market->orders_buy[i].stock->price = average(market->orders_buy[i].stock->avg_price,market->orders_buy[i].stock->index_avg_price);
			
							if (bid < market->orders_buy[i].stock->min){
								market->orders_buy[i].stock->min = bid;
							}
			
							if (bid > market->orders_buy[i].stock->max){
								market->orders_buy[i].stock->max = bid;
							}
			
							market->orders_buy[i].stock->end = bid;
			
						}//else{ // if bid >= ask
		
						//	printf("[Not found!]\n");
		
						//     }
						if (market->orders_buy[i].n_actions == 0){
							break;
						}
					} // for j=0; j< market->index_order_sell; j++
					///////////printf("\n");
				} //for (int i=0; i < market->index_order_buy; i++)
		
				printf("#Executions:%i\n",n_exe);
				//delete all the transactions for 0:
				orders_trash_collector(market);
		
				//******************************* END ************************************      

				printJapaneseCandle(market);

				//printOrders(market);
				//printMarket(market);
				//print_divergence(market);
			}
		
			//printMarket(market);
			print_divergence(market);
			//free(user);
			//free(stock);
			closeMarket(market);

		} //Master
	
		//Master END!

		MPI_Abort(MPI_COMM_WORLD,MPI_SUCCESS);

		//MPI_Finalize (); 
		return 0;
	}
