#ifndef STORMPCI_H
#define STORMPCI_H

#include <linux/types.h>

/**
 * \brief
 * \~english Capture Buffer goes from 0 - 511 = 512 bytes.
 * \~russian Размер буфера данных с платы. 512 байт.
 */
#define BOLTEK_BUFFERSIZE 512   

/**
 * \brief
 * \~english tSATELLITETYPE is the detailed satellite info from the gps
 * \~russian tSATELLITETYPE Подробная информация о спутнике
 */
struct StormProcess_tSATELLITETYPE
{
        /**
         * \brief  
         * \~english Satellite ID. 0..37
         * \~russian ID спутника. 0..37
         */
        char SVID; 

        /** 0..8
          * where 0 = code search   5 = message sync detect
          *       1 = code acquire  6 = satellite time available
          *       2 = AGC set       7 = ephemeris acquire
          *       3 = freq acquire  8 available for position
          *       4 = bit sync detect
          */
        char mode; 
        unsigned char signal_strength; ///< 0..255
        /** 16 bits
          *  where (msb) bit 15 = reserved
          *              bit 14 = reserved
          *              bit 13 = reserved
          *              bit 12 = narrow-band search mode
          *              bit 11 = channel used for time solution
          *              bit 10 = differential corrections available
          *              bit  9 = invalid data
          *              bit  8 = parity error
          *              bit  7 = channel used for position fix
          *              bit  6 = satellite momentum alert flag
          *              bit  5 = satellite anti-spoof flag set
          *              bit  4 = satellite reported unhealthy
          *              bit  3 = satellite accuracy (msb)
          *              bit  2 = satellite accuracy
          *              bit  1 = satellite accuracy 
          *              bit  0 = satellite accuracy (lsb)
          */
        unsigned short channel_status; 
};

/** \brief
  * \~english tTIMESTAMPINFO is the detailed gps and timestamp data
  * \~russian tTIMESTAMPINFO Детальная информация
  */
typedef struct 
{
        int TS_valid;
        unsigned long TS_Osc; ///< actual frequency of timestamp's 50MHz osc
        unsigned long TS_time; ///< 0..999,999,999 ns of trigger (вообще-то это число колебаний осциллятора)
        unsigned char TS_10ms; ///< 0..99 (сколько раз прошло по 10 ms с предыдущей секунды)
        unsigned long capture_time; ///< 0..999,999,999 ns of first peak

        int gps_data_valid;
        unsigned char month; ///< 1..12
        unsigned char day; ///< 1..31
        unsigned short year; ///< 1998 to 2079
        unsigned char hours; ///< 0..23
        unsigned char minutes; ///< 0..59
        unsigned char seconds; ///< 0..60
        int latitude_mas; ///< 324,000,000..324,000,000 (-90°..+90°)
        int longitude_mas; ///< 648,000,000..648,000,000 (-180°..+176°)
        char latitude_ns; ///< 'N' or 'S'
        char longitude_ew; ///< 'E' or 'W'
        int height_cm; ///< -100,000..+1,800,000 (-1000..+18,000m)
        unsigned short dop; ///< 0..999 (0.0 to 99.9 DOP)
        unsigned char satellites_visible; ///< 0..12
        unsigned char satellites_tracked; ///< 0..12
        struct StormProcess_tSATELLITETYPE satellite[12]; ///< individual satellite data

        /** 16 bits
          * where (msb) bit 15..13    111 = 3D fix
          *                           110 = 2D fix
          *                           101 = propogate mode
          *                           100 = position hold
          *                           011 = acquiring satellites
          *                           010 = bad geometry
          *                           001 = reserved
          *                           000 = reserved
          *             bit 12..11 = reserved
          *             bit 10 = narrow band tracking mode
          *             bit  9 = fast acquisition position
          *             bit  8 = filter reset to raw gps solution
          *             bit  7 = cold start (no almanac or almanac out of date or time & position unknown)
          *             bit  6 = differential fix
          *             bit  5 = position lock
          *             bit  4 = autosurvey mode
          *             bit  3 = insufficient visible satellites
          *             bit  2..1 = antenna sense
          *               where 00 = Ok
          *                     01 = over current
          *                     10 = under current
          *                    11 = no bias voltage
          *             bit  0 = code location
          */
        unsigned short receiver_status; 
        short oscillator_temperature; ///< -110..250 half degrees C (-55..+125°C)
        short serial_number;
} StormProcess_tTIMESTAMPINFO;


typedef struct StormProcess_tBOARDDATA_t
{
        int EFieldBuf[BOLTEK_BUFFERSIZE]; ///< essentially a 1 bit a-to-d converter
        int NorthBuf[BOLTEK_BUFFERSIZE]; ///< 0-255 data, we use int so we have room to filter the data
        int EastBuf[BOLTEK_BUFFERSIZE]; ///< 0-255 data,  we use int so we have room to filter the data
        
        StormProcess_tTIMESTAMPINFO lts2_data; ///< timestamp and gps data
        int NorthMaxPos, NorthMinPos, EastMaxPos, EastMinPos; ///< pos of signal peaks
        int North_Pk, East_Pk;               ///< signal pk-pk amplitude
        int NorthPol, EastPol, EFieldPol;         ///< signal polarity
} StormProcess_tBOARDDATA;


/// this is the structure passed to the device driver GET_DATA ioctl
typedef struct stormpci_packed_data
{
        __u16 usNorth[BOLTEK_BUFFERSIZE];
        __u16 usWest [BOLTEK_BUFFERSIZE];
} StormProcess_tPACKEDDATA;


typedef struct StormProcess_tSTRIKE {
	int valid; ///< data appars to be valid signal, not just noise
	float distance;  ///< miles away, for close strike detection
	float distance_averaged;  ///< miles away, for close strike detection
	float direction; ///< 0-360 degrees
} StormProcess_tSTRIKE;


#define STORMTRACKER_DEVICE_NAME "/dev/lightning-0"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief
 * \~english connect to the StormTracker card - non-zero on success
 * \~russian Инициализация PCI-карты Boltek StromTracker. Возвращает не 0 в случае успеха.
 */
int  StormPCI_OpenPciCard(void); 

/** \brief
 * \~english clean up, all done
 * \~russian Деинициализация PCI-карты Boltek StromTracker.
 */
void StormPCI_ClosePciCard(void); 

/** \brief
 * \~english after reading the data, wait for the next strike
 * \~russian После считывания данных ждать следующую вспышку.
 */
void StormPCI_RestartBoard(void); 

/** \brief
 * \~english force StormTracker to give you a capture
 * \~russian Принудить PCI-карту Boltek StromTracker очуществить захват данных.
 */
void StormPCI_ForceTrigger(void); 

/** \brief
 * \~english check if a strike is waiting to be read by GetCapture()
 * \~russian Проверить, не захвачена ли вспышка. Возвращает не ноль, если захвачена.
 */
int  StormPCI_StrikeReady(void); 

/** \brief
 * \~english 0-15, 0:most sensitive (preferred), 15: least sensitive
 * \~russian Установить чувствительность, значения из диапазона 0-15. 0 - самая высокая чуствительность, 15 - самая низкая.
 */
void StormPCI_SetSquelch(char trig_level);

/** \brief
 * \~english retrieve the waiting capture
 * \~russian Получить упакованные захваченные данные с PCI-платы Boltek StormTracker.
 * 
 * \param board_data
 * \~english Packed data
 * \~russian Упакованные даные будут помещены в структуру по этому адресу.
 */
void StormPCI_GetBoardData(StormProcess_tPACKEDDATA* board_data);

/** \brief
 * \~english Unpack raw data from board. GPS timestamps, 1 bit electric field and two 8 bit magnetic fields extracted.
 * \~russian Распаковать первичные данные с платы: метку точного времени, 1-битное электрическое поле и два перпендикулярных 8-битных магнитных поля.
 *
 * \param packed_data
 * \~english Raw data from board
 * \~russian Первичные данные с платы
 *
 * \param board_data
 * \~english Unpacked data
 * \~russian Распакованные данные: метка точного времени, 1-битное электрическое поле и два перпендикулярных 8-битных магнитных поля.
 */
void StormProcess_UnpackCaptureData(StormProcess_tPACKEDDATA *packed_data, StormProcess_tBOARDDATA* board_data);

/**
 * \brief
 * \~english Process capture data: calculate lightning direction and distance.
 * \~russian Обработка распакованного сигнала: определение положения вспышки.
 */
StormProcess_tSTRIKE StormProcess_SSProcessCapture(StormProcess_tBOARDDATA* capture);

#ifdef __cplusplus
}
#endif


#endif

