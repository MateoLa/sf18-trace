/*
  MateoLa helpers to manually trace Stockfish
*/

#include <string>

#include "mateola.h"

namespace Stockfish {

std::string getPieceType(PieceType pt) {
    std::string s = std::to_string(static_cast<int>(pt));

    switch (pt) {
        case ALL_PIECES: return s + ": AllPieces/NoPieceType";
        case PAWN: return s + ": Pawn";
        case KNIGHT: return s + ": Knight";
        case BISHOP: return s + ": Bishop";
        case ROOK: return s + ": Rook";
        case QUEEN: return s + ": Queen";
        case KING: return s + ": King";
        case PIECE_TYPE_NB: return s + ": Piece_Type_Nb";
    }
}

std::string getSquare(Square sq) {
    std::string Squares[] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", 
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "sq_nb", "sq_none"
    };

    return Squares[sq];
}

void Imprimir_Array(uint8_t arr[], int size) {
	for (int i = 0; i < size; i++) {
	    std::cout << static_cast<int>(arr[i]) << " ";
	}
}

}  // namespace Stockfish
