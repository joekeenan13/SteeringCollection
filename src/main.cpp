#include <Arduino.h>
#include <SdFat.h>
#include <Bounce2.h>

#define ROTPOTPIN A0
#define BUTTONPIN 15

void select_next_filename(char *buffer, SdFs *sd);

unsigned long t1 = 0;
unsigned long t2 = 0;

char filename[32];

uint16_t rotPotData;
Button button = Button();

SdFs sd;
FsFile file;

bool logging = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
    button.attach(BUTTONPIN, INPUT_PULLUP);
    button.interval(100);
    button.setPressedState(0);
}

void loop() {
    t1 = millis();
    button.update();


    if((t1 - t2) > 10) {
        rotPotData = analogRead(ROTPOTPIN);
        if(logging) {
            file.printf("%d, %d", t1, rotPotData);
        }
    }


    if(button.read()) {
        if(logging) {
            Serial.println("Closing file");
            file.truncate();
            file.flush();
            file.sync();
            file.close();
            logging = false;
        } else {
            if (!sd.begin(BUILTIN_SDCARD)) {
                if (sd.sdErrorCode()) {
                    if (sd.sdErrorCode() == SD_CARD_ERROR_ACMD41) {
                        Serial.println("Try power cycling the SD card.");
                    }
                    sd.printSdError(&Serial);
                }
                return;
            }
            Serial.println("Opening file");
            select_next_filename(filename, &sd);
            if (!file.open(filename, O_RDWR | O_CREAT)) {
                return;
            }

            file.printf("Time (s), RotPot\n");
            logging = true;
        }
    }

    

    t2 = t1;
}

void select_next_filename(char *buffer, SdFs *sd) { //Passed buff should be of size FILENAME_SIZE
    for (int fileNum = 0; fileNum < 1000; fileNum++) {
        char fileNumber[5]; //4-character number + null
        sprintf(fileNumber, "%d", fileNum);
        strcpy(buffer, "DAT");
        strcat(buffer, fileNumber);
        strcat(buffer, ".csv");
        //debugl(buffer);
        if (!sd->exists(buffer)) {
            return;
        }
    }
}