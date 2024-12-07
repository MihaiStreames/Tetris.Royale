#pragma once

#include <vector>
#include "tetromino.hpp" 
#include "common.hpp"    

class GameMatrix {
private:
    Tetromino* currentTetromino;
    int width;                                 
    int height;                                
    std::vector<std::vector<int>> board;       // Matrice de jeu (0 = vide, 1 = occupé)

public:
    GameMatrix(int width, int height);

    void generateBoardbyDimension();
    bool isColliding(const Tetromino& tetromino) const;   
    bool trySpawnPiece(Tetromino& tetromino);              // Genere une nv piece
    bool tryPlacePiece(const Tetromino& tetromino);        

    bool canMove(const Tetromino& tetromino, Move move) const;
    bool canRotate(const Tetromino& tetromino, Rotation rotation) const; 
    bool tryMoveCurrent(const Tetromino& tetromino, Move move); 
    bool tryRotateCurrent(const Tetromino& tetromino);     
    bool tryMakeCurrentPieceFall();                        
    
    int getRowstoObstacle(const Tetromino& tetromino) const; 
    
    bool isLineFull(int line) const;                       // Vérifie si une ligne est complète
    void clearSingleLine(int line);                        
    int makeLineFull(int line);                            
    int clearFullLines();                                  

    const std::vector<std::vector<int>>& getBoard() const; // Retourne la matrice de jeu
};