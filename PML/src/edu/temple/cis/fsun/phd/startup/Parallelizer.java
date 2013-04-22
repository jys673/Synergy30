/*
 *
 */
package edu.temple.cis.fsun.phd.startup;

import edu.temple.cis.fsun.phd.*;
import edu.temple.cis.fsun.phd.core.*;
import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Parallelizer
{
    public static final String CONFIG_FILE = "conf/config.xml";

    private String configFile = Parallelizer.CONFIG_FILE;
    private boolean isDebug = false;
    private String sourceFile;

    public static void main(String[] args)
    {
        (new Parallelizer()).process(args);
    }

    public void process(String args[])
    {
        setParallelizerHome();
        System.out.println("Parallelizer - Home Directory = " +
                    System.getProperty("parallelizer.home"));

        if (!arguments(args)) return;

        Logger.log("Parallelizer starts processing with the source file <" + 
                    sourceFile + "> --------------");
        Logger.log("Result files will be stored in the same directory as " + 
                   "the source file");
        try
        {
            Root theRoot = new Parser().parse(sourceFile);
            new Transformer().transform(theRoot);
            new Generator().generate(theRoot, sourceFile);
        }
        catch (Exception e)
        {
            Logger.log("!!! Processing ERROR: " + e.getMessage());
            if (isDebug) e.printStackTrace();
            System.exit(-1);
        }

        Logger.log("Parallelizer ends processing --------------");
    }

    private void setParallelizerHome()
    {
        if (System.getProperty("parallelizer.home") != null)
            return;

        System.setProperty("parallelizer.home", System.getProperty("user.dir"));
    }

    private boolean arguments(String args[])
    {
        if (args.length < 1)
        {
            usage();
            return false;
        }

        boolean isConfig = false;
        boolean isSource = false;
        for (int i = 0; i < args.length; i++)
        {
            if (args[i].equals("-config"))
            {
                isConfig = true;
            }
            else
            if (isConfig)
            {
                configFile = args[i];
                isConfig = false;
            }
            else
            if (args[i].equals("-debug"))
            {
                isDebug = true;
            }
            else
            if (args[i].equals("-source"))
            {
                isSource = true;
            }
            else
            if (isSource)
            {
                sourceFile = args[i];
                isSource = false;
            }
            else
            if (args[i].equals("-help"))
            {
                usage();
                return false;
            }
            else
            {
                usage();
                return false;
            }
        }

        return true;
    }

    private void usage()
    {
        System.out.println(
        "usage: java edu.temple.cis.fsun.phd.startup.Parallelizer " +
        "[-config {pathname}] [-debug] -source {pathname} | -help ");
    }
}
