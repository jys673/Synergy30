/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Parallel extends Node
{
    private String appName;

    public Parallel(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Parallel()
    {
        name = "PARALLEL";

        // Commented off. Parallel-scope declarations are moved into the header
        // file "parallel.h", which will be included into both generated master
        // and worker files.
        // 
        // Add parallel-scope declarations immediately
        // setParallelDecls();
    }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);

        if (attrs != null && attrs.size() > 0)
        {
            appName = (String) attrs.get("appname");
        }

        if (appName == null)
            throw new Exception("Invalid <" + name + "> Tag!");
    }

    public boolean isValid()
    {
        if (parent == null || !(parent instanceof Root))
        {
            Logger.log("Invalid " + name + ": parent node is NOT Root!");
            return false;
        }
        if (kids == null || kids.size() == 0)
        {
            Logger.log("Invalid " + name + ": NO kids!");
            return false;
        }
        if (appName == null)
        {
            Logger.log("Invalid " + name + ": 'appname' is null!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        Code tagStart = new Code();
        tagStart.addCode("\n/* <parallel appname=\"" + appName + "\"> */\n");

        Code tagEnd = new Code();
        tagEnd.addCode("\n/* </parallel> */\n");
        this.addChild(tagEnd);

        return tagStart;
    }

    private void setParallelDecls()
    {
        Declaration decl = null;
        int at = 0;

        try
        {
            boolean isNewPara = kids == null? true : false;

            if (isNewPara) decl = new Declaration(this);
            else
            {
                decl = new Declaration();
                kids.add(at++, decl);
                decl.setParent(this);
            }
            decl.setType("int");
            decl.setVar("_distributor");

            if (isNewPara) decl = new Declaration(this);
            else
            {
                decl = new Declaration();
                kids.add(at++, decl);
                decl.setParent(this);
            }
            decl.setType("int");
            decl.setVar("_constructor");

            if (isNewPara) decl = new Declaration(this);
            else
            {
                decl = new Declaration();
                kids.add(at++, decl);
                decl.setParent(this);
            }
            decl.setType("int");
            decl.setVar("_status");
            decl.setInit("0");

            if (isNewPara) decl = new Declaration(this);
            else
            {
                decl = new Declaration();
                kids.add(at++, decl);
                decl.setParent(this);
            }
            decl.setType("char");
            decl.setVar("_tp_name[20]");

            if (isNewPara) decl = new Declaration(this);
            else
            {
                decl = new Declaration();
                kids.add(at++, decl);
                decl.setParent(this);
            }
            decl.setType("int");
            decl.setVar("_tp_size");
            decl.setInit("0");
        }
        catch (Exception e)
        {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    public void addAtTop(ArrayList nodes)
    {
        for (int i = 0; i < kids.size(); i++)
        {
            if (!(kids.get(i) instanceof Declaration))
            {
                kids.addAll(i, nodes);
                break;
            }
        }
    }

    public void addAtTop(Node node)
    {
        for (int i = 0; i < kids.size(); i++)
        {
            if (!(kids.get(i) instanceof Declaration))
            {
                kids.add(i, node);
                break;
            }
        }
    }

    public void addAtEnd(ArrayList nodes)
    {
        kids.addAll(nodes);
    }

    public void addAtEnd(Node node)
    {
        kids.add(node);
    }

    public void setAppName(String an) { appName = an; }
    public String getAppName() { return this.appName; }
}
