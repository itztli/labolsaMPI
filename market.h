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

#ifndef MARKET_H
#define MARKET_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stock.h"
#include "user.h"
#include "common.h"

  //struct Order;
  
 
  Market *newMarket(char name[], int nstocks, int nusers, int norders);

  int addStock(Market *market, Stock stock);
  
  int addUser(Market *market, User user);

  int buy_OPI(Stock *stock, User *user, int nstocks, float value);
  
  void printMarket(Market *market);

  int closeMarket(Market *market);

  int remain_stocks(Market market);

  void print_divergence(Market *market);

  float get_value_of_stock(Market *market,char *code);
  
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* MARKET_H */
