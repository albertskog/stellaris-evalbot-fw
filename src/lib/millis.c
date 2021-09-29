// Arduino-style millis() function

#include "millis.h"
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/systick.h>
#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>

static unsigned long millis_value = 0;

static void tick_interrupt(void);

void millis_init(void){

    // Trigger interrupt every millisecond
    SysTickPeriodSet(SysCtlClockGet()/1000);

    SysTickIntRegister(tick_interrupt);

    IntMasterEnable();

    SysTickIntEnable();

    SysTickEnable();
}

unsigned long millis(void){
    return millis_value;
}

// Systick interrupt handler
static void tick_interrupt(void) {
    millis_value++;
}
