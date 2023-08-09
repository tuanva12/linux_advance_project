/*
 ******************************************************************************
 * @file      : romconfig.h
 * @Created on: Aug 1, 2023
 * @author    : Tuann
 ******************************************************************************
 * @brief
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ******************************************************************************/

#ifndef __APP_SAVEDATA_H__
#define __APP_SAVEDATA_H__

/* Includes -----------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
/* Public typedef -----------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/

/* Public macro -------------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

/* Public function prototypes -----------------------------------------------*/

/* Public user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*                              FUNCTION                                      */
/******************************************************************************/

class MYSYSTEM
{
private:
    std::string rootFolder;
    bool flag100ms = false;
    pthread_t pcbThread100ms;

    void timeCallback100ms(void);

public:
    void mainRoutine(void);
};

#endif /* __APP_SAVEDATA_H__ */