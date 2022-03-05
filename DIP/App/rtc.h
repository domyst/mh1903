typedef struct
{
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t dayOfWeek;
   uint8_t hours;
   uint8_t minutes;
   uint8_t seconds;
   uint16_t milliseconds;
} DateTime;

typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */

void convertUnixTimeToDate(time_t t, DateTime *date);
time_t convertDateToUnixTime(const DateTime *date);
