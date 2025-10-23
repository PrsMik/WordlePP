#pragma once

#include <algorithm>
#include <cmath>

struct LayoutMetrics
{
    // rows
    float startX;
    float startY;
    float boxSize;
    float boxSpacing;
    float rowHeight;
    int fontSizePT;

    // keyboard
    float keyboardStartY;
    float keyWidth;
    float keyHeight;
    float keySpacing;
    float rowSpacing;
    float keyboardCenterX;
    int keyboardFontSizePT;

    // overlay
    float overlayBoxX;
    float overlayBoxY;
    float overlayBoxWidth;
    float overlayBoxHeight;
    float overlayTitleAreaWidth;
    float overlayTitleAreaHeight;
    float overlayRestartAreaWidth;
    float overlayRestartAreaHeight;
    float overlayRestartYPosition;
};

class GameLayout
{
private:
    int windowWidth;
    int windowHeight;
    int wordLength;
    int maxGuesses;

    LayoutMetrics calculateMetrics() const;

public:
    GameLayout(int width, int height, int length, int attempts);

    LayoutMetrics getMetrics() const;
};