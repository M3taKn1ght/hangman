#include "main.h"

void setup()
{
  mGuessWord = "";
  pinMode(BUTTON, INPUT_PULLUP);
  randomSeed(analogRead(A5));
#if DEBUG
  Serial.begin(DEBUG_BAUDRATE);
  while(!Serial) { }
#endif
  mLCD.init();       //Init LCD
  mLCD.backlight();  //Backlight on
  mLCD.clear();      //Clear old content
  mText = "HANGMAN";
  writeLCDText(0, posTextMiddle);
  mText = "(c) Joern Weise";
  writeLCDText(1, posTextMiddle);
  delay(5000);

}

void loop()
{
  static bool bButtonPressed = false;
  bButtonPressed = buttonPressed();
  //Statemachine for the whole process
  switch (mState)
  {
  case stateNewGameScreen:
    mLCD.clear();
    mText = "To start game";
    writeLCDText(0, posTextMiddle);
    mText = "press button";
    writeLCDText(1, posTextMiddle);
    mState = statNewGame;
    break;
  case statNewGame:
    if(bButtonPressed)
    {
      mState = statInitGame;
    }
    break;
  case statInitGame:
    if(mBtnReleased)
    {
      memset(&mGuessed[0], 0, sizeof(mGuessed));
      mSelectLetter = 0;
      mHangmanStatus = 0;
      mTotalRight = 0;
      mGuessedIndex = 0;
      newWord();
      drawBoard();
      drawLetterSelection();
      mState = statWaitLetter;
    }
    break;
  case statWaitLetter:
    checkRedrawLetterSelection();
    if(bButtonPressed)
    {
      mState = statCheckLetter;
    }
    break;
  case statCheckLetter:
    if(mBtnReleased)
    {
      checkLetterSelection();
    }
    break;
  case statWin:
    gameOver(false);
    mState = stateNewGameScreen;
    break;
  case statGameOver:
    gameOver();
    mState = stateNewGameScreen;
    break;
  default:
    mState = stateNewGameScreen;
    break;
  }
  
}

void drawHangman()
{
  mHangmanStatus++;
  switch (mHangmanStatus){
    case 1:  
      mLCD.createChar(1, mHead);           // head
      break;
    case 2:
      mLCD.createChar(1, mTopbody);        // body
      mLCD.createChar(3, mBottombody);
      break;
    case 3:
      mLCD.createChar(1, mRightarm);       // right arm
      break;
    case 4:
      mLCD.createChar(1, mLeftarm);        // left arm
      break;
    case 5:
      mLCD.createChar(3, mRightleg);       // right leg
      break;
    case 6:
      mLCD.createChar(3, mLeftleg);        // left leg
      break;
    default:
      break;
  }
}

void gameOver(bool bGameOver)
{
  if(bGameOver) //Player lose game
  {
    mLCD.clear();      //Clear old content
    mText = "GAME";
    writeLCDText(0, posTextMiddle);
    mText = "OVER";
    writeLCDText(1, posTextMiddle);
  }
  else  //Player wins the game
  {
    mLCD.clear();      //Clear old content
    mText = "PLAYER";
    writeLCDText(0, posTextMiddle);
    mText = "WIN";
    writeLCDText(1, posTextMiddle); 
  }
  delay(5000);
}

bool getNewPos(int& iStep)
{
  long currentPos = mEncoder.read();  //Read new position

  if(currentPos % INTERRUPTSPERPOS == 0)  //One step trigger 4 pulses
  {
    currentPos = currentPos / INTERRUPTSPERPOS;
    if(currentPos != mOldPos)
    {
      iStep = currentPos - mOldPos;
      mOldPos = currentPos;
      return true;
    }
  }
  iStep = 0;
  return false;
}

void writeLCDText(int iRow, int ePos, int iOffset)
{
  if(mText.length() != 0 && mText.length() < LCD_COLUMNS && iRow >= 0  && iRow < LCD_ROWS)
  {
    switch(ePos)
    {
      case posTextLeft:
        mLCD.setCursor(0,iRow);
        break;
      case posTextMiddle:
        mLCD.setCursor((unsigned int)(LCD_COLUMNS - 1 - mText.length())/2 , iRow);
        break;
      case posTextRight:
        mLCD.setCursor(LCD_COLUMNS - 1 - mText.length(), iRow);
        break;
      default:
        if(iOffset > 0 && iOffset < LCD_COLUMNS - 1)
          mLCD.setCursor(iOffset, iRow);
        break;
    }
    mLCD.print(mText);
  }
}

void newWord()
{
  int iPos = random(NUMWORDS);
  const char* pickWord = mWords[iPos];
  mGuessWord = pickWord;
}

bool buttonPressed()
{
  static int iLastButtonState = HIGH;
  static int iCurrentStat = HIGH;
  iCurrentStat = digitalRead(BUTTON);
  if(iCurrentStat != iLastButtonState)
  {
    if(iCurrentStat == HIGH)
    {
      mBtnReleased = true;
    }
    else
    {
      mBtnReleased = false;
    }
    iLastButtonState = iCurrentStat;
    return !iLastButtonState;
  }
  return false;
}

void drawBoard()
{
  mLCD.createChar(0, mTopleft);
  mLCD.createChar(1, mTopright);
  mLCD.createChar(2, mBottomleft);
  mLCD.createChar(3, mBottomright);
  mLCD.createChar(4, mLeftarrow);
  mLCD.createChar(5, mRightarrow);

  // draw blank hangman table
  mLCD.clear();
  mLCD.home();
  mLCD.write(0);
  mLCD.write(1);
  mLCD.setCursor(0,1);
  mLCD.write(2);
  mLCD.write(3);

  mText = "";
  //Show number of letters with "_"
  for (unsigned int i = 0; i < mGuessWord.length(); i++)
  {
    mText+=("_");
  }
  writeLCDText(1, posTextOwnOffset, 3);
}

void drawLetterSelection()
{
  mText = "";
  //Show the letters before selected letter
  for (int x = 5; x >= 1 ; x--)
  {
    if (mSelectLetter - x >= 0)
    {
      checkSelectedLetterUsed(mText, mLetterVal[mSelectLetter - x]);
    }
    else  //Here we are in an overflow to last letters
    {
      checkSelectedLetterUsed(mText, mLetterVal[26 - (x - mSelectLetter)]);
    }
  }
  writeLCDText(0, posTextOwnOffset, 3);
  mLCD.write(4);  //left arrow
  //Selected letter (maybe used?)
  if(!checkSelectedLetterUsed(mLetterVal[mSelectLetter]))
    mText = mLetterVal[mSelectLetter];
  else
    mText = "*";
  writeLCDText(0, posTextOwnOffset, 9);  //Right arrow
  mLCD.write(5);
  mText = "";
  //Letters after selected letter
  for (int x=1; x <= 5 ; x++)
  {
    if (mSelectLetter + x <= 25)
    { 
      checkSelectedLetterUsed(mText, mLetterVal[mSelectLetter + x]);
    }
    else
    {
      checkSelectedLetterUsed(mText, mLetterVal[mSelectLetter + x - 26]);
    }
  }
  writeLCDText(0, posTextOwnOffset, 11);
}

bool checkSelectedLetterUsed(const char& charSelectLetter)
{
  unsigned int iNumElements = sizeof(mGuessed) / sizeof(mGuessed[0]);
  for(unsigned int i = 0; i < iNumElements; i++)
  {
    if(mGuessed[i] == charSelectLetter)
      return true;
  }
  return false;
}

void checkSelectedLetterUsed(String& strList, const char& charSelectLetter)
{
  if(!checkSelectedLetterUsed(charSelectLetter))
   strList += charSelectLetter;
  else
    strList += "*";
}

void checkRedrawLetterSelection()
{
  static int iStep = 0;
  if(getNewPos(iStep))  //Do the encoder get new position?
  {
    mSelectLetter += iStep;
    if(mSelectLetter < 0)
    {
      mSelectLetter = 25;
    }
    if (mSelectLetter >= 26)
    {
      mSelectLetter = 0;
    }
    drawLetterSelection();
  }
}

void checkLetterSelection()
{
  if(checkSelectedLetterUsed(mLetterVal[mSelectLetter]))
  {
    mState = statWaitLetter;
    return;
  }
  else
  {
    static bool foundLetter = false;
    foundLetter = false;
    for (unsigned int i = 0; i < mGuessWord.length(); i++)
    {
      mText = String(mLetterVal[mSelectLetter]);
      if (mGuessWord.substring(i, i+1) == mText)
      {
        foundLetter = true;
        mTotalRight++;
        writeLCDText(1, posTextOwnOffset, i+3);
      }
    }
    if(mTotalRight >= mGuessWord.length())
    {
      mState = statWin;
      return;
    }
    if(!foundLetter)
    {
      if(mHangmanStatus < 6)
      {
        mGuessed[mGuessedIndex] = mLetterVal[mSelectLetter];
        mGuessedIndex++;
        drawLetterSelection();
        drawHangman();
        mState = statWaitLetter;
        return;
      }
      else
      {
        
        mState = statGameOver;
        return;
      }
    }
    else
    {
      mGuessed[mGuessedIndex] = mLetterVal[mSelectLetter];
      mGuessedIndex++;
      drawLetterSelection();
      mState = statWaitLetter;
      return;
    }
  }
}
/** FOR DEBUGGING */
#if DEBUG
void writeSerialMessage(const char * msg, bool bNewLine)
{
  bNewLine == true ? Serial.println(msg) : Serial.print(msg);
}

void writeSerialMessage(int msg, bool bNewLine)
{
  bNewLine == true ? Serial.println(msg) : Serial.print(msg);
}

void writeSerialMessage(String msg, bool bNewLine)
{
  bNewLine == true ? Serial.println(msg) : Serial.print(msg);

}
#endif