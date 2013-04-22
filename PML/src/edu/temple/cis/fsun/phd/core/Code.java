/*
 *
 */
package edu.temple.cis.fsun.phd.core;

import java.util.*;

import edu.temple.cis.fsun.phd.util.*;

/**
 *
 */
public class Code extends Node
{
    private StringBuffer code;

    public Code(Node p) throws Exception
    {
        this();
        super.setup(p, null);
    }

    public Code() { name = "CODE"; }

    public void setup(Node p, HashMap attrs) throws Exception
    {
        super.setup(p, null);
    }

    public boolean isValid()
    {
        if (parent == null)
        {
            Logger.log("Invalid " + name + ": parent node is null!");
            return false;
        }
        if (kids != null)
        {
            Logger.log("Invalid " + name + ": kids is NOT null!");
            return false;
        }
        if (code == null)
        {
            Logger.log("Invalid " + name + ": NO code for the tag!");
            return false;
        }
        return true;
    }

    public Code makeCode() throws Exception
    {
        return this;
    }

    public void addCode(String text)
    {
        if (code == null)
            code = new StringBuffer();
        code.append(text);
    }

    public void setCode(StringBuffer code) { this.code = code; }
    public StringBuffer getCode() { return this.code; }
}
