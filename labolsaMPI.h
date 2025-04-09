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

#ifndef LABOLSAMPI_H
#define LABOLSA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "market.h"
#include <mpi.h>
  
  typedef struct{    
    int i;
    int j;
    int index_order_buy;
    int norders_buy;
    float price;
    int n_actions; //number of actions in the transaction.
    int typeOrder; //0:sell 1:buy -1:not enough memory
    float ask; //sell
    float bid; //buy
    int stock_begin_flag;
    float money;
    float money_in_orders;
    int flag_start;
  }OrderMPI;

  MPI_Datatype create_OrderMPI_type();


  OrderMPI createOrderMPI_buy(int i,
			     int j,
			    int market_index_order_buy,
			    int market_norders_buy,
			    float stock_price,
			      float user_money);
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* LABOLSAMPI_H */
