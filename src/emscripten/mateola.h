/*
  MateoLa helpers to manually trace Stockfish
*/

#include <cstdint>
#include <iostream>
#include <string>

#include "../types.h"


namespace Stockfish {
    void printAllSquares();
    std::string getPieceType(PieceType pt);
    std::string getSquare(Square sq);
    void Imprimir_Array(uint8_t arr[], int size);
    void emaciateAttacks(PieceType pt, Square sq, Bitboard b);
}