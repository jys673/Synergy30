/*
 *
 */
package edu.temple.cis.fsun.phd.util;

import java.util.*;

/**
 * ToDo List:
 *
 */
public class DateTool
{
    /**
     * Convert to format: ccyymmddhhMMss
     */
    public static String timeToString(Date date)
    {
        if (date == null)
            date = new Date();

        Calendar cal = Calendar.getInstance();
        cal.setTime(date);

        int year  = cal.get(Calendar.YEAR);
        int month = cal.get(Calendar.MONTH) + 1;
        int day   = cal.get(Calendar.DAY_OF_MONTH);

        String yearString = Integer.toString(year);
        if (yearString.length() == 3)
        {
            yearString = "0" + yearString;
        }
        else
        if (yearString.length() == 2)
        {
            yearString = "00" + yearString;
        }
        else
        if (yearString.length() == 1)
        {
            yearString = "000" + yearString;
        }

        String monthString = "";
        if (month < 10)
        {
            monthString = "0" + month;
        }
        else
        {
            monthString = Integer.toString(month);
        }

        String dayString   = "";
        if (day < 10)
        {
            dayString = "0" + day;
        }
        else
        {
            dayString = Integer.toString(day);
        }

        Integer temp  = new Integer(cal.get(Calendar.HOUR_OF_DAY));
        String  hours = temp.toString();
        if (hours.length() == 1)
        {
            hours = "0" + hours;
        }
        else
        if (hours.length() == 0)
        {
            hours = "00";
        }

        temp = new Integer(cal.get(Calendar.MINUTE));
        String minutes = temp.toString();
        if (minutes.length() == 1)
        {
            minutes = "0" + minutes;
        }
        else
        if (minutes.length() == 0)
        {
            minutes = "00" ;
        }

        temp = new Integer(cal.get(Calendar.SECOND));
        String seconds = temp.toString();
        if (seconds.length() == 1)
        {
            seconds = "0" + seconds;
        }
        else
        if (seconds.length() == 0)
        {
            seconds = "00" ;
        }

        String retVal = yearString + monthString + dayString +
                            hours + minutes + seconds;
        return retVal;
    }
}
