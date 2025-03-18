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
#include <string.h>
#include "market.h"
#include "user.h"
#include "order.h"

Market *newMarket(char name[], int nstocks, int nusers, int norders){
  Market *market; 
  market = malloc(sizeof(Market));
  strcpy(market->name, name);
  market->nstocks = nstocks;
  market->nusers = nusers;
  market->norders_buy = norders;
  market->norders_sell = norders;
  market->stocks = malloc(sizeof(Stock)*nstocks);
  market->users = malloc(sizeof(User)*nusers);
  market->orders_buy = malloc(sizeof(Order)*norders);
  market->orders_sell = malloc(sizeof(Order)*norders);
  market->index_stock = 0;
  market->index_user = 0;
  market->index_order_buy = 0;
  market->index_order_sell = 0;
  return market;
}

int addStock(Market *market, Stock stock){
  market->stocks[market->index_stock] = stock;
  market->index_stock++;
  return 1;
}

int addUser(Market *market, User user){
  market->users[market->index_user] = user;
  market->index_user++;
  return 1;
}

// Returns the number of stocks bought by the user.
int buy_OPI(Stock *stock, User *user, int nstocks, float value){
  //printf("INFO1:nstocks=%i\tstock->nstocks=%i\n",nstocks,stock->nstocks);
  //printf("INFO2:%s\n",stock->code);
  int last_stock;
  if ((nstocks > stock->nstocks) && (stock->nstocks > 0)){
    nstocks = stock->nstocks;
  }
    //printf("INFO2:nstocks=%i\tstock->nstocks=%i\n",nstocks,stock->nstocks);
  if ((user->money >= nstocks*value) && (stock->nstocks > 0)){    
      user->money -= nstocks*value;
      stock->nstocks -= nstocks;
      //printf("INFO1:User:%i, Stock code:%s\n",user->index, stock->code);

      //BUG6: add instead update
      last_stock = get(*user,stock->code);
      if(last_stock < 0){
	last_stock = 0;
      }
      insert(user,stock->code,nstocks+last_stock);
      //printMap(*user);
      //printf("---------\n");
      return nstocks;
    }else{ return 0; }
    //}else{ return 0; }
  
}

void printMarket(Market *market){
  float vstock;
  printf("Stocks:\n");
  for(int i=0; i < market->index_stock; i++){
    printf("%s\t%f\t%i\n",market->stocks[i].code,market->stocks[i].price,market->stocks[i].nstocks);
  }
  printf("Users:\n");
  printf("INDEX\tCASH\t\tORDERS\t\tSTOCKS\t\tTOTAL\n");
  for(int i=0; i < market->index_user; i++){
    vstock = value_in_stocks(market,market->users[i]);
    printf("%i\t%f\t%f\t%f\t%f\n",market->users[i].index, market->users[i].money, market->users[i].money_in_orders,vstock,market->users[i].money+market->users[i].money_in_orders+vstock);
    printMap(market->users[i]);
  }
}

int closeMarket(Market *market){
  free(market->stocks);
  free(market->users);
  free(market->orders_buy);
  free(market->orders_sell);
  free(market);
  return 1;
}

int remain_stocks(Market market){
  for (int i=0; i < market.index_stock; i++){
    if(market.stocks[i].nstocks > 0)
      return 1;
  }
  return 0;
}

void print_divergence(Market *market){
  int stocks,n,m,p;
  printf("#Divergence\n");
  printf("#CODE\tUSER\tORDER\tTOTAL\n");
  m=0;
  for (int i=0; i < market->index_stock; i++){
    n=0;
    p=0;
    for (int j=0; j < market->index_user; j++){
      stocks = get(market->users[j], market->stocks[i].code);
      if (stocks > 0){
	n+=stocks;
      }
    }

    for (int j=0; j < market->index_order_sell; j++){
      if (strcmp( market->orders_sell[j].stock->code, market->stocks[i].code) == 0){
	p += market->orders_sell[j].n_actions;
      }
    }
    
    m += n+p;    
    printf("#%s\t%i\t%i\t%i\n",market->stocks[i].code,n,p,n+p);
  }
  printf("#TOTAL=%i\n",m);
}


float get_value_of_stock(Market *market,char *code){
  for(int i=0; i< market->index_stock;i++){
    if(strcmp(market->stocks[i].code,code)==0){
      return market->stocks[i].price;
    }
  }
  printf("Warning: Stock value not found %s.\n",code);
  return 0.0;
}
