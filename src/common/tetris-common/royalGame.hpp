#include "classicGame.hpp"

class RoyalGame : public ClassicGame
{
private:
    int timer;
    
public:
    RoyalGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0): 
    ClassicGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {}
    ~RoyalGame() {}

    void inverted_command(bool flag) override {
        reverseControls = flag;
    }

    void thunder_strike() override
    {
        // Choisir une colonne qui sera frappée par la foudre
        int col = rand() % gameMatrix.getWidth();

        // -1 = colonne ou se trouve aucune brique
        int impactRow = -1;

        tetroMat& board = gameMatrix.getBoard();

        for (int y = 0; y < gameMatrix.getHeight(); ++y)
        {
            if (board[y][col] == 1)
            { // Trouve la première brique
                impactRow = y;
                break;
            }
        }

        // Si aucun bloc touché, on ne fait rien
        if (impactRow == -1)
            return ; 

        // Détruire la zone 2x2 autour de l'impact
        for (int dy = 0; dy < 2; ++dy)
        {
            for (int dx = 0; dx < 2; ++dx)
            {
                int x = col + dx;
                int y = impactRow + dy;
                if (x < gameMatrix.getWidth() && y < gameMatrix.getHeight())
                {                    
                    board[y][x] = 0;
                }
            }
        }
    }

    void block_command() override {
        blockCommand = !blockCommand;
    }

    void fast_falling_pieces() override {
        speedFactor++;
    }

    void darkMode() override {
        setDarkModeTimer(getFrameCount() + 600);
        setDarkMode(true);
    }

    void blocs_1x1() override {
        for (int i = 0; i < 2; ++i){
            factory.pushPiece(Tetromino({0,0}, Single));
        }
    }

    void slow_falling_pieces() override {
        if (level > 0) {
            speedFactor--;
            if (level + speedFactor < 0) { // verifier si c est correct
                speedFactor = 0;
            }
        }
    }
};