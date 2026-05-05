/*
  MateoLa helpers to manually trace Stockfish
*/

#include "mateola.h"
#include "../bitboard.h"

namespace Stockfish {

/*
std::string getPieceType(PieceType pt) {
    switch (pt) {
        case ALL_PIECES: return "AllPieces/NoPieceType";
        case PAWN: return "Pawn";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case ROOK: return "Rook";
        case QUEEN: return "Queen";
        case KING: return "King";
        case PIECE_TYPE_NB: return "Piece_Type_Nb";
    }
}
*/

void printAllSquares() {
    std::cout << "MaLa debugging: ALL SQUARES" << std::endl;
    std::string str = "";
    for (Square s = SQ_A1; s <= SQ_H8; ++s) { str += getCuadrado(s) + " "; };
    std::cout << str << std::endl;
}


std::string getPieceType(PieceType pt) {
    std::string Piezas[] = {"All/NoPT", "Pawn", "Knight", "Bishop", "Rook", "Queen", "King", " Piece_Type_Nb"};
    return Piezas[pt];
}

std::string getCuadrado(Square sq) {
    std::string Cuadrados[] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", 
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "sq_nb" };
    return Cuadrados[sq];
}

void Imprimir_Array(uint8_t arr[], int size) {
	for (int i = 0; i < size; i++) {
	    std::cout << static_cast<int>(arr[i]) << " ";
	}
}

}  // namespace Stockfish
