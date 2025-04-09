#include "labolsaMPI.h"
#include <stdio.h>
#include "order.h"
#include <mpi.h>

MPI_Datatype create_OrderMPI_type() {
    OrderMPI dummy;
    MPI_Datatype order_mpi_type;
    
    // Definir los bloques y tipos
    int blocklengths[13] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[13] = {
        MPI_INT,     // i
        MPI_INT,     // j
        MPI_INT,     // index_order_buy
        MPI_INT,     // norders_buy
        MPI_FLOAT,   // price
        MPI_INT,     // n_actions
        MPI_INT,     // typeOrder
        MPI_FLOAT,   // ask
        MPI_FLOAT,   // bid
        MPI_INT,     // stock_begin_flag
        MPI_FLOAT,   // money
        MPI_FLOAT,   // money_in_orders
        MPI_INT      // flag_start
    };
    
    // Calcular desplazamientos
    MPI_Aint displacements[13];
    MPI_Aint base_address;
    
    MPI_Get_address(&dummy, &base_address);
    
    MPI_Get_address(&dummy.i, &displacements[0]);
    MPI_Get_address(&dummy.j, &displacements[1]);
    MPI_Get_address(&dummy.index_order_buy, &displacements[2]);
    MPI_Get_address(&dummy.norders_buy, &displacements[3]);
    MPI_Get_address(&dummy.price, &displacements[4]);
    MPI_Get_address(&dummy.n_actions, &displacements[5]);
    MPI_Get_address(&dummy.typeOrder, &displacements[6]);
    MPI_Get_address(&dummy.ask, &displacements[7]);
    MPI_Get_address(&dummy.bid, &displacements[8]);
    MPI_Get_address(&dummy.stock_begin_flag, &displacements[9]);
    MPI_Get_address(&dummy.money, &displacements[10]);
    MPI_Get_address(&dummy.money_in_orders, &displacements[11]);
    MPI_Get_address(&dummy.flag_start, &displacements[12]);
    
    // Hacer los desplazamientos relativos
    for (int i = 0; i < 13; i++) {
        displacements[i] -= base_address;
    }
    
    // Crear el tipo estructurado
    MPI_Type_create_struct(13, blocklengths, displacements, types, &order_mpi_type);
    MPI_Type_commit(&order_mpi_type);
    
    return order_mpi_type;
}

OrderMPI createOrderMPI_buy(int i,
			    int j,
			    int market_index_order_buy,
			    int market_norders_buy,
			    float stock_price,
			    float user_money){
  OrderMPI orderMPI;
  float risk;
  if (market_index_order_buy < market_norders_buy){
    orderMPI.i = i;
    orderMPI.j = j;
  //necesitamos una funcion que de valores entre 0.02 y - 0.05)
  risk = randomFloatInRange(-0.05, 0.02);
  ////////order.stock = stock;
  ////////order.user = user;
  orderMPI.typeOrder = 1;
  orderMPI.bid = stock_price*(1.0+risk);
  orderMPI.n_actions = (int)((user_money/orderMPI.bid)*randomFloatInRange(0.0, 1.0));
  orderMPI.stock_begin_flag = 1;
  
  if (orderMPI.n_actions < 1){
    orderMPI.n_actions = 1;
  }
  //user->money -= ((float)order.n_actions)*order.bid;
  orderMPI.money =  ((float)orderMPI.n_actions)*orderMPI.bid;

  //user->money_in_orders += ((float)order.n_actions)*order.bid;
  orderMPI.money_in_orders = ((float)orderMPI.n_actions)*orderMPI.bid;

  //market->orders_buy[market->index_order_buy] = order;

  //market->index_order_buy++;
  }else{
    printf("Warning: Not enough memory to create order.\n");
    orderMPI.typeOrder = -1;
  }
  return orderMPI;    
}
