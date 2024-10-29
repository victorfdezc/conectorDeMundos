#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); // Initialize TFT screen

// Calibration file and repeat calibration flag
#define CALIBRATION_FILE "/TouchCalData"
#define REPEAT_CAL false
#define SCREEN_HEIGHT 320 //pixels
#define SCREEN_WIDTH 240 //pixels

// Main app button labels and colors
const char *appLabels[6] = {"Role", "Chat", "Polls", "Story", "Game", "Settings"};
uint16_t appColors[6] = {TFT_RED, TFT_BLUE, TFT_GREEN, TFT_YELLOW, TFT_PURPLE, TFT_ORANGE};

// Button array
TFT_eSPI_Button appButtons[6];

// Function prototypes
void openApp(int appIndex);
void touch_calibrate();

// Draw the main menu with buttons positioned evenly on the screen
void drawMainMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&FreeSansBold12pt7b);
  
  int buttonWidth = 80;   // Width of each button
  int buttonHeight = 60;  // Height of each button
  int spacingX = 80;      // Horizontal spacing between buttons
  int spacingY = 80;      // Vertical spacing between rows of buttons
  int startX = 100;        // X offset for the first button
  int startY = 100;        // Y offset for the first button
  
  for (int i = 0; i < 6; i++) {
    int row = i / 3;                // Calculate row (0 or 1)
    int col = i % 3;                // Calculate column (0, 1, or 2)
    int x = startX + col * (buttonWidth + spacingX); // X position of the button
    int y = startY + row * (buttonHeight + spacingY); // Y position of the button
    
    appButtons[i].initButton(&tft, x, y, buttonWidth, buttonHeight, TFT_WHITE, appColors[i], TFT_WHITE, (char*)appLabels[i], 1);
    appButtons[i].drawButton();
  }
}

// Touch input handler for main menu
void handleMainMenuTouch() {
  uint16_t t_x, t_y;
  bool pressed = tft.getTouch(&t_x, &t_y);

  for (int i = 0; i < 6; i++) {
    if (pressed && appButtons[i].contains(t_x, t_y)) {
      appButtons[i].press(true);
    } else {
      appButtons[i].press(false);
    }

    if (appButtons[i].justPressed()) {
      appButtons[i].drawButton(true);  // Invert colors on press
      openApp(i);  // Open selected app
    } else if (appButtons[i].justReleased()) {
      appButtons[i].drawButton();  // Draw normal
    }
  }
}

// Placeholder function for opening an app
void openApp(int appIndex) {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(100, 100);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.print("Opening ");
  tft.println(appLabels[appIndex]);
  delay(1000);  // Temporary delay, replace with actual app code
  drawMainMenu();  // Return to main menu for now
}

// Calibration function for touch screen
void touch_calibrate() {
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Check if calibration file exists and size is correct
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL) {
      SPIFFS.remove(CALIBRATION_FILE);
    } else {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char*)calData, 14) == 14) {
          calDataOK = 1;
        }
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    tft.setTouch(calData);
  } else {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Touch corners as indicated");
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char*)calData, 14);
      f.close();
    }
  }
}

// Setup and loop functions
void setup() {
  Serial.begin(9600);
  tft.init();
  tft.setRotation(1);
  touch_calibrate();
  drawMainMenu(); // Draw initial menu
}

void loop() {
  handleMainMenuTouch();
}
