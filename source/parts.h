/*
 *  parts.h
 *
 * Created:5/11/2020
 * Author: Spence Konde  (tindie.com/stores/drazzy)
 * github.com/SpenceKonde
 */

#ifndef  PARTS_H_
#define  PARTS_H_

#include <avr/io.h>

#if  (defined(__AVR_ATtiny3217__) || defined(__AVR_ATtiny1617__) || defined(__AVR_ATtiny1604__) || defined(__AVR_ATtiny1607__) || \
      defined(__AVR_ATtiny3216__) || defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny1606__) || defined(__AVR_ATtiny1614__))

      #define __AVR_ATtiny_Zero_One__


#elif (defined(__AVR_ATmega16__)    || defined(__AVR_ATmega32__)    || defined(__AVR_ATmega164A__)  || defined(__AVR_ATmega164PA__) || \
       defined(__AVR_ATmega324A__)  || defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega324PB__) || defined(__AVR_ATmega644A__)  || \
       defined(__AVR_ATmega644PA__) || defined(__AVR_ATmega1284__)  || defined(__AVR_ATmega1284P__))

      #define __AVR_ATmega_Mighty__


#elif  (defined(__AVR_ATmega641__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__) || \
        defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__))

      #define __AVR_ATmega_Mega__


#elif (defined(__AVR_ATmega328__)   || defined(__AVR_ATmega328P__) ||defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || \
       defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega168PB__) )

      #define __AVR_ATmega_Mini__

#elif (defined(__AVR_ATmega808__) ||defined(__AVR_ATmega1608__) ||defined(__AVR_ATmega3208__) ||defined(__AVR_ATmega4808__) || \
       defined(__AVR_ATmega808__) ||defined(__AVR_ATmega1608__) ||defined(__AVR_ATmega3208__) ||defined(__AVR_ATmega4808__) )

      #define __AVR_ATmega_Zero__

#endif
#endif
