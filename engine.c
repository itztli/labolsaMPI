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
#include "market.h"
#include "order.h"
#include "user.h"
#include <mpi.h>

float average(float arr[], int n) {
    float sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum / n;
}

//LOG
// BUG1: segmentation fault.
int montecarlo(Market *market){
  float price;
  int r;
  int n_actions,actual_stock;
  float ask,bid;
  int n_buy, n_sell,n_exe;
  //int miproc, numproc;

  //MPI_Init(&argn, &args); /* Inicializar MPI */
  //MPI_Comm_rank(MPI_COMM_WORLD,&miproc); /* Determinar el rango del proceso invocado*/
  //MPI_Comm_size(MPI_COMM_WORLD,&numproc); /* Determinar el numero de procesos */
  //MPI_Barrier (MPI_COMM_WORLD);

  //printf("miproc %i\n",miproc);

  //MPI_Finalize ();
  
  n_buy=0;
  n_sell=0;
  n_exe=0;
  //CREATING BUY/SELL ORDERS
  printf("#Creating NEW orders (old %i/%i):\n",market->index_order_buy,market->index_order_sell);  
  //printf("INFO: index_stock=%i\n",market->index_stock);
  for(int i=0; i < market->index_stock; i++){
    price = market->stocks[i].price;
    for(int j=0; j < market->index_user;j++){
      //if (price <= market->users[j].money){
      //the user have enough money to make a transaction.
      if (askOrderBuy(market->users[j], market->stocks[i])){
	createrOrder_buy(market, &market->stocks[i], &market->users[j]);
	n_buy++;
      }else 
      //printf("INFO: User=%i code=%s\n",j,market->stocks[i].code);
      if (askOrderSell(market->users[j], market->stocks[i])){
	//printf("INFO: User %i wants to sell!\n",j);
	createrOrder_sell(market, &market->stocks[i], &market->users[j]);
	n_sell++;
      }
      //}
    } //j
  } //i
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
      if (market->orders_buy[i].n_actions == 0){break;}
    }
    ///////////printf("\n");
  }
  printf("#Executions:%i\n",n_exe);
  //delete all the transactions for 0:
  orders_trash_collector(market);
  
  return 1;
}

int printJapaneseCandle(Market *market){
  //printf("Stocks:\n");
  printf("#CODE\tMIN\t\tBEGIN\t\tAVERAGE\t\tEND\t\tMAX\n");
  for(int i=0; i < market->index_stock; i++){
    printf("%s\t%f\t%f\t%f\t%f\t%f\n",market->stocks[i].code,market->stocks[i].min,market->stocks[i].begin,market->stocks[i].price,market->stocks[i].end,market->stocks[i].max);
  }

  return 1;
}

