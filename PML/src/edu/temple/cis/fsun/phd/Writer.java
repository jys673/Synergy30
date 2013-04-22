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
public class Writer
{
    private ArrayList buffer;

    public void write(StringBuffer output, String file) throws Exception
    {
        buffer = new ArrayList();

        Pattern pattern = Pattern.compile("\r*\n\r*");
        String[] parsed = pattern.split(output);
        for (int i = 0; i < parsed.length; i++)
        {
            buffer.add(parsed[i]);
        }

        // postprocessing the output
        dealingXmlChars();
        addIncludeMacro();

        PrintWriter writer = new PrintWriter(new BufferedWriter(
                                    new FileWriter(file)));
        for (int i = 0; i < buffer.size(); i++)
        {
            writer.println((String) buffer.get(i));
        }
        writer.close();
    }

    private void dealingXmlChars()
    {
        Pattern pattern = Pattern.compile(
"^\\s*/\\*\\s*(</?parallel|</?worker|</?data|</?target|</?reference).*\\*/\\s*$");

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
                    if (aLine.indexOf("@amp@") >= 0)
                    {
                        int pos = aLine.indexOf("<");
                        pos = aLine.indexOf("@amp@");
                        aLine = aLine.substring(0, pos) + "&" + 
                                aLine.substring(pos + 5);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("@lt@") >= 0)
                    {
                        int pos = aLine.indexOf("@lt@");
                        aLine = aLine.substring(0, pos) + "<"  + 
                                aLine.substring(pos + 4);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("@gt@") >= 0)
                    {
                        int pos = aLine.indexOf("@gt@");
                        aLine = aLine.substring(0, pos) + ">"  + 
                                aLine.substring(pos + 4);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("@quot@") >= 0)
                    {
                        int pos = aLine.indexOf("@quot@");
                        aLine = aLine.substring(0, pos) + "\""+ 
                                aLine.substring(pos + 6);
                        found = true;
                    }
                    else
                    if (aLine.indexOf("@apos@") >= 0)
                    {
                        int pos = aLine.indexOf("@apos@");
                        aLine = aLine.substring(0, pos) + "\'"+ 
                                aLine.substring(pos + 6);
                        found = true;
                    }
                }

                buffer.add(i, aLine);
                buffer.remove(i + 1);
            }
        }
    }

    private void addIncludeMacro()
    {
        buffer.add(0, "#include \"parallel.h\"");
    }
}
