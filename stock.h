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

#ifndef STOCK_H
#define STOCK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  typedef struct{
    char code[8];
    float price;
    float avg_price[256]; //vector to compute average price
    int index_avg_price;
    int nstocks; // stocks available for sell.
    float min;
    float max;
    float begin;
    float end;
    int begin_flag;
  }Stock;

  Stock newStock(char code[], float price, int nstocks);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* STOCK_H */
