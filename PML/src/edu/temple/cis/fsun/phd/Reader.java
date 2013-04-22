/*
 *
 */
package edu.temple.cis.fsun.phd;

import java.util.*;
import java.util.regex.*;
import java.io.*;

/**
 *
 */
public class Reader
{
    private StringBuffer input;

    private ArrayList buffer;

    public StringBuffer read(String file) throws Exception
    {
        buffer  = new ArrayList();

        BufferedReader reader = new BufferedReader(new FileReader(file));
        String aLine = reader.readLine();
        while (aLine != null)
        {
            buffer.add(aLine);
            aLine = reader.readLine();
        }
        reader.close();

        // preprocessing the input
        dealingXmlChars();
        cleanUpTagLines();
        addWrappingTags();

        input = new StringBuffer();
        for (int i = 0; i < buffer.size(); i++)
        {
            input.append((String) buffer.get(i) + "\n");
        }
        return input;
    }

    private void dealingXmlChars()
    {
        Pattern pattern = Pattern.compile(
"^\\s*/\\*\\s*(</?parallel|</?master|</?worker|</?send|</?read|</?target|</?reference).*\\*/\\s*$");

        // remove the comments wrapping around each xml tag
        for (int i = 0; i < buffer.size(); i++)
        {
            String aLine = (String) buffer.get(i);
            Matcher matcher = pattern.matcher(aLine);

            if (!matcher.matches())
            {
                boolean found = true;
                while (found)
                {
                    found = false;
                    if (aLine.indexOf("&") >= 0)
                    {
                        int pos = aLine.indexOf("&");
                        aLine = aLine.substring(0, pos) + "@amp@" + 
                                aLine.substring(pos + 1);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("<") >= 0)
                    {
                        int pos = aLine.indexOf("<");
                        aLine = aLine.substring(0, pos) + "@lt@"  + 
                                aLine.substring(pos + 1);
                        found = true;
                    }
                    else
                    if (aLine.indexOf(">") >= 0)
                    {
                        int pos = aLine.indexOf(">");
                        aLine = aLine.substring(0, pos) + "@gt@"  + 
                                aLine.substring(pos + 1);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("\"") >= 0)
                    {
                        int pos = aLine.indexOf("\"");
                        aLine = aLine.substring(0, pos) + "@quot@"+ 
                                aLine.substring(pos + 1);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("\'") >= 0)
                    {
                        int pos = aLine.indexOf("\'");
                        aLine = aLine.substring(0, pos) + "@apos@"+ 
                                aLine.substring(pos + 1);
                        found = true;
                    }
                }

                buffer.add(i, aLine);
                buffer.remove(i + 1);
            }
        }
    }

    private void cleanUpTagLines()
    {
        Pattern pattern = Pattern.compile(
"^\\s*/\\*\\s*(</?parallel|</?master|</?worker|</?send|</?read|</?target|</?reference).*\\*/\\s*$");

        // remove the comments wrapping around each xml tag
        for (int i = 0; i < buffer.size(); i++)
        {
            String aLine = (String) buffer.get(i);
            Matcher matcher = pattern.matcher(aLine);

            if (matcher.matches())
            {
                aLine = aLine.replaceAll("/\\*", "");
                aLine = aLine.replaceAll("\\*/", "");

                buffer.add(i, aLine);
                buffer.remove(i + 1);
            }
        }
    }

    private void addWrappingTags()
    {
        buffer.add(0, 
        "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>");

        buffer.add(1, "<!DOCTYPE root SYSTEM \"./conf/parallel.dtd\">");

        buffer.add(2, "<root>");
        buffer.add("</root>");
    }
}
