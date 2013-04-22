/*
 *
 */
package edu.temple.cis.fsun.phd;

import java.util.*;
import java.io.*;

import edu.temple.cis.fsun.phd.core.*;
import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Generator
{
    String workerDir;
    String workerExt;
    String appName;

    public void generate(Root theRoot, String file) throws Exception
    {
        Logger.log("\nNow generating code ... ...\n");

        setWorkerPath(file);

        StringBuffer output = new StringBuffer();
        produceCode(theRoot, output);

        String masterFile = workerDir + File.separator + 
                            appName + "_parallel" + workerExt;

        Logger.log("Write master file: " + masterFile);
        new Writer().write(output, masterFile);

        Logger.log("\nCode is generated ... ...\n");
    }

    private void produceCode(Node node, StringBuffer out) throws Exception
    {
        if (node instanceof Master)
        {
            buildWorker(((Master) node).getWorker());
        }

        Code code = node.makeCode();
        if (code != null) out.append(code.getCode());

        ArrayList kids = node.getKids();
        if (kids != null)
        {
            for (int i = 0; i < kids.size(); i++)
            {
                produceCode((Node) kids.get(i), out);
            }
        }
    }

    private void buildWorker(Worker worker) throws Exception
    {
        StringBuffer output = new StringBuffer();

        Parallel parallel = (Parallel) worker.getParent();
        Root theRoot = (Root) parallel.getParent();
        produceCode(theRoot, output);

        appName = parallel.getAppName();

        String workerFile = workerDir + File.separator + 
                            appName + "_" + worker.getId() + workerExt;

        Logger.log("Write worker file: " + workerFile);
        new Writer().write(output, workerFile);
    }

    private void setWorkerPath(String sourceFile)
    {
        File f = new File(sourceFile);

        workerDir = f.getParent();

        String name = f.getName();
        workerExt = name.indexOf(".") < 0? "" : 
                    name.substring(name.lastIndexOf("."));
    }
}
