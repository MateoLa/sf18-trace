/*
  MateoLa helpers to manually trace Stockfish
*/

#include <iostream>

#include "../types.h"


namespace Stockfish {

std::string getPieceType(PieceType pt);
std::string getSquare(Square sq);
void Imprimir_Array(uint8_t arr[], int size);
}