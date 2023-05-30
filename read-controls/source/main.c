#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

void printTitle()
{
    printf("CHECK CONTROLS\n\n");
    printf("please held any button 2 seconds to exit\n\n");
    printf("\x1b[5;1HLeft stick position:\n");
    printf("\x1b[7;1HRight stick position:\n");
};

// https://switchbrew.github.io/libnx/hid_8h.html#a218ac0c6184a8e98e58c45fe64c3668bac70da0a5250487aade1692c46e630ce9
// 检测当前按下的按键是不是摇杆
bool checkStick(u64 kDown)
{
    if(kDown & HidNpadButton_StickLLeft
        ||  kDown & HidNpadButton_StickLUp
        ||  kDown & HidNpadButton_StickLRight
        ||  kDown & HidNpadButton_StickLDown
        ||  kDown & HidNpadButton_StickRLeft
        ||  kDown & HidNpadButton_StickRUp
        ||  kDown & HidNpadButton_StickRRight
        ||  kDown & HidNpadButton_StickRDown)
        return true;

    return false;
};

int main(int argc, char* argv[])
{

    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    //Matrix containing the name of each key. Useful for printing when a key is pressed
    char keysNames[28][32] = {
        "A", "B", "X", "Y",
        "L3", "R3", "L", "R",
        "ZL", "ZR", "+", "-",
        "Left", "Up", "Right", "Down",
        "StickLLeft", "StickLUp", "StickLRight", "StickLDown",
        "StickRLeft", "StickRUp", "StickRRight", "StickRDown",
        "LeftSL", "LeftSR", "RightSL", "RightSR",
    };
    
    char temp[10][32];
    int tempIndex = 0;

    u64 startTime = 0;
    int exitProgram = 0;
    //定位输出按键的行号，确保可以换行输出按键信息
    int len = 10;

    printTitle();

    // Main loop
    while(appletMainLoop())
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        u64 kDown = padGetButtonsDown(&pad);
        u64 kUp = padGetButtonsUp(&pad);

        // ===========================持续按下任意按键2秒退出（不包括推摇杆操作）===========================
        if (kDown != 0 && !checkStick(kDown) && startTime == 0)
        {
            startTime = svcGetSystemTick();
        }

        if (kUp != 0 && startTime > 0)
        {
            startTime = 0;
        }

        if (startTime > 0)
        {
            u64 elapsedTime = svcGetSystemTick() - startTime;
            u64 elapsedTimeSeconds = (elapsedTime / 19200000);

            if(elapsedTimeSeconds >= 2) exitProgram = 1;
        }
        
        // ===========================按键模块===========================
        int i;
        for (i = 0; i < 28; i++)
        {
            if(kDown & BIT(i)) 
            {
                consoleClear();
                printTitle();
                len = 10;
                if(tempIndex < 10){
                    strcpy(temp[tempIndex++] , keysNames[i]);
                }else{
                    for (int i = 0; i < 9; ++i)
                        strcpy(temp[i] , temp[i + 1]);
                    
                    strcpy(temp[9] , keysNames[i]);
                }
                    for (int i = 0; i < 10; ++i)
                    {
                        printf("\x1b[%d;1H%s \n\n" , len , temp[i]);
                        len += 2;
                    }
            }
        }

        // ===========================摇杆模块===========================
        HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);
        HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);

        printf("\x1b[6;1H%06d; %06d\n", analog_stick_l.x, analog_stick_l.y);
        printf("\x1b[8;1H%06d; %06d\n\n", analog_stick_r.x, analog_stick_r.y);

        // ===========================退出程序===========================
        if (exitProgram)
        {
            break;
        }

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
