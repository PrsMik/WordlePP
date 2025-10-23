#include "GameLayout.hpp"

GameLayout::GameLayout(int width, int height, int length, int attempts)
    : windowWidth(width), windowHeight(height), wordLength(length), maxGuesses(attempts) {}

LayoutMetrics GameLayout::getMetrics() const
{
    return calculateMetrics();
}

LayoutMetrics GameLayout::calculateMetrics() const
{
    LayoutMetrics m{};

    // ====================================================================
    // --- 1. CALCULATE MAIN LAYOUT (WORD ROWS AND KEYBOARD) ---
    // ====================================================================

    // calcucalte grid width and keyboard
    const float GRID_WIDTH_RATIO = 0.80f;
    const float SPACING_RATIO = 0.1f;
    const float AVAILABLE_WIDTH_FOR_GRID = windowWidth * GRID_WIDTH_RATIO;

    m.boxSize = AVAILABLE_WIDTH_FOR_GRID / (wordLength + (wordLength - 1) * SPACING_RATIO);
    m.boxSpacing = m.boxSize * SPACING_RATIO;

    float actualGridWidth = wordLength * m.boxSize + (wordLength - 1) * m.boxSpacing;

    // height calculate
    const float TOTAL_HEIGHT_RATIO = 0.90f;
    const float AVAILABLE_HEIGHT_FOR_LAYOUT = windowHeight * TOTAL_HEIGHT_RATIO;

    const float GRID_TO_TOTAL_HEIGHT_RATIO = 0.65f;
    const float KEYBOARD_TO_TOTAL_HEIGHT_RATIO = 1.0f - GRID_TO_TOTAL_HEIGHT_RATIO;

    // grid height calculate
    float availableHeightForGrid = AVAILABLE_HEIGHT_FOR_LAYOUT * GRID_TO_TOTAL_HEIGHT_RATIO;
    const float ROW_SPACING_RATIO = 0.1f;
    const int MAX_GUESSES_ROWS = maxGuesses;

    m.rowHeight = availableHeightForGrid / (MAX_GUESSES_ROWS + (MAX_GUESSES_ROWS - 1) * ROW_SPACING_RATIO);
    m.rowSpacing = m.rowHeight * ROW_SPACING_RATIO;

    float actualGridHeight = (MAX_GUESSES_ROWS * m.rowHeight) + ((MAX_GUESSES_ROWS - 1) * m.rowSpacing);

    // keyboard height calculate
    float availableHeightForKeyboard = AVAILABLE_HEIGHT_FOR_LAYOUT * KEYBOARD_TO_TOTAL_HEIGHT_RATIO;
    const int KEYBOARD_NUM_ROWS = 3;

    m.keyHeight = (availableHeightForKeyboard - (KEYBOARD_NUM_ROWS - 1) * m.rowSpacing) / KEYBOARD_NUM_ROWS;
    float actualKeyboardHeight = (KEYBOARD_NUM_ROWS * m.keyHeight) + ((KEYBOARD_NUM_ROWS - 1) * m.rowSpacing);

    // calculate horizontal position
    m.startX = (windowWidth - actualGridWidth) / 2.0F;

    // calculate vertical position
    float totalLayoutHeight = actualGridHeight + m.rowSpacing + actualKeyboardHeight;
    m.startY = (windowHeight - totalLayoutHeight) / 2.0F;

    m.keyboardStartY = m.startY + actualGridHeight + m.rowSpacing;
    m.keyboardCenterX = (float)windowWidth / 2.0f;

    // calculate width of keys in keyboard
    const float REQUIRED_WIDTH_FOR_KEYBOARD = actualGridWidth;
    const int MAX_KEYS_IN_ROW = 11;
    const float SPACING_RATIO_KEY = 0.1f;

    m.keyWidth = REQUIRED_WIDTH_FOR_KEYBOARD / (MAX_KEYS_IN_ROW + (MAX_KEYS_IN_ROW - 1) * SPACING_RATIO_KEY);
    m.keySpacing = m.keyWidth * SPACING_RATIO_KEY;

    // font size for grid and keyboard
    m.fontSizePT = (int)std::round(m.rowHeight * 0.7f);
    m.keyboardFontSizePT = (int)std::round(m.keyHeight * 0.4f);

    // ====================================================================
    // --- 2. CALCULATE END GAME MESSAGE ---
    // ====================================================================

    // calculate message box metrics
    const float BOX_WIDTH = windowWidth * 0.8f;
    const float BOX_HEIGHT = windowHeight * 0.4f;

    m.overlayBoxWidth = BOX_WIDTH;
    m.overlayBoxHeight = BOX_HEIGHT;
    m.overlayBoxX = (windowWidth - BOX_WIDTH) / 2.0f;
    m.overlayBoxY = (windowHeight - BOX_HEIGHT) / 2.0f;

    // calculate end game message area
    m.overlayTitleAreaWidth = BOX_WIDTH * 0.9f;
    m.overlayTitleAreaHeight = BOX_HEIGHT * 0.45f;

    // calculate restart instructions area
    m.overlayRestartAreaWidth = BOX_WIDTH * 0.9f;
    m.overlayRestartAreaHeight = BOX_HEIGHT * 0.2f;
    const float RESTART_Y_POSITION_RATIO = 0.7f;

    m.overlayRestartYPosition = m.overlayBoxY + BOX_HEIGHT * RESTART_Y_POSITION_RATIO;

    return m;
}