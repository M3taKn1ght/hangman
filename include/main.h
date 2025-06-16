#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
//Needed enum stats for statusmachine
enum eState
{
    stateNewGameScreen = 0,
    statNewGame,
    statInitGame,
    statWaitLetter,
    statCheckLetter,
    statWin,
    statGameOver
};

//Needed to calculate the position for displayed LCD-text
enum eTextPos
{
    posTextLeft = 0,
    posTextMiddle,
    posTextRight,
    posTextOwnOffset
};

/********* Functions *********/
/**
 * Setup function every arduino code needs
 * 
 * @return void
 */
void setup();

/**
 * Loop function every arduino code needs
 * 
 * @return void
 */
void loop();

/**
 * Function to draw hangman, if guessed letter was wrong
 * 
 * @return void
 */
void drawHangman();

/**
 * Draw Game Over or Player Win text
 * 
 * @param bGameOver Flag which screen should be shown
 * @return void
 */
void gameOver(bool bGameOver = true);

/**
 * Get a maybe new step-position from encoder
 * 
 * @param iStep Return positiv or negativ step(s) from encoder
 * @return bool Returns true if position changed
 */
bool getNewPos(int& iStep);

/**
 * Select from array the new word, that should be used to guess
 * 
 * @return void
 */
void newWord();

/**
 *  Draw basic board layout at the beginning of the game
 * 
 * @return void
 */
void drawBoard();

/**
 * Draw the selected letter, after encoder position was changed
 * 
 * @return void
 */
void drawLetterSelection();

/**
 * Check if the selected letter ist used before
 * 
 * @param charSelectLetter  Reference to selected letter
 * @return bool true if letter is used, otherwise false
 */
bool checkSelectedLetterUsed(const char& charSelectLetter);

/**
 * OVERLOADED FUNCTION
 * Check if the selected letter ist used before
 * 
 * @param strList   Reference to a StringList
 * @param charSelectLetter  Reference to selected letter
 * @return bool true if letter is used, otherwise false
 */
void checkSelectedLetterUsed(String& strList, const char& charSelectLetter);

/**
 * Redraw selected letter in LCD display
 * 
 * @return void
 */
void checkRedrawLetterSelection();

/**
 * General function to redraw and check selection of letter
 * 
 * @return voif
 */
void checkLetterSelection();

/**
 * Write text to LCD-Display
 * 
 * @param iRow  Row to show text
 * @param ePos  enum, see enum eTextPos, how to position text
 * @param iOffset if ePos == posTextOwnOffset, own position for first letter will be used
 * @return void
 */
void writeLCDText(int iRow = 0, int ePos = posTextLeft, int iOffset = 0);

/**
 * Overloaded function for String - Message
 * 
 * @param msg       Message that should be written to serial monitor
 * @param bNewLine  Flag to print with new line, default true
 * @return void
 */
bool buttonPressed();

//Private memeber (variables)
LiquidCrystal_I2C mLCD(LCD_ADDRESS,LCD_COLUMNS,LCD_ROWS);
Encoder mEncoder(DT, CLK);
long mOldPos = 0;
String mText = "";
byte mState = stateNewGameScreen;
bool mBtnReleased = true;
String mGuessWord = String(MAXLENGTHWORD);
byte mHangmanStatus = 0;
int mSelectLetter = 0;
unsigned int mTotalRight = 0;

const char* mWords[] = {"berrybase", "arduino", "code", "display", "giraffe", "katze", "motorrad", "dilemma", "abend"};
const char mLetterVal[] = "abcdefghijklmnopqrstuvwxyz";
int mGuessedIndex = 0;
char mGuessed[25];

byte mTopleft[] = { 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 };
byte mTopright[] = { 0x1C, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte mBottomleft[] = { 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x1F, 0x1F };
byte mBottomright[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte mHead[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x00, 0x00, 0x00 };
byte mTopbody[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x04, 0x04 };
byte mBottombody[] = { 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte mRightarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x05, 0x06, 0x04 };
byte mLeftarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x15, 0x0E, 0x04 };
byte mRightleg[] = { 0x04, 0x04, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00 };
byte mLeftleg[] = { 0x04, 0x04, 0x0A, 0x0A, 0x11, 0x00, 0x00, 0x00 };
byte mLeftarrow[] = { 0x10, 0x18, 0x1C, 0x1E, 0x1E, 0x1C, 0x18, 0x10 };
byte mRightarrow[] = { 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01 };

/* FOR DEBUGGING*/
#if DEBUG
/**
 * Writes serial messages to serial monitor, if firmware is build as
 * debug - Version.
 * 
 * @param msg       Message that should be written to serial monitor
 * @param bNewLine  Flag to print with new line, default true
 * @return void
 */
void writeSerialMessage(const char * msg, bool bNewLine = true);

/**
 * Overloaded function for INT - Message
 * 
 * @param msg       Message that should be written to serial monitor
 * @param bNewLine  Flag to print with new line, default true
 * @return void
 */
void writeSerialMessage(int msg, bool bNewLine = true);

/**
 * Overloaded function for String - Message
 * 
 * @param msg       Message that should be written to serial monitor
 * @param bNewLine  Flag to print with new line, default true
 * @return void
 */
void writeSerialMessage(String msg, bool bNewLine = true);
#endif