#include "labolsaMPI.h"

OrderMPI createOrderMPI_buy(void *stock_reference,
			    void *market_reference,
			    void *user_reference,
			    int market_index_order_buy,
			    int market_norders_buy,
			    float stock_price,
			    float user_money){
  OrderMPI orderMPI;
  float risk;
  if (market_index_order_buy < market_norders_buy){

    orderMPI.stock_reference = stock_reference;
    orderMPI.market_reference = market_reference;
    orderMPI.user_reference = user_reference;
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
