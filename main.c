#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "delay.h"
#include "dogm162.h"
#include "drv_canfdspi_api.h"
#include "drv_canfdspi_defines.h"
#include "drv_canfdspi_register.h"
#include "mcp9808.h"

#define LED1_TOG PORTB ^= (1<<_PORTB_RB6_POSITION) /*changes the bit state to the opposite*/

CAN_RX_MSGOBJ rxObj;
CAN_RX_FIFO_EVENT rxFlags ;
uint8_t rxData[MAX_DATA_BYTES];
volatile uint8_t rx_interrupt_flags;
int main(void)
{
    /*initialize the device*/
    SYSTEM_Initialize();
    DRV_CANFDSPI_MY_Configure();
    lcd_Initialize();
    WpiszSwojeZnaki(); /*Celsius degree sign*/
    //TMR2_Start(); /*not used*/
    
    while (1)
    {
         if(rx_interrupt_flags) { //test interrupt flags
          lcd_Cls();
          DRV_CANFDSPI_ReceiveChannelEventGet(CAN_FIFO_CH1, &rxFlags) ; //get event
          if(rxFlags & CAN_RX_FIFO_NOT_EMPTY_EVENT) {//test event
          memset(rxData, 0, MAX_DATA_BYTES); /*clear RX Bufor*/
          /*read message and set UINC*/
          DRV_CANFDSPI_ReceiveMessageGet(CAN_FIFO_CH1, &rxObj, rxData, MAX_DATA_BYTES);
         /*Process message*/         
          lcd_Locate(1,1);
          lcd_String("Receive CAN FD: ");
          lcd_Locate(2,1);
          lcd_String(rxData);
          }
          rx_interrupt_flags =0;
        }
      } 
    }

/*Routime CN Interrupt for PORTA*/
void __attribute__ ((vector(_CHANGE_NOTICE_A_VECTOR), interrupt(IPL2SOFT))) CN_ISR()
{
    rx_interrupt_flags = 1;
    LED1_TOG;
    /*zerowanie statusu CN dla pinu RA2*/ 
    CNFAbits.CNFA2 = 0 ; /*clear status*/
    /*zerowanie flagi przerwania przerwania*/
    IFS0CLR = 1 << _IFS0_CNAIF_POSITION  ;
    
    
}



// void transmitMessage( char *msg )
// {
//     CAN_TX_FIFO_EVENT         txFlags;
//     CAN_TX_MSGOBJ             txObj;
//     uint8_t                   txData[MAX_DATA_BYTES];
//     bool                      flush;
//     
//     flush = true;
//     txObj.word[0] = 0;
//     txObj.word[1] = 0;
//     
//     txObj.bF.id.SID = 0x300; /*standard or Base ID*/
//     txObj.bF.id.EID = 0;
//     
//     txObj.bF.ctrl.FDF = 1; /*CAN FD frame*/
//     txObj.bF.ctrl.BRS = 1; /*switch bit rate*/
//     txObj.bF.ctrl.IDE = 0; /*standard frame*/
//     txObj.bF.ctrl.RTR = 0; /*not a remote frame request*/
//     txObj.bF.ctrl.DLC = CAN_DLC_64; /*65 data bytes*/
//     txObj.bF.ctrl.SEQ = 1; /*sequence: doesn't get transmitted but will be stored in TEF*/
//     
//     memset(txData, 0, MAX_DATA_BYTES); /*reset*/
//     strcpy(txData, msg);
//     DRV_CANFDSPI_TransmitChannelEventGet(CAN_FIFO_CH1, &txFlags);
//     
//     if( txFlags & CAN_TX_FIFO_NOT_FULL )
//     {
//         DRV_CANFDSPI_TransmitChannelLoad(CAN_FIFO_CH1, &txObj, txData, DRV_CANFDSPI_DlcToDataBytes(txObj.bF.ctrl.DLC), flush);
//         lcd_Locate(1,1);
//         lcd_String("CAN Message sent");
//         }
//     else
//     {
//         lcd_Locate(1,1);
//         lcd_String("Message not sent");
//     }
// }



  /* RAM access test */
//  void MCP2517FD_TEST_RAM_ACCESS(void)
//        {
//            // Variables
//            uint8_t txd[MAX_DATA_BYTES];
//            uint8_t rxd[MAX_DATA_BYTES];
//            uint8_t i;
//            uint8_t length;
//
//            // Verify read/write with different access length
//            // Note: RAM can only be accessed in multiples of 4 bytes
//            for (length = 4; length <= MAX_DATA_BYTES; length += 4) {
//                for (i = 0; i < length; i++) {
//                    txd[i] = rand() & 0xff;
//                    rxd[i] = 0xff;
//                }
//
//                // Write data to RAM
//                DRV_CANFDSPI_WriteByteArray(cRAMADDR_START, txd, length);
//
//                // Read data back from RAM
//                DRV_CANFDSPI_ReadByteArray(cRAMADDR_START, rxd, length);
//
//                // Verify
//                bool good = false;
//                for (i = 0; i < length; i++) {
//                    good = txd[i] == rxd[i];
//
//                    if (!good) {
//                       PORTBbits.RB6 = 1; /*on LED if inconformity tx and rx data*/
//
//                        // Data mismatch
//                    }
//                }
//            }
//
//           
//        }

            /* Register access test */
//   void MCP2517_TEST_REGISTER_ACCESS(void)
//        {
//            // Variables
//            uint8_t txd[MAX_DATA_BYTES];
//            uint8_t rxd[MAX_DATA_BYTES];
//            uint8_t i;
//            uint8_t length;
//
//            // Verify read/write with different access length
//            // Note: registers can be accessed in multiples of bytes
//            for (length = 1; length <= MAX_DATA_BYTES; length++) {
//                for (i = 0; i < length; i++) {
//                    txd[i] = rand() & 0x7f; // Bit 31 of Filter objects is not implemented , txd[i] <= 127
//                    rxd[i] = 0xff;
//                }
//                // Write data to registers
//                DRV_CANFDSPI_WriteByteArray(cREGADDR_CiFLTOBJ, txd, length);
//
//                // Read data back from registers
//                DRV_CANFDSPI_ReadByteArray(cREGADDR_CiFLTOBJ, rxd, length);
//
//                // Verify
//                bool good = false;
//                for (i = 0; i < length; i++) {
//                    good = txd[i] == rxd[i];
//
//                    if (!good) {
//                       PORTBbits.RB6 = 1; /*on LED if inconformity tx and rx data*/
//
//                        // Data mismatch
//                    }
//                }
//            }
//
//            Nop();
//            Nop();
//         
//        }
  
